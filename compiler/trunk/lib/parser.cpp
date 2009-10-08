/*
    <one line to give the program's name and a brief idea of what it does.>
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

#include <parser.h>
#include <token.h>
#include <iostream>
#include <symboltable.h>
#include <tokenizer.h>
#include <symboltableentry.h>
#include <boost/lexical_cast.hpp>
#include <instruction.h>
#include <programstore.h>

namespace LexicalAnalyzer
{
    Parser::Parser(std::istream &in, Environment::SymbolTable *table, bool debug, int maxErrors)
    :errors(maxErrors)
    {
        this->debug = debug;
        this->table = table;
        this->tokenizer = new Tokenizer(in, table);
    }

    Parser::Parser(std::string fileName, Environment::SymbolTable *table, bool debug, int maxErrors)
    :errors(maxErrors)
    {
        this->debug = debug;
        this->table = table;
        this->tokenizer = new Tokenizer(fileName, table);
    }

    std::string Parser::Code()
    {
        return this->code.ToString();
    }

    bool Parser::match(Environment::Token &token, Environment::TOKEN_VALUE value, std::string lexeme)
    {
        /**
         * @note This method performs phrase level error recovery on all tokens attempted to be
         * matched by it.  This allows us to list errors even if an operator is missing or
         * another error occurs.
         */
        bool tmp = this->tokenizer->Peek().GetTokenValue() == value;
        token = this->tokenizer->Peek();
        if (!(lexeme.length() == 0 && tmp) && !(lexeme.length() > 0 && tmp && token.GetLexeme() == lexeme))
        {
            this->errors.Push(token, value, this->tokenizer->GetCurrentLine(), lexeme);
            return false;
        }
        this->tokenizer->Ignore();
        if (this->debug) std::cerr << token << std::endl;
        return true;
    }

    void Parser::Parse()
    {
        this->program();
    }

    void Parser::program()
    {
        Environment::Token tmp("blank");
        this->match(tmp, Environment::KEYWORD, "PROGRAM");
        this->block();
        this->match(tmp, Environment::OPERATOR, ".");
        this->match(tmp, Environment::EOFL);
    }

    void Parser::block()
    {
        Environment::Token tmp("blank");
        int addr = this->code.TopAddress();
        int level = 0;
        this->code.Push(new Instruction("int", 0, 3));
        while (this->tokenizer->Peek().GetLexeme() == "CONST")
        {
            this->match(tmp, Environment::KEYWORD, "CONST");
            do
            {
                if (this->tokenizer->Peek().GetLexeme() == ",")
                    this->match(tmp, Environment::OPERATOR, ",");
                std::string id = this->tokenizer->Peek().GetLexeme();
                this->identifier(Environment::CONST, DECLARE);
                this->match(tmp, Environment::OPERATOR, "=");
                std::string op;
                if (this->tokenizer->Peek().GetLexeme() == "+" || this->tokenizer->Peek().GetLexeme() == "-")
                {
                    this->match(tmp, Environment::OPERATOR);
                    op = tmp.GetLexeme();
                }
                this->match(tmp, Environment::NUMBER);
                this->code.Push(new Instruction("lit", 0, boost::lexical_cast<int, std::string>(tmp.GetLexeme())));
                if (op == "-") this->code.Push(new Instruction("opr", 0, 1));
                Environment::SymbolTableEntry *entry = this->table->Find(id, level);
                this->code.Push(new Instruction("sto", level, entry->GetOffSet()));
            }
            while (this->tokenizer->Peek().GetLexeme() == ",");
            /**
             * @note Panic mode error recovery.
             */
            while (!this->match(tmp, Environment::OPERATOR, ";"))
                this->tokenizer->Ignore();
        }
        while (this->tokenizer->Peek().GetLexeme() == "VAR")
        {
            this->match(tmp, Environment::KEYWORD, "VAR");
            do
            {
                if (this->tokenizer->Peek().GetLexeme() == ",")
                    this->match(tmp, Environment::OPERATOR, ",");
                this->identifier(Environment::VAR, DECLARE);
            }
            while (this->tokenizer->Peek().GetLexeme() == ",");
            /**
             * @note Panic mode error recovery.
             */
            while (!this->match(tmp, Environment::OPERATOR, ";"))
                this->tokenizer->Ignore();
        }
        while (this->tokenizer->Peek().GetLexeme() == "PROCEDURE")
        {
            this->match(tmp, Environment::KEYWORD, "PROCEDURE");
            int addr2 = this->code.TopAddress();
            this->code.Push(new Instruction("jmp", 0, this->code.TopAddress() + 1));
            std::string id = this->tokenizer->Peek().GetLexeme();
            this->identifier(Environment::PROCEDURE, DECLARE);
            int level;
            Environment::SymbolTableEntry *entry = this->table->Find(id, level);
            entry->SetAddress(this->code.TopAddress());
            this->match(tmp, Environment::OPERATOR, ";");
            if (this->debug) std::cerr << *this->table << std::endl;
            this->table->Push();
            this->block();
            if (this->debug) std::cerr << *this->table << std::endl;
            this->table->Pop();
            this->match(tmp, Environment::OPERATOR, ";");
            this->code.Push(new Instruction("opr", 0, 0));
            this->code[addr2].SetAddress(this->code.TopAddress());
        }
        this->code[addr].SetAddress(this->table->Count());
        this->statement();
    }

    void Parser::identifier(Environment::ID_TYPE tipe, ID_PURPOSE declaration)
    {
        Environment::Token tmp("blank");
        this->match(tmp, Environment::IDENTIFIER);
        Environment::SymbolTableEntry entry(tmp.GetLexeme());

        if (declaration == DECLARE)
        {
            if (tmp.GetSymbolTableEntry() && tmp.GetLevel() < 1)
                this->errors.Push(tmp, DECLARE, this->tokenizer->GetCurrentLine());
            entry.SetIdentifierType(tipe);
            entry.SetTokenValue(tmp.GetTokenValue());
            this->table->Insert(entry);
        }
        else
            if (!tmp.GetSymbolTableEntry())
                this->errors.Push(tmp, USE, this->tokenizer->GetCurrentLine());
            else
            {
                /**
                 * @note CONST can be assigned to.
                 */
                Environment::ID_TYPE got = tmp.GetSymbolTableEntry()->GetIdentifierType();
                if ((tipe == Environment::VARCONST && got != Environment::VAR && got != Environment::CONST) && got != tipe)
                    this->errors.Push(got, tipe, this->tokenizer->GetCurrentLine(), tmp.GetLexeme());
            }
    }

    void Parser::statement()
    {
        Environment::Token tmp("blank");
        if (this->tokenizer->Peek().GetTokenValue() == Environment::IDENTIFIER)
        {
            std::string id = this->tokenizer->Peek().GetLexeme();
            this->identifier(Environment::VAR, USE);
            this->match(tmp, Environment::OPERATOR, ":=");
            this->expression();
            int level;
            Environment::SymbolTableEntry *entry = this->table->Find(id, level);
            this->code.Push(new Instruction("sto", level, entry->GetOffSet()));
        }
        else if (this->tokenizer->Peek().GetLexeme() == "BEGIN")
        {
            this->match(tmp, Environment::KEYWORD, "BEGIN");
            this->statement();
            while (this->tokenizer->Peek().GetLexeme() == ";")
            {
                this->match(tmp, Environment::OPERATOR, ";");
                this->statement();
            }
            this->match(tmp, Environment::KEYWORD, "END");
        }
        else if (this->tokenizer->Peek().GetLexeme() == "IF")
        {
            this->match(tmp, Environment::KEYWORD, "IF");
            this->condition();
            int addr = this->code.TopAddress();
            this->code.Push(new Instruction("jmp", 0, this->code.TopAddress() + 1));
            this->match(tmp, Environment::KEYWORD, "THEN");
            this->statement();
            this->code[addr].SetFunction("jpc");
            this->code[addr].SetAddress(this->code.TopAddress());
        }
        else if (this->tokenizer->Peek().GetLexeme() == "WHILE")
        {
            this->match(tmp, Environment::KEYWORD, "WHILE");
            int addr = this->code.TopAddress();
            this->condition();
            int addr2 = this->code.TopAddress();
            this->code.Push(new Instruction("jpc", 0, this->code.TopAddress() + 1));
            this->match(tmp, Environment::KEYWORD, "DO");
            this->statement();
            this->code.Push(new Instruction("jmp", 0, addr));
            this->code[addr2].SetAddress(this->code.TopAddress());
        }
        else if (this->tokenizer->Peek().GetLexeme() == "READ")
        {
            this->match(tmp, Environment::KEYWORD, "READ");
            this->code.Push(new Instruction("opr", 0, 14));
            std::string id = this->tokenizer->Peek().GetLexeme();
            this->identifier(Environment::VAR, USE);
            int level;
            Environment::SymbolTableEntry *entry = this->table->Find(id, level);
            this->code.Push(new Instruction("sto", level, entry->GetOffSet()));
        }
        else if (this->tokenizer->Peek().GetLexeme() == "CALL")
        {
            this->match(tmp, Environment::KEYWORD, "CALL");
            std::string id = this->tokenizer->Peek().GetLexeme();
            this->identifier(Environment::PROCEDURE, USE);
            int level;
            Environment::SymbolTableEntry *entry = this->table->Find(id, level);
            this->code.Push(new Instruction("cal", 0, entry->GetAddress()));
        }
        else if (this->tokenizer->Peek().GetLexeme() == "PRINT")
        {
            this->match(tmp, Environment::KEYWORD, "PRINT");
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
            this->match(tmp, Environment::OPERATOR);
            this->term();
            this->code.Push(new Instruction("opr", 0, (op == "+") ? 2 : 3));
        }
    }

    void Parser::condition()
    {
        Environment::Token tmp("blank");
        std::string op = this->tokenizer->Peek().GetLexeme();
        if (this->tokenizer->Peek().GetLexeme() == "ODD")
            this->match(tmp, Environment::KEYWORD, "ODD");
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
                this->errors.Push(this->tokenizer->Peek(), Environment::OPERATOR, this->tokenizer->GetCurrentLine());
            this->match(tmp, Environment::OPERATOR);
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
            this->match(tmp, Environment::OPERATOR);
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
            this->match(tmp, Environment::OPERATOR, "(");
            this->expression();
            this->match(tmp, Environment::OPERATOR, ")");
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
            this->match(tmp, Environment::NUMBER);
        }
        else
            this->errors.Push(this->tokenizer->Peek(), Environment::OPERATOR, this->tokenizer->GetCurrentLine());
    }
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
