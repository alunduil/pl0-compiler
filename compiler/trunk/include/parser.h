/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2009  Alex Brandt

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

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <istream>

#include "../include/token.h"
#include "../include/tokenizer.h"
#include "../include/symboltable.h"
#include "../include/programstore.h"
#include "../include/errorqueue.h"

namespace Analyzer
{
    class Parser
    {
        public:
            /**
             * @brief Constructor.
             * @param fileName The name of the file to open.
             * @param maxErrors The maximum number of errors to report before dying.
             * @param verbose Whether or not to print verbose output.
             * @param debug Whether or not to print debugging output.
             */
            Parser(const std::string & fileName, const int & maxErrors = 25, const bool & verbose = false, const bool & debug = false, const bool & warnings = false);

            /**
             * @brief Constructor.
             * @param in The input stream to read from.
             * @param maxErrors The maximum number of errors to report before dying.
             * @param verbose Whether or not to print verbose output.
             * @param debug Whether or not to print debugging output.
             */
            Parser(std::istream & in, const int & maxErrors = 25, const bool & verbose = false, const bool & debug = false, const bool & warnings = false);

            /**
             * @brief Return the string of code generated.
             */
            std::string Code();

            /**
             * @brief Parse the input.
             *
             * Parse the input and if the debug flag is set print out the tree as it
             * is generated.
             */
            void Parse();

            /**
             * @brief Tell the caller whether errors were detected during compilation.
             */
            bool HaveErrors() const;

        private:
            bool debug;
            bool verbose;
            bool warn;
            Tokenizer tokenizer;
            Environment::SymbolTable table;
            Generator::ProgramStore code;
            Environment::ErrorQueue errors;
            Environment::ErrorQueue warnings;

            /**
             * @brief Parse a program.
             *
             * Read the production:
             *   program -> PROGRAM block .
             */
            void program();

            /**
             * @brief Parse a block.
             *
             * Read the production:
             *   block -> constant_declarations variable_declarations procedure_declarations statement
             */
            void block();

            /**
             * @brief Parse a constant_declarations.
             *
             * Read the production:
             *   constant_declarations -> CONST identifier = sign number more_constants ; constant_declarations | epsilon
             */
            void constant_declarations();

            /**
             * @brief Parse a variable_declarations.
             *
             * Read the production:
             *   variable_declarations -> VAR identifier more_variables ; variable_declarations | epsilon
             */
            void variable_declarations();

            /**
             * @brief Parse a procedure_declarations.
             *
             * Read the production:
             *   procedure_declarations -> PROCEDURE identifier ; block ; procedure_declarations | epsilon
             */
            void procedure_declarations();

            /**
             * @brief Parse a more_constants.
             *
             * Read the production:
             *   more_constants -> , identifier = sign number more_constants | epsilon
             */
            void more_constants();

            /**
             * @brief Parse a more_variables.
             *
             * Read the production:
             *   more_variables -> , identifier more_variables | epsilon
             */
            void more_variables();

            /**
             * @brief Parse a statement.
             *
             * Read the production:
             *   statement -> identifier := expression
             *     | BEGIN statement more_statements END
             *     | IF condition THEN statement
             *     | WHILE condition DO statement
             *     | READ identifier
             *     | CALL identifier
             *     | PRINT expression
             *     | epsilon
             */
            void statement();

            /**
             * @brief Parse a more_statements.
             *
             * Read the production:
             *   more_statements -> ; statement more_statements | epsilon
             */
            void more_statements();

            /**
             * @brief Parse an expression.
             *
             * Read the production:
             *   expression -> term expression_rhs
             */
            void expression();

            /**
             * @brief Parse an expression_rhs.
             *
             * Read the production:
             *   expression_rhs -> + term expression_rhs | - term epxression_rhs | epsilon
             */
            void expression_rhs();

            /**
             * @brief Parse a term.
             *
             * Read the production:
             *   term -> sign factor term_rhs
             */
            void term();

            /**
             * @brief Parse a term_rhs.
             *
             * Read the production:
             *   term_rhs -> * factor term_rhs | / factor term_rhs | DIV factor term_rhs | MOD factor term_rhs | epsilon
             */
            void term_rhs();

            /**
             * @brief Parse a factor.
             *
             * Read the production:
             *   factor -> ( expression ) | identifier | number
             */
            void factor();

            /**
             * @brief Parse a condition.
             *
             * Read the production:
             *   condition -> ODD expression | expression binary_condition
             */
            void condition();

            /**
             * @brief Parse a binary condition.
             *
             * Read the production:
             *   binary_condition -> ( = | <> | < | > | <= | >= ) expression
             */
            void binary_condition();

            /**
             * @brief Parse an identifier.
             */
            void identifier(Environment::Token & token, const Environment::ID_TYPE & type, const Environment::ID_PURPOSE & declaration);

            /**
             * @brief Match a token.
             * @param got The token we matched.
             * @param value The value we are looking for.
             */
            bool match(Environment::Token & got, const Environment::TOKEN_VALUE & value, const Environment::ERROR_TYPE & level = Environment::ERROR_T);

            /**
             * @brief Peek at the next token.
             * @return The TOKEN_VALUE of the next token.
             */
            Environment::TOKEN_VALUE peek() const;

            /**
             * @brief Parse a sign.
             * @return True if negation needs to be performed.
             *
             * Read the production:
             *   sign -> + | - | epsilon
             */
            bool sign();
    };
}

#endif // PARSER_H
// kate: indent-mode cstyle; space-indent on; indent-width 4;
