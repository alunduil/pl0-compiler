/*
    A particular instruction for an fsti stack machine interpreter.
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

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>

namespace Generator
{
    class Instruction
    {
        public:
            /**
             * @brief Constructor.
             * @param function The function for this instruction.
             * @param level The level associated with this function.
             * @param address The address for the function.
             */
            Instruction(const std::string & function, const int & level, const int & address);

            /**
             * @brief Constructor.
             * @param function The function for this instruction.
             * @param level The level associated with this function.
             * @param address The address for the function.
             */
            Instruction(const std::string & function, const int & level, const float & address);

            /**
             * @brief Get the function of the instruction.
             * @return The function.
             */
            std::string GetFunction() const;

            /**
             * @brief Set the function of the instruction.
             * @param function The function to store.
             */
            void SetFunction(const std::string & function);

            /**
             * @brief Get the level of the instruction.
             * @return The level.
             */
            int GetLevel() const;

            /**
             * @brief Set the level of the instruction.
             * @param level The level to store.
             */
            void SetLevel(const int & level);

            /**
             * @brief Get the address of the instruction.
             * @return The address.
             */
            int GetAddress() const;

            /**
             * @brief Set the address of the instruction.
             * @param address The address to store.
             */
            void SetAddress(const int & address);

            /**
             * @brief Set the address of the instruction.
             * @param address The address to store.
             */
            void SetAddress(const float & address);

            /**
             * @brief Convert to string.
             * @return A string for the instruction.
             */
            std::string ToString() const;
        private:
            std::string function;
            int level;
            union
            {
                int inum;
                float rnum;
            } address;
            enum
            {
                INUM,
                RNUM
            } type;
    };
}

#endif // INSTRUCTION_H
// kate: indent-mode cstyle; space-indent on; indent-width 4;
