/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include <iostream>
#include <fstream>
#include <cstdio>
#include <stdio.h>

#include "../include/compiler.h"
#include "../include/output.h"
#include "../include/programstore.h"

#include <parser.hpp>

Compiler::Compiler(int argc, char *argv[])
        : debug(false), verbose(false)
{
    using namespace boost::program_options;
    std::string usage;
    usage += "Usage: ";
    usage += argv[0];
    usage += " [options] <file_list>";
    options_description description(usage);
    variables_map variables;
    try
    {
        variables = this->parseOptions(argc, argv, &description);
    }
    catch (...)
    {
        throw CompilerArgumentError("", description);
    }

    if (variables.count("help") > 0) throw CompilerArgumentError("", description);
    this->debug = variables.count("debug") > 0;
    this->verbose = variables.count("verbose") > 0;
    this->warnings = variables.count("warnings") > 0;

    this->output = (variables.count("output") > 0) ? variables["output"].as<std::string>().c_str() : "a.st";
    if (!std::freopen(this->output.c_str(), "w", stdout))
    {
        std::string output("Could not re-open stdout on file: ");
        output += variables["output"].as<std::string>();
        throw CompilerArgumentError(output, description);
    }

    if (variables.count("filename") < 1) throw CompilerArgumentError("", description);
    this->filenames = variables["filename"].as<std::vector<std::string> >();
}

boost::program_options::variables_map Compiler::parseOptions(int argc, char *argv[], boost::program_options::options_description * description)
{
    using namespace boost::program_options;
    description->add_options()
    ("help,h", "Produce help information.")
    ("debug,d", "Turn on the debug flag to have extremely verbose output.")
    ("verbose,v", "Turn on the verbose flag to have more verbose output.")
    ("filename,f", value<std::vector<std::string> >(), "The file(s) to compile.")
    ("output,o", value<std::string>(), "The output file.")
    ("warnings,w", "If passed denotes that warnings should be generated rather than errors in cases.")
    ;

    positional_options_description positional_arguments;
    positional_arguments.add("filename", -1);

    variables_map variables;
    store(command_line_parser(argc, argv).options(*description).positional(positional_arguments).run(), variables);
    notify(variables);
    return variables;
}

/**
 * @note Prototype for yyparse ...
 */
int yyparse(Generator::ProgramStore *);

void Compiler::Run()
{
    using namespace Generator;

    if (!this->filenames.empty())
        for (std::vector<std::string>::iterator i = this->filenames.begin(); i != this->filenames.end(); ++i)
        {
            if (!std::freopen(i->c_str(), "r", stdin))
            {
                std::string output("Could not re-open stdin on file: ");
                output += *i;
                throw CompilerError(output);
            }
            ProgramStore code;
            if (yyparse(&code) > 0)
            {
                std::remove(this->output.c_str());
                throw CompilerError("Parse Error Occured!");
            }
            std::cout << code.ToString() << std::endl;
        }
    else
    {
        ProgramStore code;
        if (yyparse(&code) > 0)
        {
            std::remove(this->output.c_str());
            throw CompilerError("Parse Error Occured!");
        }
        std::cout << code.ToString() << std::endl;
    }
}

CompilerError::CompilerError(const std::string &message)
        : message(message)
{
}

std::string CompilerError::GetMessage() const
{
    return this->message;
}

CompilerArgumentError::CompilerArgumentError(const std::string &message, const boost::program_options::options_description &description)
        : CompilerError(message), description(description)
{
}

boost::program_options::options_description CompilerArgumentError::GetDescription() const
{
    return this->description;
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
