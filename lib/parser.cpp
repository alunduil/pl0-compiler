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

#include <parser.h>
#include <token.h>
#include <iostream>
#include <symboltable.h>
#include <tokenizer.h>
#include <symboltableentry.h>
#include <boost/lexical_cast.hpp>

#define ERROR(E) throw ParserException(this->tokenizer->Peek(), (E), this->tokenizer->GetCurrentLine())

namespace LexicalAnalyzer
{
    Parser::Parser(std::istream &in, Environment::SymbolTable *table, bool debug)
    {
        this->debug = debug;
        this->table = table;
        this->tokenizer = new Tokenizer(in, table);
    }

    Parser::Parser(std::string fileName, Environment::SymbolTable *table, bool debug)
    {
        this->debug = debug;
        this->table = table;
        this->tokenizer = new Tokenizer(fileName, table);
    }

    void Parser::match(Environment::TOKEN_VALUE value, std::string lexeme)
    {
        bool tmp = this->tokenizer->Peek().GetTokenValue() == value;
#ifndef NDEBUG
        Environment::Token foo("blank");
        foo = this->tokenizer->Peek();
#endif
        if (!(lexeme.length() == 0 && tmp) && !(lexeme.length() > 0 && tmp && this->tokenizer->Peek().GetLexeme() == lexeme))
            ERROR(value);
        this->tokenizer->Ignore();
#ifndef NDEBUG
        std::cerr << foo << std::endl;
#endif
    }

    void Parser::Parse()
    {
        this->program();
    }

    void Parser::program()
    {
        Environment::Token tmp("blank");
        this->match(Environment::KEYWORD, "PROGRAM");
        this->block();
        this->match(Environment::OPERATOR, ".");
        this->match(Environment::EOFL);
    }

    void Parser::block()
    {
        Environment::Token tmp("blank");
        while (this->tokenizer->Peek().GetLexeme() == "CONST")
        {
            this->match(Environment::KEYWORD, "CONST");
            do
            {
                if (this->tokenizer->Peek().GetLexeme() == ",")
                    this->tokenizer->Get();
                this->identifier(Environment::CONST, DECLARE);
                this->match(Environment::OPERATOR, "=");
                if (this->tokenizer->Peek().GetLexeme() == "+" || this->tokenizer->Peek().GetLexeme() == "-")
                {
                    tmp = this->tokenizer->Get();
                    if (this->debug) std::cerr << tmp << std::endl;
                }
                this->match(Environment::NUMBER);
            }
            while (this->tokenizer->Peek().GetLexeme() == ",");
            this->match(Environment::OPERATOR, ";");
        }
        while (this->tokenizer->Peek().GetLexeme() == "VAR")
        {
            this->match(Environment::KEYWORD, "VAR");
            do
            {
                if (this->tokenizer->Peek().GetLexeme() == ",")
                    this->tokenizer->Get();
                this->identifier(Environment::VAR, DECLARE);
            }
            while (this->tokenizer->Peek().GetLexeme() == ",");
            this->match(Environment::OPERATOR, ";");
        }
        while (this->tokenizer->Peek().GetLexeme() == "PROCEDURE")
        {
            this->match(Environment::KEYWORD, "PROCEDURE");
            this->identifier(Environment::PROCEDURE, DECLARE);
            this->match(Environment::OPERATOR, ";");
            if (this->debug) std::cerr << *this->table << std::endl;
            this->table->Push();
            this->block();
            if (this->debug) std::cerr << *this->table << std::endl;
            this->table->Pop();
            this->match(Environment::OPERATOR, ";");
        }
        this->statement();
    }

    void Parser::identifier(Environment::ID_TYPE tipe, ID_PURPOSE declaration)
    {
        Environment::Token tmp = this->tokenizer->Peek();
        Environment::SymbolTableEntry entry(tmp.GetLexeme());
        this->match(Environment::IDENTIFIER);
        Environment::ID_TYPE got;

        if (declaration == DECLARE)
        {
            if (tmp.GetSymbolTableEntry() && tmp.GetLevel() < 1)
                throw ParserException(tmp, Environment::IDENTIFIER, this->tokenizer->GetCurrentLine(), "Identifier Redeclared");
            entry.SetIdentifierType(tipe);
            entry.SetLexeme(tmp.GetLexeme());
            entry.SetTokenValue(tmp.GetTokenValue());
            this->table->Insert(entry);
        }
        else
            if (!tmp.GetSymbolTableEntry())
                throw ParserException(tmp, Environment::IDENTIFIER, this->tokenizer->GetCurrentLine(), "Undeclared Identifier");
            else
            {
                got = tmp.GetSymbolTableEntry()->GetIdentifierType();
                if ((tipe == Environment::VARCONST && got != Environment::VAR && got != Environment::CONST) && got != tipe)
                {
                    throw ParserException(tmp, static_cast<Environment::TOKEN_VALUE>(tipe), this->tokenizer->GetCurrentLine(), boost::lexical_cast<std::string, Environment::ID_TYPE>(got));
                }
            }
    }

