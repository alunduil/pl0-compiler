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

#include "../include/compiler.h"
#include "../include/output.h"
#include "../include/symboltable.h"
#include "../include/parser.h"

Compiler::Compiler(int argc, char *argv[])
        : debug(false), verbose(false), errors(25)
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

    if (variables.count("output") > 0 && !std::freopen(variables["output"].as<std::string>().c_str(), "w", stdout) || !std::freopen("a.st", "w", stdout))
    {
        std::string output("Could not re-open stdout on file: ");
        output += variables["output"].as<std::string>();
        throw CompilerArgumentError(output, description);
    }

    if (variables.count("filename") > 0 && !std::freopen(variables["filename"].as<std::vector<std::string> >()[0].c_str(), "r", stdin))
    {
        std::string output("Could not re-open stdin on file: ");
        output += variables["filename"].as<std::vector<std::string> >()[0];
        throw CompilerArgumentError(output, description);
    }

    if (variables.count("errors") > 0) this->errors = variables["errors"].as<int>();
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
    ("errors,e", value<int>(), "The maximum number of errors.")
    ("warnings,w", "If passed denotes that errors should be generated rather than errors in cases.")
    ;

    positional_options_description positional_arguments;
    positional_arguments.add("filename", -1);

    variables_map variables;
    store(command_line_parser(argc, argv).options(*description).positional(positional_arguments).run(), variables);
    notify(variables);
    return variables;
}

void Compiler::Run()
{
    using namespace Analyzer;
    using namespace Environment;

    try
    {
        Parser parser(std::cin, this->errors, this->verbose, this->debug, this->warnings);
        parser.Parse();
        if (parser.HaveErrors()) throw ErrorQueueError();
        std::cout << parser.Code();
    }
    catch (ErrorQueueError e)
    {
        throw e;
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
