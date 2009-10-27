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
#include <boost/tuple/tuple.hpp>
#include <FlexLexer.h>

Environment::Token *yytoken = NULL;
Environment::SymbolTable *yysymtab = NULL;

namespace LexicalAnalyzer
{
    Tokenizer::Tokenizer(const std::string fileName, Environment::SymbolTable *table)
    :table(table), in(new std::ifstream(fileName.c_str())), line(1)
    {
        yysymtab = table;
        this->buffer.push_back(this->get());
        this->current = this->buffer.begin();
    }

    Tokenizer::Tokenizer(std::istream &in, Environment::SymbolTable *table)
    :table(table), in(&in), line(1)
    {
        yysymtab = table;
        this->buffer.push_back(this->get());
        this->current = this->buffer.begin();
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
        static std::stringstream bit_bucket;
        if (yytoken) delete yytoken;
        static yyFlexLexer *scanner = new yyFlexLexer(this->in, &bit_bucket);
        while (scanner->yylex() == Environment::COMMENT) delete yytoken;
        this->line = scanner->lineno();
        return *yytoken;
    }
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
