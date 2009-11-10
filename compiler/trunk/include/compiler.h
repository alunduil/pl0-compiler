/*
    Application controller for a compiler of the PL/0 Language.
    Copyright (C) 2009 Alex Brandt

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

#ifndef COMPILER_H
#define COMPILER_H

#include <boost/program_options.hpp>

class Compiler
{
    public:
        /**
         * @brief Constructor.
         * @param argc Count of the arguments.
         * @param argv The arguments vector.
         */
        Compiler(int argc, char *argv[]);

        /**
         * @brief Run Application.
         */
        void Run();

    private:
        bool debug;
        bool verbose;
        int errors;
        bool warnings;

        /**
         * @brief Parse the command line options.
         * @param argc The count of the arguments.
         * @param argv The argument vector.
         * @param description The options description pointer.
         * @return A variables map containing all of the variables parsed.
         */
        boost::program_options::variables_map parseOptions(int argc, char *argv[], boost::program_options::options_description * description = new boost::program_options::options_description("Options: "));
};

class CompilerError
{
    public:
        std::string GetMessage() const;
        CompilerError(const std::string & msg);
    private:
        std::string message;
};

class CompilerArgumentError : public CompilerError
{
    public:
        boost::program_options::options_description GetDescription() const;

        CompilerArgumentError(const std::string & msg, const boost::program_options::options_description & description);
    private:
        boost::program_options::options_description description;
};

#endif // COMPILER_H
// kate: indent-mode cstyle; space-indent on; indent-width 4;
