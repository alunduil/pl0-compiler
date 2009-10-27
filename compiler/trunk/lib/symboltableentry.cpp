/*
    This is an entry for a symbol table for the compiler designed in CSIS 435.
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

#include <string>
#include <symboltableentry.h>
#include <ostream>

namespace Environment
{
    SymbolTableEntry::SymbolTableEntry(const std::string &lexeme, const int &offset, const TOKEN_VALUE &value)
    :lexeme(lexeme), offset(offset), value(value), tokenType(REAL)
    {
    }

    ID_TYPE SymbolTableEntry::GetIdentifierType() const
    {
        return this->idType;
    }

    void SymbolTableEntry::SetIdentifierType(const ID_TYPE &type)
    {
        this->idType = type;
    }

    std::string SymbolTableEntry::GetLexeme() const
    {
        return this->lexeme;
    }

    int SymbolTableEntry::GetOffSet() const
    {
        return this->offset;
    }

    TOKEN_VALUE SymbolTableEntry::GetTokenValue() const
    {
        return this->value;
    }

    TOKEN_TYPE SymbolTableEntry::GetType() const
    {
        return this->tokenType;
    }

    void SymbolTableEntry::SetLexeme(const std::string &lexeme)
    {
        this->lexeme = lexeme;
    }

    void SymbolTableEntry::SetOffSet(const int &offset)
    {
        this->offset = offset;
    }

    void SymbolTableEntry::SetTokenValue(const TOKEN_VALUE &token)
    {
        this->value = token;
    }

    void SymbolTableEntry::SetType(const TOKEN_TYPE &type)
    {
        this->tokenType = type;
    }

    bool SymbolTableEntry::operator==(const SymbolTableEntry &that) const
    {
        return this->lexeme == that.lexeme;
    }

    std::ostream & SymbolTableEntry::print(std::ostream & out) const
    {
        return out << this->lexeme << ", " << this->value << ", " << this->tokenType << ", " << this->offset;
    };

    void SymbolTableEntry::SetAddress(const int &address)
    {
        this->address = address;
    }

    int SymbolTableEntry::GetAddress() const
    {
        return this->address;
    }
};
// kate: indent-mode cstyle; space-indent on; indent-width 4;
