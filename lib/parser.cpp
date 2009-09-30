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

    void Parser::Parse()
    {
        this->program();
    }

    void Parser::program()
    {
        Environment::Token tmp("blank");
        if (this->tokenizer->Peek().GetLexeme() != "PROGRAM")
            throw ParserException(this->tokenizer->Peek(), Environment::KEYWORD, this->tokenizer->GetCurrentLine());
        tmp = this->tokenizer->Get();
        if (this->debug) std::cerr << tmp << std::endl;
        this->block();
        this->specialCharacter(".");
    }

    void Parser::constIdentifierNumber()
    {
        Environment::Token tmp("blank");
        this->identifier(true);
        if (this->tokenizer->Peek().GetLexeme() != "=")
            throw ParserException(this->tokenizer->Peek(), Environment::OPERATOR, this->tokenizer->GetCurrentLine());
        tmp = this->tokenizer->Get();
        if (this->debug) std::cerr << tmp << std::endl;
        if (this->tokenizer->Peek().GetLexeme() == "+" || this->tokenizer->Peek().GetLexeme() == "-")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
        }
        if (this->tokenizer->Peek().GetTokenValue() != Environment::NUMBER)
            throw ParserException(this->tokenizer->Peek(), Environment::NUMBER, this->tokenizer->GetCurrentLine());
        tmp = this->tokenizer->Get();
        if (this->debug) std::cerr << tmp << std::endl;
    }

    void Parser::identifier(bool declaration)
    {
        Environment::Token tmp("blank");
        if (this->tokenizer->Peek().GetTokenValue() != Environment::IDENTIFIER)
            throw ParserException(this->tokenizer->Peek(), Environment::IDENTIFIER, this->tokenizer->GetCurrentLine());
        tmp = this->tokenizer->Get();
        if (this->debug) std::cerr << tmp << std::endl;

        if (declaration)
        {
            if (tmp.GetSymbolTableEntry())
            {
                if (tmp.GetLevel() > 0)
                    this->table->Insert(tmp.GetLexeme(), tmp.GetTokenValue());
                else
                    throw ParserException(tmp, Environment::IDENTIFIER, this->tokenizer->GetCurrentLine(), "Identifier redeclared");
            }
            else
                this->table->Insert(tmp.GetLexeme(), tmp.GetTokenValue());
        }
        else
            if (!tmp.GetSymbolTableEntry())
                throw ParserException(tmp, Environment::IDENTIFIER, this->tokenizer->GetCurrentLine(), "Undeclared Identifier");
    }

    void Parser::specialCharacter(std::string foo)
    {
        Environment::Token tmp("blank");
        if (this->tokenizer->Peek().GetLexeme() != foo)
            throw ParserException(this->tokenizer->Peek(), Environment::OPERATOR, this->tokenizer->GetCurrentLine());
        tmp = this->tokenizer->Get();
        if (this->debug) std::cerr << tmp << std::endl;
    }

    void Parser::block()
    {
        Environment::Token tmp("blank");
        while (this->tokenizer->Peek().GetLexeme() == "CONST")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            this->constIdentifierNumber();
            while (this->tokenizer->Peek().GetLexeme() == ",")
            {
                tmp = this->tokenizer->Get();
                if (this->debug) std::cerr << tmp << std::endl;
                this->constIdentifierNumber();
            }
            this->specialCharacter(";");
        }
        while (this->tokenizer->Peek().GetLexeme() == "VAR")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            this->identifier(true);
            while (this->tokenizer->Peek().GetLexeme() == ",")
            {
                tmp = this->tokenizer->Get();
                if (this->debug) std::cerr << tmp << std::endl;
                this->identifier(true);
            }
            this->specialCharacter(";");
        }
        while (this->tokenizer->Peek().GetLexeme() == "PROCEDURE")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            this->identifier(true);
            this->specialCharacter(";");
            if (this->debug) std::cerr << *this->table << std::endl;
            this->table->Push();
            this->block();
            if (this->debug) std::cerr << *this->table << std::endl;
            this->table->Pop();
            this->specialCharacter(";");
        }
        this->statement();
    }

    void Parser::condition()
    {
        Environment::Token tmp("blank");
        if (this->tokenizer->Peek().GetLexeme() == "ODD")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
        }
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
            {
                throw ParserException(this->tokenizer->Peek(), Environment::OPERATOR, this->tokenizer->GetCurrentLine());
            }
            this->specialCharacter(this->tokenizer->Peek().GetLexeme());
        }
        this->expression();
    }

    void Parser::expression()
    {
        Environment::Token tmp("blank");
        this->term();
        while (this->tokenizer->Peek().GetLexeme() == "+" || this->tokenizer->Peek().GetLexeme() == "-")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            this->term();
        }
    }

    void Parser::number()
    {
        Environment::Token tmp("blank");
        if (this->tokenizer->Peek().GetTokenValue() != Environment::NUMBER)
            throw ParserException(this->tokenizer->Peek(), Environment::NUMBER, this->tokenizer->GetCurrentLine());
        tmp = this->tokenizer->Get();
        if (this->debug) std::cerr << tmp << std::endl;
    }

    void Parser::factor()
    {
        Environment::Token tmp("blank");
        if (this->tokenizer->Peek().GetLexeme() == "(")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std:: endl;
            this->expression();
            if (this->tokenizer->Peek().GetLexeme() != ")")
                throw ParserException(this->tokenizer->Peek(), Environment::OPERATOR, this->tokenizer->GetCurrentLine());
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
        }
        else if (this->tokenizer->Peek().GetTokenValue() == Environment::IDENTIFIER)
            this->identifier();
        else if (this->tokenizer->Peek().GetTokenValue() == Environment::NUMBER)
            this->number();
        else
            throw ParserException(this->tokenizer->Peek(), Environment::OPERATOR, this->tokenizer->GetCurrentLine());
    }

    void Parser::statement()
    {
        Environment::Token tmp("blank");
        if (this->tokenizer->Peek().GetTokenValue() == Environment::IDENTIFIER)
        {
            this->identifier();
            this->specialCharacter(":=");
            this->expression();
        }
        else if (this->tokenizer->Peek().GetLexeme() == "BEGIN")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            this->statement();
            while (this->tokenizer->Peek().GetLexeme() == ";")
            {
                tmp = this->tokenizer->Get();
                if (this->debug) std::cerr << tmp << std::endl;
                this->statement();
            }
            if (this->tokenizer->Peek().GetLexeme() != "END")
                throw ParserException(this->tokenizer->Peek(), Environment::KEYWORD, this->tokenizer->GetCurrentLine());
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
        }
        else if (this->tokenizer->Peek().GetLexeme() == "IF")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            this->condition();
            if (this->tokenizer->Peek().GetLexeme() != "THEN")
                throw ParserException(this->tokenizer->Peek(), Environment::KEYWORD, this->tokenizer->GetCurrentLine());
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            this->statement();
        }
        else if (this->tokenizer->Peek().GetLexeme() == "WHILE")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            this->condition();
            if (this->tokenizer->Peek().GetLexeme() != "DO")
                throw ParserException(this->tokenizer->Peek(), Environment::KEYWORD, this->tokenizer->GetCurrentLine());
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            this->statement();
        }
        else if (this->tokenizer->Peek().GetLexeme() == "READ")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            this->identifier();
        }
        else if (this->tokenizer->Peek().GetLexeme() == "CALL")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            this->identifier();
        }
        else if (this->tokenizer->Peek().GetLexeme() == "PRINT")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            this->expression();
        }
    }

    void Parser::term()
    {
        Environment::Token tmp("blank");
        if (this->tokenizer->Peek().GetLexeme() == "+" || this->tokenizer->Peek().GetLexeme() == "-")
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
        }
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
