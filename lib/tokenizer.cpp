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

#ifndef NFLEX
#include <FlexLexer.h>
Environment::Token *yytoken = NULL;
Environment::SymbolTable *yysymtab = NULL;
#endif

namespace LexicalAnalyzer
{
    Tokenizer::Tokenizer(const std::string fileName, Environment::SymbolTable *table)
    {
        this->table = table;
#ifdef NFLEX
        this->addKeywords();
#else
        yysymtab = table;
#endif
        this->in = new std::ifstream(fileName.c_str());
        this->buffer.push_back(this->get());
        this->current = this->buffer.begin();
        this->line = 1;
    }

    Tokenizer::Tokenizer(std::istream &in, Environment::SymbolTable *table)
    {
        this->table = table;
#ifdef NFLEX
        this->addKeywords();
#else
        yysymtab = table;
#endif
        this->in = &in;
        this->buffer.push_back(this->get());
        this->current = this->buffer.begin();
        this->line = 1;
    }

#ifdef NFLEX
    void Tokenizer::addKeywords()
    {
        std::vector<boost::tuple<std::string, Environment::TOKEN_VALUE> > keywords;
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("BEGIN", Environment::IBEGIN));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("CALL", Environment::CALL));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("CONST", Environment::CONST));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("DIV", Environment::DIV));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("DO", Environment::DO));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("END", Environment::END));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("IF", Environment::IF));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("MOD", Environment::MOD));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("ODD", Environment::ODD));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("PROCEDURE", Environment::PROCEDURE));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("PROGRAM", Environment::PROGRAM));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("PRINT", Environment::PRINT));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("READ", Environment::READ));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("THEN", Environment::THEN));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("VAR", Environment::VAR));
        keywords.push_back(boost::tuple<std::string, Environment::TOKEN_VALUE>("WHILE", Environment::WHILE));
        for (std::vector<boost::tuple<std::string, Environment::TOKEN_VALUE> >::iterator i = keywords.begin(); i != keywords.end(); ++i)
            this->table->Insert(boost::get<0>(*i), boost::get<1>(*i));
    }
#endif

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

#ifdef NFLEX
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
                        ret.SetTokenValue(Environment::ASSIGNMENT);
                    }
                    return ret;
                case '+':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::PLUS);
                    return ret;
                case '-':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::MINUS);
                    return ret;
                case '*':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::MULTIPLY);
                    return ret;
                case '/':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::DIVIDE);
                    return ret;
                case '=':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::EQUALS);
                    return ret;
                case '(':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::LEFTPAREN);
                    return ret;
                case ')':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::RIGHTPAREN);
                    return ret;
                case ',':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::COMMA);
                    return ret;
                case ';':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::SEMICOLON);
                    return ret;
                case '.':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetLexeme(tmp.str());
                    ret.SetTokenValue(Environment::DOT);
                    return ret;
                case '<':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetTokenValue(Environment::LESSTHAN);
                    if (this->in->peek() == '>')
                    {
                        tmp << static_cast<char>(this->in->get());
                        ret.SetTokenValue(Environment::NOTEQUAL);
                    }
                    else if (this->in->peek() == '=')
                    {
                        tmp << static_cast<char>(this->in->get());
                        ret.SetTokenValue(Environment::LESSTHANEQUAL);
                    }
                    ret.SetLexeme(tmp.str());
                    return ret;
                case '>':
                    tmp << static_cast<char>(this->in->get());
                    ret.SetTokenValue(Environment::GREATERTHAN);
                    if (this->in->peek() == '=')
                    {
                        tmp << static_cast<char>(this->in->get());
                        ret.SetTokenValue(Environment::GREATERTHANEQUAL);
                    }
                    ret.SetLexeme(tmp.str());
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
#else
    Environment::Token Tokenizer::get()
    {
        static std::stringstream bit_bucket;
        if (yytoken) delete yytoken;
        static yyFlexLexer *scanner = new yyFlexLexer(this->in, &bit_bucket);
        while (scanner->yylex() == Environment::COMMENT) delete yytoken;
        return *yytoken;
    }
#endif
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
