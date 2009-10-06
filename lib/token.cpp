/*
    Token abstraction for a compiler's communication between tokenizer and parser.
    Copyright (C) 2009 Alex Brandt <alunduil@alunduil.com>

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

#include <token.h>
#include <symboltableentry.h>
#include <string>
#include <iostream>

namespace Environment
{
    Token::Token(std::string lexeme, TOKEN_VALUE value, SymbolTableEntry *entry, int level)
    {
        this->lexeme = lexeme;
        this->value = value;
        this->entry = entry;
        this->level = level;
    }

    Token::Token()
    {
    }

    int Token::GetLevel() const
    {
        return this->level;
    }

    std::string Token::GetLexeme() const
    {
        return this->lexeme;
    }

    SymbolTableEntry * Token::GetSymbolTableEntry() const
    {
        return this->entry;
    }

    TOKEN_VALUE Token::GetTokenValue() const
    {
        return this->value;
    }

    void Token::SetLevel(const int level)
    {
        this->level = level;
    }

    void Token::SetLexeme(const std::string lexeme)
    {
        this->lexeme = lexeme;
    }

    void Token::SetSymbolTableEntry(SymbolTableEntry * entry)
    {
        this->entry = entry;
    }

    void Token::SetTokenValue(const TOKEN_VALUE value)
    {
        this->value = value;
    }

    bool Token::operator==(const Token & other) const
    {
        return this->lexeme == other.lexeme;
    }

    bool Token::operator!=(const Token &other) const
    {
        return !(*this == other);
    }

    std::ostream & Token::print(std::ostream & out) const
    {
        return out << "< " << this->lexeme << ", " << this->value << ", " << this->entry << ", " << this->level << " >";
    }
}
// kate: indent-mode cstyle; space-indent on; indent-width 4; 
