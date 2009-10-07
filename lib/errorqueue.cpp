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

#include <errorqueue.h>
#include <token.h>
#include <iostream>

namespace LexicalAnalyzer
{
    void ErrorQueue::Push(const Environment::Token &got, const Environment::TOKEN_VALUE &expected, const int &line, const std::string &msg)
    {

        std::cerr << line << ": error: expected `" << this->castTokenValue(expected) << "' but got `" << got << "' token";
        if (msg.length() > 0) std::cerr << ": " << msg;
        std::cerr << std::endl;
        this->checkCount();
    }

    std::string ErrorQueue::castTokenValue(const Environment::TOKEN_VALUE &expected)
    {
        return (expected == Environment::IDENTIFIER) ? "identifier" :
            (expected == Environment::KEYWORD) ? "keyword" :
            (expected == Environment::NUMBER) ? "number" :
            (expected == Environment::OPERATOR) ? "operator" :
            (expected == Environment::GARBAGE) ? "garbage" :
            (expected == Environment::EOFL) ? "eof" : "";
    }

    void ErrorQueue::checkCount()
    {
        if (++this->count == this->maxErrors) throw new ErrorQueueError();
    }

    void ErrorQueue::Push(const Environment::Token &got, const ID_PURPOSE &declaration, const int &line, const std::string &msg)
    {
        std::cerr << line;
        if (declaration == DECLARE) std::cerr << ": error: redeclaration of '" << got.GetLexeme() << "'";
        else std::cerr << ": error: '" << got.GetLexeme() << "' was not declared in this scope";
        if (msg.length() > 0) std::cerr << ": " << msg;
        std::cerr << std::endl;
        this->checkCount();
    }

    std::string ErrorQueue::castIdType(const Environment::ID_TYPE &expected)
    {
        return (expected == Environment::VAR) ? "VAR" :
            (expected == Environment::CONST) ? "CONST" :
            (expected == Environment::PROCEDURE) ? "PROCEDURE" :
            (expected == Environment::VARCONST) ? "VAR or CONST" : "";
    }

    void ErrorQueue::Push(const Environment::ID_TYPE &got, const Environment::ID_TYPE &expected, const int &line, const std::string &msg)
    {
        std::cerr << line << ": error: expected `" << this->castIdType(expected) << "' but got `" << this->castIdType(got) << "' identifier";
        if (msg.length() > 0) std::cerr << ": " << msg;
        std::cerr << std::endl;
        this->checkCount();
    }

    ErrorQueue::ErrorQueue(int maxErrors)
    {
        this->maxErrors = maxErrors;
    }
}
