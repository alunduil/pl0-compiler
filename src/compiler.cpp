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

#include <compiler.h>
#include <boost/program_options.hpp>
#include <parser.h>
#include <string>
#include <vector>
#include <symboltable.h>
#include <iostream>
#include <tokenizer.h>
#include <fstream>

Compiler::Compiler(int argc, char *argv[])
        : options("Options")
{
    this->parseOptions(argc, argv);
}

void Compiler::parseOptions(int argc, char *argv[])
{
    using namespace boost::program_options;
    options.add_options()
    ("help,h", "Produce help information.")
    ("debug,d", "Turn on the debug flag to have extremely verbose output.")
    ("filename,f", value<std::vector<std::string> >(), "The file to compile.")
    ("output,o", value<std::string>(), "The output file.")
    ("errors,e", value<int>(), "The maximum number of errors.")
    ;

    positional_options_description arguments;
    arguments.add("filename", -1);

    store(command_line_parser(argc, argv).options(options).positional(arguments).run(), this->vars);
    notify(this->vars);
}

void Compiler::Run()
{
    if (this->vars.count("help"))
    {
        std::cerr << options << std::endl;
        exit(EXIT_FAILURE);
    }

    if (this->vars.count("filename") <= 0) throw CompilerException();
    LexicalAnalyzer::Parser parser(this->vars["filename"].as<std::vector<std::string> >()[0], new Environment::SymbolTable(), (this->vars.count("debug") > 0) ? true : false, (this->vars.count("errors") > 0) ? this->vars["errors"].as<int>() : 100);
    try
    {
        parser.Parse();
        if (parser.HaveErrors())
            throw new LexicalAnalyzer::ErrorQueueError();
        std::ofstream out(((this->vars.count("output") > 0) ? this->vars["output"].as<std::string>().c_str() : "a.st"));
        out << parser.Code();
        out.close();
    }
    catch (LexicalAnalyzer::ErrorQueueError e)
    {
        throw e;
    }
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
