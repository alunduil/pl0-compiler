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

#ifndef PROGRAMSTORE_H
#define PROGRAMSTORE_H

#include <vector>
#include <instruction.h>

namespace LexicalAnalyzer
{
    class ProgramStore
    {
        public:
            /**
             * @brief Push instruction.
             * @param instruction The Instruction to push on the stack.
             */
            void Push(const Instruction &instruction);

            /**
             * @brief Push Instruction.
             * @param instruction The Instruction to push on the stack.
             */
            void Push(const Instruction *instruction);

            /**
             * @brief Get the address of the top of the stack.
             * @return The index of the top of the address.
             */
            int TopAddress() const;

            /**
             * @brief Index access operator to have random access to the stack.
             * @param address The address in the stack.
             * @return Reference to the instruction.
             */
            Instruction & operator[](const int address);

            /**
             * @brief Convert the instructions to a string.
             * @return The string of instructions.
             */
            std::string ToString();
        private:
            std::vector<Instruction> instructions;
    };
}

#endif // PROGRAMSTORE_H
// kate: indent-mode cstyle; space-indent on; indent-width 4;
