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
#include <symboltableentry.h>
#include <programstore.h>
#include <errorqueue.h>

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
            Parser(std::string fileName, Environment::SymbolTable *table, bool debug = false, int maxErrors = 10);

            /**
            * @brief Constructor.
            */
            Parser(std::istream & in, Environment::SymbolTable *table, bool debug = false, int maxErrors = 10);

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

        private:
            Tokenizer *tokenizer;
            bool debug;
            Environment::SymbolTable *table;
            ProgramStore code;
            ErrorQueue errors;

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
            void identifier(Environment::ID_TYPE tipe, ID_PURPOSE declaration = USE);

            /**
             * @brief Read number
             */
            void number();

            /**
             * @brief Match a token.
             * @param token The token we matched.
             * @param value The value we are looking for.
             * @param lexeme Optional lexeme to match on.
             */
            bool match(Environment::Token &token, Environment::TOKEN_VALUE value, std::string lexem = "");
    };
}

#endif // PARSER_H
// kate: indent-mode cstyle; space-indent on; indent-width 4; 
