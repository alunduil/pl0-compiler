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

#include "tokenizer.h"
#include <fstream>
#include <istream>
#include <token.h>
#include <sstream>
#include <ctype.h>
#include <symboltable.h>
#include <symboltableentry.h>
#include <iostream>

namespace LexicalAnalyzer
{
    Tokenizer::Tokenizer(const std::string fileName, Environment::SymbolTable *table)
    {
        this->table = table;
        this->in = new std::ifstream(fileName.c_str());
        this->buffer.push_back(this->get());
        this->current = this->buffer.begin();
        this->line = 1;
    }

    Tokenizer::Tokenizer(std::istream &in, Environment::SymbolTable *table)
    {
        this->table = table;
        this->in = &in;
        this->buffer.push_back(this->get());
        this->current = this->buffer.begin();
        this->line = 1;
    }

    Environment::Token Tokenizer::Get()
    {
        Environment::Token ret = this->Peek();
        this->Ignore();
        return ret;
    }

    void Tokenizer::Ignore()
    {
        this->buffer.push_back(this->get());
        ++this->current;
    }

    Environment::Token Tokenizer::Peek() const
    {
        return *this->current;
    }

    void Tokenizer::PutBack(const Environment::Token token)
    {
        if (*(--this->current) != token)
            ++this->current;
    }

    void Tokenizer::UnGet()
    {
        --this->current;
    }

    int Tokenizer::GetCurrentLine() const
    {
        return this->line;
    }

    Environment::Token Tokenizer::get()
    {
        std::stringstream tmp;
        Environment::Token ret("blank");
        ret.SetTokenValue(Environment::GARBAGE);
        while (!(*this->in).eof())
        {
            switch (this->in->peek())
            {
                case ' ':
                case '\t':
                    this->in->get();
                    break;
                case '\n':
                    ++this->line;
                    this->in->get();
                    break;
                case '{':
                    while (this->in->get() != '}');
                    break;
                case '+':
                case '-':
                case '*':
                case '/':
                case '=':
                case '(':
                case ')':
                case ',':
                case ';':
                case '.':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::OPERATOR);
                    return ret;
                case ':':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    if (this->in->peek() == '=')
                    {
                        tmp << static_cast<char>(this->in->get());
                        ret.SetLexeme(tmp.str());
                        ret.SetTokenValue(Environment::OPERATOR);
                    }
                    return ret;
                case '<':
                    tmp << static_cast<char>(this->in->get());
                    if (this->in->peek() == '>' || this->in->peek() == '=')
                        tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::OPERATOR);
                    return ret;
                case '>':
                    tmp << static_cast<char>(this->in->get());
                    if (this->in->peek() == '=')
                        tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::OPERATOR);
                    return ret;
                default:
                    if (std::isdigit(this->in->peek()))
                    {
                        int foo;
                        *this->in >> foo;
                        tmp << foo;
                        ret.SetLexeme(tmp.str());
                        ret.SetTokenValue(Environment::NUMBER);
                    }
                    else
                    {
                        while (std::isalnum(this->in->peek()))
                            tmp << static_cast<char>(this->in->get());
                        std::string foo = tmp.str();
                        for (int i = 0; i < foo.length(); ++i)
                            foo[i] = std::toupper(foo[i]);
                        ret.SetLexeme(foo);
                        if (foo == "BEGIN" ||
                            foo == "CALL" ||
                            foo == "CONST" ||
                            foo == "DIV" ||
                            foo == "DO" ||
                            foo == "END" ||
                            foo == "IF" ||
                            foo == "MOD" ||
                            foo == "ODD" ||
                            foo == "PROCEDURE" ||
                            foo == "PROGRAM" ||
                            foo == "PRINT" ||
                            foo == "READ" ||
                            foo == "THEN" ||
                            foo == "VAR" ||
                            foo == "WHILE"
                            )
                        {
                            ret.SetTokenValue(Environment::KEYWORD);
                        }
                        else
                        {
                            ret.SetTokenValue(Environment::IDENTIFIER);
                            int level;
                            Environment::SymbolTableEntry *entry = this->table->Find(foo, level);
                            if (entry)
                            {
                                ret.SetSymbolTableEntry(entry);
                                ret.SetLevel(level);
                            }
                        }
                    }
                    return ret;
            }
        }
    }
}
