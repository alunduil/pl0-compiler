/*
    This is a symbol table for the compiler designed in CSIS 435.
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

#include <iostream>
#include <boost/tuple/tuple_io.hpp>

#include "../include/symboltable.h"
#include "../include/symboltableentry.h"
#include "../include/output.h"

namespace Environment
{
    SymbolTable::SymbolTable()
    {
        this->tables.push_back(new InternalSymbolTable());
    }

    SymbolTable::~SymbolTable()
    {
        for (std::list<InternalSymbolTable *>::reverse_iterator i = this->tables.rbegin(); i != this->tables.rend(); ++i)
            delete *i;
        while (this->tables.size() != 0)
            this->tables.pop_back();
    }

    int SymbolTable::Count() const
    {
        return this->tables.back()->Count();
    }

    SymbolTableEntry * SymbolTable::Find(const SymbolTableEntry & entry, int & level)
    {
        level = 0;
        for (std::list<InternalSymbolTable *>::reverse_iterator i = this->tables.rbegin(); i != this->tables.rend(); ++i, ++level)
            if ((*i)->Find(entry)) return (*i)->Find(entry);
        return NULL;
    }

    SymbolTableEntry * SymbolTable::Find(const std::string & lexeme, int & level)
    {
        SymbolTableEntry *tmp = new SymbolTableEntry(lexeme, 0);
        SymbolTableEntry *ret = this->Find(*tmp, level);
        delete tmp;
        return ret;
    };

    SymbolTableEntry * SymbolTable::Insert(const SymbolTableEntry & entry)
    {
        SymbolTableEntry *tmp = new SymbolTableEntry(entry);
        this->tables.back()->Insert(*tmp);
        return tmp;
    }

    SymbolTableEntry * SymbolTable::Insert(const std::string & lexeme, const yytokentype & value)
    {
        SymbolTableEntry * tmp = new SymbolTableEntry(lexeme, 0, value);
        SymbolTableEntry * ret = this->Insert(*tmp);
        delete tmp;
        return ret;
    }

    void SymbolTable::Push()
    {
        this->tables.push_back(new InternalSymbolTable());
    }

    void SymbolTable::Pop()
    {
        delete this->tables.back();
        this->tables.pop_back();
    }

    std::ostream & SymbolTable::print(std::ostream & out)
    {
        int level = 0;
        for (std::list<InternalSymbolTable *>::reverse_iterator i = this->tables.rbegin(); i != this->tables.rend(); ++i, ++level)
        {
            out << "Level: " << level << std::endl;
            out << *(*i);
        }
        return out;
    };

    SymbolTableEntry * SymbolTable::InternalSymbolTable::Find(const std::string & lexeme)
    {
        SymbolTableEntry *tmp = new SymbolTableEntry(lexeme, 0);
        SymbolTableEntry *ret = this->Find(*tmp);
        delete tmp;
        return ret;
    };

    SymbolTableEntry * SymbolTable::InternalSymbolTable::Find(const SymbolTableEntry & entry)
    {
        if (this->table.find(entry.GetLexeme()) != this->table.end()) return this->table[entry.GetLexeme()];
        return NULL;
    }

    SymbolTableEntry * SymbolTable::InternalSymbolTable::Insert(SymbolTableEntry & entry)
    {
        entry.SetOffSet(this->count++);
        this->table[entry.GetLexeme()] = &entry;
        return &entry;
    };

    int SymbolTable::InternalSymbolTable::Count()
    {
        this->count = 3;
        for (std::map<std::string, SymbolTableEntry *>::iterator i = this->table.begin(); i != this->table.end(); ++i)
        {
            i->second->SetOffSet(this->count);
            this->count += ((i->second->GetIdentifierType() & ARRAY_ID) == ARRAY_ID) ? i->second->GetSize() : 1;
        }
        return this->count;
    }

    SymbolTableEntry * SymbolTable::InternalSymbolTable::Insert(const std::string & lexeme, const yytokentype & value)
    {
        SymbolTableEntry *tmp = new SymbolTableEntry(lexeme, 0, value);
        return this->Insert(*tmp);
    }

    std::ostream & SymbolTable::InternalSymbolTable::print(std::ostream & out)
    {
        for (std::map<std::string, SymbolTableEntry *>::iterator i = this->table.begin(); i != this->table.end(); ++i)
            out << (*(i->second)) << std::endl;
        return out;
    }

    SymbolTable::InternalSymbolTable::~InternalSymbolTable()
    {
        for (std::map<std::string, SymbolTableEntry *>::iterator i = this->table.begin(); i != this->table.end(); ++i)
            delete i->second;
    }

    SymbolTable::InternalSymbolTable::InternalSymbolTable()
    {
        this->count = 3;
    }
};

// kate: indent-mode cstyle; space-indent on; indent-width 4;
