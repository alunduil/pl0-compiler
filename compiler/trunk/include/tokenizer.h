/*
    Definition of the tokenizer scanner thingy.
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

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <istream>
#include <string>
#include <list>

namespace Environment
{
    class Token;
    class SymbolTable;
};

namespace LexicalAnalyzer
{
    class Tokenizer
    {
        public:
            /**
             * @brief Constructor.
             * @param in The input stream to take stuff from.
             * @param table The symbol table to look things up in->
             */
            Tokenizer(std::istream &in, Environment::SymbolTable *table);

            /**
             * @brief Constructor.
             * @param fileName The file to open and read from.
             * @param table The symbol to look things up in->
             *
             * Calls the stream initializer constructor if necessary.
             */
            Tokenizer(const std::string fileName, Environment::SymbolTable *table);

            /**
             * @brief Get a Token.
             * @return The current token in the buffer.
             *
             * Returns the current Token and then moves current to the next Token
             * in the buffer.
             */
            Environment::Token Get();

            /**
             * @brief Ignore the current Token.
             *
             * Increment the current Token in the buffer.
             */
            void Ignore();

            /**
             * @brief Peek at the current token.
             * @return The current token in the buffer.
             *
             * Returns the current Token without incrementing the current Token
             * in the buffer.
             */
            Environment::Token Peek() const;

            /**
             * @brief Put the token back on the buffer.
             * @param token The token to put back.
             *
             * Check that token is equal to previous Token in the buffer, and
             * then decrement current if this is the case.
             */
            void PutBack(const Environment::Token token);

            /**
             * @brief Unget the last item.
             *
             * Decrement the current pointer without checking that the correct
             * Token is previous in the buffer.
             */
            void UnGet();

            /**
             * @brief Get the current line number.
             * @return The current line we are parsing.
             */
            int GetCurrentLine() const;

        private:
            std::list<Environment::Token> buffer;
            std::list<Environment::Token>::iterator current;
            int line;
            std::istream *in;
            Environment::SymbolTable *table;

            /**
             * @brief Get a token from the stream.
             * @return Token.
             */
            Environment::Token get();
    };
}

#endif // TOKENIZER_H
// kate: indent-mode cstyle; space-indent on; indent-width 4;
