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
#include <instruction.h>
#include <programstore.h>

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

    std::string Parser::Code()
    {
        return this->code.ToString();
    }

    void Parser::match(Environment::TOKEN_VALUE value, std::string lexeme)
    {
        bool tmp = this->tokenizer->Peek().GetTokenValue() == value;
        Environment::Token foo("blank");
        if (this->debug) foo = this->tokenizer->Peek();
        if (!(lexeme.length() == 0 && tmp) && !(lexeme.length() > 0 && tmp && this->tokenizer->Peek().GetLexeme() == lexeme))
            ERROR(value);
        this->tokenizer->Ignore();
        if (this->debug) std::cerr << foo << std::endl;
    }

    void Parser::Parse()
    {
        this->program();
    }

    void Parser::program()
    {
        this->match(Environment::KEYWORD, "PROGRAM");
        this->block();
        this->match(Environment::OPERATOR, ".");
        this->match(Environment::EOFL);
    }

    void Parser::block()
    {
        Environment::Token tmp("blank");
        int addr = this->code.TopAddress();
        int level = 0;
        this->code.Push(new Instruction("int", 0, 3));
        while (this->tokenizer->Peek().GetLexeme() == "CONST")
        {
            this->match(Environment::KEYWORD, "CONST");
            do
            {
                if (this->tokenizer->Peek().GetLexeme() == ",")
                    this->tokenizer->Get();
                std::string id = this->tokenizer->Peek().GetLexeme();
                this->identifier(Environment::CONST, DECLARE);
                this->match(Environment::OPERATOR, "=");
                if (this->tokenizer->Peek().GetLexeme() == "+" || this->tokenizer->Peek().GetLexeme() == "-")
                    tmp = this->tokenizer->Get();
                this->code.Push(new Instruction("lit", 0, (tmp.GetLexeme()== "-") ? -1*boost::lexical_cast<int, std::string>(this->tokenizer->Peek().GetLexeme()) : boost::lexical_cast<int, std::string>(this->tokenizer->Peek().GetLexeme())));
                this->code.Push(new Instruction("sto", 0, this->table->Find(id, level)->GetOffSet()));
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
            int addr2 = this->code.TopAddress();
            this->code.Push(new Instruction("jmp", 0, 1));
            std::string id = this->tokenizer->Peek().GetLexeme();
            int level;
            this->identifier(Environment::PROCEDURE, DECLARE);
            Environment::SymbolTableEntry *entry = this->table->Find(id, level);
            entry->SetAddress(this->code.TopAddress());
            this->match(Environment::OPERATOR, ";");
            if (this->debug) std::cerr << *this->table << std::endl;
            this->table->Push();
            this->block();
            if (this->debug) std::cerr << *this->table << std::endl;
            this->table->Pop();
            this->match(Environment::OPERATOR, ";");
            this->code.Push(new Instruction("opr", 0, 0));
            this->code[addr2].SetAddress(this->code.TopAddress());
        }
        this->code[addr].SetAddress(this->table->Count());
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
            std::string id = this->tokenizer->Peek().GetLexeme();
            int level;
            this->identifier(Environment::VAR, USE);
            this->match(Environment::OPERATOR, ":=");
            this->expression();
            Environment::SymbolTableEntry *entry = this->table->Find(id, level);
            this->code.Push(new Instruction("sto", level, entry->GetOffSet()));
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
            int addr = this->code.TopAddress();
            this->code.Push(new Instruction("jmp", 0, 1));
            this->match(Environment::KEYWORD, "THEN");
            this->statement();
            this->code[addr].SetFunction("jpc");
            this->code[addr].SetAddress(this->code.TopAddress());
        }
        else if (this->tokenizer->Peek().GetLexeme() == "WHILE")
        {
            this->match(Environment::KEYWORD, "WHILE");
            int addr = this->code.TopAddress();
            this->condition();
            int addr2 = this->code.TopAddress();
            this->code.Push(new Instruction("jmp", 0, 1));
            this->match(Environment::KEYWORD, "DO");
            this->statement();
            this->code.Push(new Instruction("jmp", 0, addr));
            this->code[addr2].SetFunction("jpc");
            this->code[addr2].SetAddress(this->code.TopAddress());
        }
        else if (this->tokenizer->Peek().GetLexeme() == "READ")
        {
            this->match(Environment::KEYWORD, "READ");
            this->code.Push(new Instruction("opr", 0, 14));
            std::string id = this->tokenizer->Peek().GetLexeme();
            this->identifier(Environment::VAR, USE);
            int level;
            Environment::SymbolTableEntry *entry = this->table->Find(id, level);
            this->code.Push(new Instruction("sto", level, entry->GetOffSet()));
        }
        else if (this->tokenizer->Peek().GetLexeme() == "CALL")
        {
            this->match(Environment::KEYWORD, "CALL");
            std::string id = this->tokenizer->Peek().GetLexeme();
            this->identifier(Environment::PROCEDURE, USE);
            int level;
            Environment::SymbolTableEntry *entry = this->table->Find(id, level);
            this->code.Push(new Instruction("cal", 0, entry->GetAddress()));
        }
        else if (this->tokenizer->Peek().GetLexeme() == "PRINT")
        {
            this->match(Environment::KEYWORD, "PRINT");
            this->expression();
            this->code.Push(new Instruction("opr", 0, 15));
        }
    }

    void Parser::expression()
    {
        Environment::Token tmp("blank");
        this->term();
        while (this->tokenizer->Peek().GetLexeme() == "+" || this->tokenizer->Peek().GetLexeme() == "-")
        {
            std::string op = this->tokenizer->Peek().GetLexeme();
            this->match(Environment::OPERATOR);
            this->term();
            this->code.Push(new Instruction("opr", 0, (op == "+") ? 2 : 3));
        }
    }

    void Parser::condition()
    {
        Environment::Token tmp("blank");
        std::string op = this->tokenizer->Peek().GetLexeme();
        if (this->tokenizer->Peek().GetLexeme() == "ODD")
            this->match(Environment::KEYWORD, "ODD");
        else
        {
            this->expression();
            op = this->tokenizer->Peek().GetLexeme();
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
        this->code.Push(new Instruction("opr", 0,
            (op == "ODD") ? 6 :
            (op == "=") ? 8 :
            (op == "<>") ? 9 :
            (op == "<") ? 10 :
            (op == ">") ? 12 :
            (op == "<=") ? 13 : 11));
    }

    void Parser::term()
    {
        Environment::Token tmp("blank");
        std::string op = this->tokenizer->Peek().GetLexeme();
        if (this->tokenizer->Peek().GetLexeme() == "+" || this->tokenizer->Peek().GetLexeme() == "-")
            this->match(Environment::OPERATOR);
        this->factor();
        if (op == "-") this->code.Push(new Instruction("opr", 0, 1));
        op = this->tokenizer->Peek().GetLexeme();
        while (this->tokenizer->Peek().GetLexeme() == "*"
                || this->tokenizer->Peek().GetLexeme() == "/"
                || this->tokenizer->Peek().GetLexeme() == "DIV"
                || this->tokenizer->Peek().GetLexeme() == "MOD"
              )
        {
            tmp = this->tokenizer->Get();
            if (this->debug) std::cerr << tmp << std::endl;
            if (op == "MOD") this->code.Push(new Instruction("opr", 0, 7));
            this->factor();
            if (op == "MOD")
            {
                this->code.Push(new Instruction("opr", 0, 5));
                this->code.Push(new Instruction("int", 0, 1));
                this->code.Push(new Instruction("opr", 0, 4));
                this->code.Push(new Instruction("opr", 0, 3));
            }
            else
                this->code.Push(new Instruction("opr", 0, (op == "*") ? 4 :
                    (op == "/") ? 5 : 5));
        }
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
        {
            std::string id = this->tokenizer->Peek().GetLexeme();
            int level;
            this->identifier(Environment::VARCONST, USE);
            Environment::SymbolTableEntry *entry = this->table->Find(id, level);
            this->code.Push(new Instruction("lod", level, entry->GetOffSet()));
        }
        else if (this->tokenizer->Peek().GetTokenValue() == Environment::NUMBER)
        {
            this->code.Push(new Instruction("lit", 0, boost::lexical_cast<int, std::string>(this->tokenizer->Peek().GetLexeme())));
            this->match(Environment::NUMBER);
        }
        else
            throw ParserException(this->tokenizer->Peek(), Environment::OPERATOR, this->tokenizer->GetCurrentLine());
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
