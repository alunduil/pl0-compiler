/*
    Error passing container for a compiler implementation.
    Copyright (C) 2009 Alex Brandt

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

#ifndef ERRORQUEUE_H
#define ERRORQUEUE_H

#include <string>
#include <token.h>
#include <symboltableentry.h>

namespace Environment
{
    class Token;
    class SymbolTableEntry;
}

namespace LexicalAnalyzer
{
    enum ID_PURPOSE
    {
        DECLARE,
        USE
    };

    class ErrorQueue
    {
        public:
            /**
             * @brief Constructor.
             */
            ErrorQueue(int maxErrors = 100);

            /**
             * @brief Push an error to the error stream.
             * @param error The error to output.
             */
            void Push(std::string error);

            /**
             * @brief Push an error to the error stream.
             * @param got The token we got.
             * @param expected The token we expected.
             * @param line The line we found the error.
             * @param msg Extra information.
             */
            void Push(const Environment::Token &got, const Environment::TOKEN_VALUE &expected, const int &line, const std::string &msg = "");

            /**
             * @brief Push an error to the error stream.
             * @param got The token we got.
             * @param declaration USE or DECLARE of identifier.
             * @param line The line we found the error.
             * @param msg Extra information.
             */
            void Push(const Environment::Token &got, const ID_PURPOSE &declaration, const int &line, const std::string &msg = "");

            /**
             * @brief Push an error to the error stream.
             * @param got The ID_TYPE we got.
             * @param expected The ID_TYPE we expected.
             * @param line The line we found the error.
             * @param msg Extra information.
             */
            void Push(const Environment::ID_TYPE &got, const Environment::ID_TYPE &expected, const int &line, const std::string &msg = "");

            /**
             * @brief Tells the caller if we've seen any errors.
             */
            bool HaveErrors() const;

        private:
            int count;
            int maxErrors;
            bool hasErrors;

            /**
             * @brief Check the error count.
             *
             * Increments the error count and if we have more than the maximum throws
             * an ErrorQueueError exception.
             */
            void checkCount();

            /**
             * @brief Cast TOKEN_VALUE to string.
             * @param expected The value to convert.
             */
            std::string castTokenValue(const Environment::TOKEN_VALUE &expected) const;

            /**
             * @brief Cast ID_TYPE to string.
             * @param expected The value to convert.
             */
            std::string castIdType(const Environment::ID_TYPE &expected) const;
    };

    class ErrorQueueError
    {
    };
}

#endif // ERRORQUEUE_H
// kate: indent-mode cstyle; space-indent on; indent-width 4;