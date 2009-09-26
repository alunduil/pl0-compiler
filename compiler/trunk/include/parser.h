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

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <istream>
#include <token.h>

namespace Environment
{
    class SymbolTable;
}

namespace LexicalAnalyzer
{
    class Tokenizer;

    class Parser
    {
        public:
            /**
            * @brief Constructor.
            */
            Parser(std::string fileName, Environment::SymbolTable *table, bool debug = false);

            /**
            * @brief Constructor.
            */
            Parser(std::istream & in, Environment::SymbolTable *table, bool debug = false);

            /**
            * @brief Parse the input.
            *
            * Parse the input and if the debug flag is set print out the tree as it
            * is generated.
            */
            void Parse();

        private:
            Tokenizer *tokenizer;
            bool debug;
            Environment::SymbolTable *table;

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
             *   block -> ( CONST identifier = { + | - } number (, identifier = { + | - } number )* ; )*
             *     ( VAR identifier (, identifier )* ; )*
             *     ( PROCEDURE identifier ; block ; )*
             *     statement
             */
            void block();

            /**
             * @brief Parse a statement.
             *
             * Read the production:
             *   statement -> identifier := expression
             *     | BEGIN statement ( ; statement )* END
             *     | IF condition THEN statement
             *     | WHILE condition DO statement
             *     | READ identifier
             *     | CALL identifier
             *     | PRINT expression
             *     | \epsilon
             */
            void statement();

            /**
             * @brief Parse an expression.
             *
             * Read the production:
             *   expression -> term ( ( + | - ) term )*
             */
            void expression();

            /**
             * @brief Parse a term.
             *
             * Read the production:
             *   term -> { + | - } factor ( ( * | / | DIV | MOD ) factor )*
             */
            void term();

            /**
             * @brief Parse a factor.
             *
             * Read the production:
             *   factor -> ( expression )
             *     | identifier
             *     | number
             */
            void factor();

            /**
             * @brief Parse a condition.
             *
             * Read the production:
             *   condition -> ODD expression
             *     | expression ( = | <> | < | > | <= | >= ) expression
             */
            void condition();

            /**
             * @brief CONST IDENTIFIER NUMBER.
             */
            void constIdentifierNumber();

            /**
             * @brief IDENTIFIER
             */
            void identifier(bool declaration = false);

            /**
             * @brief Read character
             */
            void specialCharacter(std::string foo);

            /**
             * @brief Read number
             */
            void number();
    };

    class ParserException
    {
        public:
            ParserException(Environment::Token got, Environment::TOKEN_VALUE expected, int line, std::string msg = "");
            friend std::ostream & operator<<(std::ostream & out, const ParserException & error)
            {
                out << "Got: " << error.got << std::endl;
                out << "Expected: " << error.expected << std::endl;
                out << "Line: " << error.line << std::endl;
                return out << error.msg;
            }

        private:
            std::string msg;
            int line;
            Environment::Token got;
            Environment::TOKEN_VALUE expected;
    };
}

#endif // PARSER_H
