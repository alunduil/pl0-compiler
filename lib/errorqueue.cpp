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

#include <iostream>
#include <boost/lexical_cast.hpp>

#include "../include/errorqueue.h"
#include "../include/output.h"
#include "../include/token.h"

namespace Environment
{
    void ErrorQueue::Push(const Token & got, const TOKEN_VALUE & expected, const int & line, const std::string & msg)
    {
        using namespace boost;
        std::string error = lexical_cast<std::string>(line);
        error += (this->type == ERROR_T) ? ": error: " : ": warning: ";
        error += "expected `";
        error += this->castTokenValue(expected);
        error += "' but got `";
        error += got.GetLexeme();
        error += "' token";
        if (msg.length() > 0) error += ": " + msg;
        if (this->type == ERROR_T) ERROR(error);
        else WARNING(error);
        this->checkCount();
    }

    std::string ErrorQueue::castTokenValue(const TOKEN_VALUE & expected) const
    {
        return (expected == IDENTIFIER) ? "identifier" :
               (expected == ASSIGNMENT) ? ":=" :
               (expected == NUMBER) ? "number" :
               (expected == IBEGIN) ? "begin" :
               (expected == GARBAGE) ? "garbage" :
               (expected == EOFL) ? "eof" :
               (expected == CALL) ? "call" :
               (expected == COMMA) ? "," :
               (expected == CONST) ? "const" :
               (expected == DIV) ? "div" :
               (expected == DIVIDE) ? "/" :
               (expected == DO) ? "do" :
               (expected == DOT) ? "." :
               (expected == END) ? "end" :
               (expected == EQUALS) ? "=" :
               (expected == GREATERTHAN) ? ">" :
               (expected == GREATERTHANEQUAL) ? ">=" :
               (expected == IF) ? "if" :
               (expected == LEFTPAREN) ? "(" :
               (expected == LESSTHAN) ? "<" :
               (expected == LESSTHANEQUAL) ? "<=" :
               (expected == MINUS) ? "-" :
               (expected == MOD) ? "mod" :
               (expected == MULTIPLY) ? "*" :
               (expected == NOTEQUAL) ? "<>" :
               (expected == ODD) ? "odd" :
               (expected == PLUS) ? "+" :
               (expected == PRINT) ? "print" :
               (expected == PROCEDURE) ? "procedure" :
               (expected == PROGRAM) ? "program" :
               (expected == READ) ? "read" :
               (expected == RIGHTPAREN) ? ")" :
               (expected == SEMICOLON) ? ";" :
               (expected == THEN) ? "then" :
               (expected == VAR) ? "var" :
               (expected == WHILE) ? "while" :
               (expected == BINARYCONDITIONALOPERATOR) ? "( = | <> | < | > | <= | >= )" : "NOT FOUND!";
    }

    void ErrorQueue::checkCount()
    {
        this->hasErrors = true;
        if (++this->count == this->maxErrors) throw ErrorQueueError();
    }

    void ErrorQueue::Push(const Token & got, const ID_PURPOSE & declaration, const int & line, const std::string & msg)
    {
        using namespace boost;
        std::string error = lexical_cast<std::string>(line);
        error += ": error: ";
        error += (declaration == DECLARE) ? "redeclaration of '" + got.GetLexeme() + "'" : "'" + got.GetLexeme() + "' was not declared in this scope";
        if (msg.length() > 0) error += ": " + msg;
        ERROR(error);
        this->checkCount();
    }

    std::string ErrorQueue::castIdType(const ID_TYPE & expected) const
    {
        return (expected == VAR_ID) ? "VAR" :
               (expected == CONST_ID) ? "CONST" :
               (expected == PROC_ID) ? "PROCEDURE" :
               (expected == VAR_ID | CONST_ID) ? "VAR or CONST" : "";
    }

    void ErrorQueue::Push(const ID_TYPE & got, const ID_TYPE & expected, const int & line, const std::string & msg)
    {
        using namespace boost;
        std::string error = lexical_cast<std::string>(line);
        error += ": error: expected `";
        error += this->castIdType(expected);
        error += "' but got `";
        error += this->castIdType(got);
        error += "' identifier";
        if (msg.length() > 0) error += ": " + msg;
        ERROR(error);
        this->checkCount();
    }

    ErrorQueue::ErrorQueue(const ERROR_TYPE & type, const int & maxErrors)
            : count(0), hasErrors(false), type(type), maxErrors(maxErrors)
    {
    }

    bool ErrorQueue::HaveErrors() const
    {
        return this->hasErrors;
    }
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