    void Parser::statement()
    {
        Environment::Token tmp("blank");
        if (this->tokenizer->Peek().GetTokenValue() == Environment::IDENTIFIER)
        {
            this->identifier(Environment::VAR, USE);
            this->match(Environment::OPERATOR, ":=");
            this->expression();
        }
        else if (this->tokenizer->Peek().GetLexeme() == "BEGIN")
        {
            this->match(Environment::KEYWORD, "BEGIN");
            this->statement();
            while (this->tokenizer->Peek().GetLexeme() == ";")
            {
                this->match(Environment::OPERATOR, ";");
                this->statement();
            }
            this->match(Environment::KEYWORD, "END");
        }
        else if (this->tokenizer->Peek().GetLexeme() == "IF")
        {
            this->match(Environment::KEYWORD, "IF");
            this->condition();
            this->match(Environment::KEYWORD, "THEN");
            this->statement();
        }
        else if (this->tokenizer->Peek().GetLexeme() == "WHILE")
        {
            this->match(Environment::KEYWORD, "WHILE");
            this->condition();
            this->match(Environment::KEYWORD, "DO");
            this->statement();
        }
        else if (this->tokenizer->Peek().GetLexeme() == "READ")
        {
            this->match(Environment::KEYWORD, "READ");
            this->identifier(Environment::VAR, USE);
        }
        else if (this->tokenizer->Peek().GetLexeme() == "CALL")
        {
            this->match(Environment::KEYWORD, "CALL");
            this->identifier(Environment::PROCEDURE, USE);
        }
        else if (this->tokenizer->Peek().GetLexeme() == "PRINT")
        {
            this->match(Environment::KEYWORD, "PRINT");
            this->expression();
        }
    }

    void Parser::expression()
    {
        Environment::Token tmp("blank");
        this->term();
        while (this->tokenizer->Peek().GetLexeme() == "+" || this->tokenizer->Peek().GetLexeme() == "-")
        {
            this->match(Environment::OPERATOR);
            this->term();
        }
    }

    void Parser::condition()
    {
        Environment::Token tmp("blank");
        if (this->tokenizer->Peek().GetLexeme() == "ODD")
            this->match(Environment::KEYWORD, "ODD");
        else
        {
            this->expression();
            if (this->tokenizer->Peek().GetLexeme() != "="
                && this->tokenizer->Peek().GetLexeme() != "<>"
                && this->tokenizer->Peek().GetLexeme() != "<"
                && this->tokenizer->Peek().GetLexeme() != ">"
                && this->tokenizer->Peek().GetLexeme() != "<="
                && this->tokenizer->Peek().GetLexeme() != ">="
                )
                throw ParserException(this->tokenizer->Peek(), Environment::OPERATOR, this->tokenizer->GetCurrentLine());
            this->match(Environment::OPERATOR);
        }
        this->expression();
    }

    void Parser::factor()
    {
        Environment::Token tmp("blank");
        if (this->tokenizer->Peek().GetLexeme() == "(")
        {
            this->match(Environment::OPERATOR, "(");
            this->expression();
            this->match(Environment::OPERATOR, ")");
        }
        else if (this->tokenizer->Peek().GetTokenValue() == Environment::IDENTIFIER)
            this->identifier(Environment::VARCONST, USE);
        else if (this->tokenizer->Peek().GetTokenValue() == Environment::NUMBER)
            this->match(Environment::NUMBER);
        else
            throw ParserException(this->tokenizer->Peek(), Environment::OPERATOR, this->tokenizer->GetCurrentLine());
    }

    void Parser::term()
    {
        Environment::Token tmp("blank");
        if (this->tokenizer->Peek().GetLexeme() == "+" || this->tokenizer->Peek().GetLexeme() == "-")
            this->match(Environment::OPERATOR);
        this->factor();
        while (this->tokenizer->Peek().GetLexeme() == "*"
            || this->tokenizer->Peek().GetLexeme() == "/"
            || this->tokenizer->Peek().GetLexeme() == "DIV"
            || this->tokenizer->Peek().GetLexeme() == "MOD"
            )
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            this->factor();
        }
    }

    ParserException::ParserException(Environment::Token got, Environment::TOKEN_VALUE expected, int line, std::string msg)
    {
        this->got = got;
        this->expected = expected;
        this->line = line;
        this->msg = msg;
    }
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
