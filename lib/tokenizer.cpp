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

#include <tokenizer.h>
#include <fstream>
#include <istream>
#include <token.h>
#include <sstream>
#include <ctype.h>
#include <symboltable.h>
#include <symboltableentry.h>
#include <iostream>
#include <vector>

namespace LexicalAnalyzer
{
    Tokenizer::Tokenizer(const std::string fileName, Environment::SymbolTable *table)
    {
        this->table = table;
        this->addKeywords();
        this->in = new std::ifstream(fileName.c_str());
        this->buffer.push_back(this->get());
        this->current = this->buffer.begin();
        this->line = 1;
    }

    Tokenizer::Tokenizer(std::istream &in, Environment::SymbolTable *table)
    {
        this->table = table;
        this->addKeywords();
        this->in = &in;
        this->buffer.push_back(this->get());
        this->current = this->buffer.begin();
        this->line = 1;
    }

    void Tokenizer::addKeywords()
    {
        Environment::SymbolTableEntry entry("blank");
        std::vector<std::string> keywords;
        keywords.push_back("BEGIN");
        keywords.push_back("CALL");
        keywords.push_back("CONST");
        keywords.push_back("DIV");
        keywords.push_back("DO");
        keywords.push_back("END");
        keywords.push_back("IF");
        keywords.push_back("MOD");
        keywords.push_back("ODD");
        keywords.push_back("PROCEDURE");
        keywords.push_back("PROGRAM");
        keywords.push_back("PRINT");
        keywords.push_back("READ");
        keywords.push_back("THEN");
        keywords.push_back("VAR");
        keywords.push_back("WHILE");
        for (std::vector<std::string>::iterator i = keywords.begin(); i != keywords.end(); ++i)
        {
            entry.SetLexeme(*i);
            entry.SetTokenValue(Environment::KEYWORD);
            this->table->Insert(entry);
        }
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
        ret.SetTokenValue(Environment::EOFL);
        while (!(*this->in).eof())
        {
            switch (this->in->peek())
            {
                case '\n':
                    ++this->line;
                case ' ':
                case '\r': /// @note Thanks Steve! </sarcasm>
                case '\t':
                    this->in->get();
                    break;
                case '{':
                    while (this->in->peek() != '}')
                        if (this->in->get() == '\n') ++this->line;
                    this->in->get();
                    break;
                case ':':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::GARBAGE);
                    if (this->in->peek() == '=')
                    {
                        tmp << static_cast<char>(this->in->get());
                        ret.SetLexeme(tmp.str());
                        ret.SetTokenValue(Environment::OPERATOR);
                    }
                    return ret;
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
                        return ret;
                    }
                    else if (std::isalnum(this->in->peek()))
                    {
                        while (std::isalnum(this->in->peek()))
                            tmp << static_cast<char>(this->in->get());
                        std::string foo = tmp.str();
                        for (int i = 0; i < foo.length(); ++i)
                            foo[i] = std::toupper(foo[i]);
                        int level;
                        ret.SetLexeme(foo);
                        ret.SetTokenValue(Environment::IDENTIFIER);
                        Environment::SymbolTableEntry *entry = this->table->Find(foo, level);
                        if (entry)
                        {
                            ret.SetLexeme(entry->GetLexeme());
                            ret.SetTokenValue(entry->GetTokenValue());
                            ret.SetSymbolTableEntry(entry);
                            ret.SetLevel(level);
                        }
                        return ret;
                    }
                    break; /// @todo Character not in alphabet . . .
            }
        }
        return ret;
    }
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
