/*
    <one line to give the program's name and a brief idea of what it does.>
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

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace Environment
{
    class SymbolTableEntry;

    /**
    * @brief A definition of the different tokens.
    * @todo Define more of these values.
    */
    enum TOKEN_VALUE
    {
        IDENTIFIER = 256,
        IBEGIN,
        CALL,
        COMMENT,
        CONST,
        DIV,
        DO,
        END,
        IF,
        MOD,
        ODD,
        PROCEDURE,
        PROGRAM,
        PRINT,
        READ,
        THEN,
        VAR,
        WHILE,
        GARBAGE,
        ASSIGNMENT,
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        EQUALS,
        LEFTPAREN,
        RIGHTPAREN,
        COMMA,
        SEMICOLON,
        NUMBER,
        DOT,
        LESSTHAN,
        NOTEQUAL,
        LESSTHANEQUAL,
        GREATERTHAN,
        GREATERTHANEQUAL,
        EOFL,
        BINARYCONDITIONALOPERATOR
    };

    class Token
    {
        public:
            /**
             * @brief Constructor.
             * @param lexeme The string representing the token.
             * @param value The value of the token.
             * @param entry Entry to a token in the symbol table.
             * @param level The level in the lexical scoping.
             */
            Token(std::string lexeme = "", TOKEN_VALUE value = IDENTIFIER, SymbolTableEntry *entry = NULL, int level = 0);

            /**
             * @brief Get the lexeme for the token.
             * @return The string lexeme.
             */
            std::string GetLexeme() const;

            /**
             * @brief Set the lexeme for the token.
             * @param lexeme The new value of the lexeme for this token.
             */
            void SetLexeme(const std::string &lexeme);

            /**
             * @brief Get the token value.
             * @return TOKEN_VALUE corresponding to the token.
             */
            TOKEN_VALUE GetTokenValue() const;

            /**
             * @brief Set the token value.
             * @param value TOKEN_VALUE the token should take.
             */
            void SetTokenValue(const TOKEN_VALUE &value);

            /**
             * @brief Get the SymbolTableEntry pointer.
             * @return pointer to the SymbolTableEntry.
             */
            SymbolTableEntry * GetSymbolTableEntry() const;

            /**
             * @brief Set the SymbolTableEntry pointer.
             * @param entry SymbolTableEntry pointer.
             */
            void SetSymbolTableEntry(SymbolTableEntry *entry);

            /**
             * @brief Get the lexical level.
             * @return The lexical level of the token.
             */
            int GetLevel() const;

            /**
             * @brief Set the lexical level.
             * @param level The level to set this token to.
             */
            void SetLevel(const int &level);

            /**
             * @brief Compare two Tokens and see if they're equal.
             * @param other The other token to compare to.
             * @return True if the lexemes match.
             * @note Could extend this to check for TOKEN_VALUE as well.
             */
            bool operator==(const Token &other) const;

            /**
             * @brief Not Equivelent.
             * @param other Other token to compare.
             * @return True if the lexemes match.
             */
            bool operator!=(const Token &other) const;

            /**
             * @brief Stream insertion operator.
             * @param out The output stream.
             * @param other The thing to output.
             * @return The stream reference.
             */
            friend std::ostream & operator<<(std::ostream &out, const Token &token)
            {
                return token.print(out);
            }

        private:
            std::string lexeme;
            TOKEN_VALUE value;
            SymbolTableEntry *entry;
            int level;

            /**
             * @brief Print the token.
             * @param out The output stream.
             * @param other The thing to print.
             * @return The stream reference.
             */
            std::ostream & print(std::ostream & out) const;
    };
}

#endif // TOKEN_H
// kate: indent-mode cstyle; space-indent on; indent-width 4;
