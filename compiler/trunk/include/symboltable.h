/*
    Definition of the Symbol Table defined for CSIS 435.
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

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <list>
#include <map>

#include <parser.hpp>

namespace Environment
{
    class SymbolTableEntry;

    class SymbolTable
    {
        public:
            /**
             * @brief Constructor
             *
             * Construct Symbol Table.
             */
            SymbolTable();

            /**
             * @brief Destructor
             *
             * Destructor Symbol Table.
             */
            ~SymbolTable();

            /**
             * @brief Count of elements in top level.
             * @return The count of elements in the top level.
             */
            int Count() const;

            /**
             * @brief Find an entry in the table based on the lexeme.
             * @param lexeme The unique identifier of the word representing the token.
             * @param level An inout parameter that returns the number of references to
             *   the table containing the desired lexeme.
             * @return SymbolTableEntry pointer of the found entry with the desired
             *   lexeme or null if not found.
             *
             * Search for the lexeme in the table and return a reference to the entry if
             * it is found.  If it is not in the table return null so we can have testing
             * on the return value for the user.
             *
             * @note Adding const after this prototype throws a compile error . . . why?
             */
            SymbolTableEntry * Find(const std::string & lexeme, int & level);

            /**
             * @brief Overloaded version of the previous that takes an entry.
             * @param entry The Symbol Table Entry reference to find.
             * @param level The level of the entry in our sequence of tables.
             * @return SymbolTableEntry pointer of the found entry with the desired
             *   lexeme or null if not found.
             *
             * Same as the previous definition of Find, but takes a Symbol Table Entry
             * rather than a lexeme.
             */
            SymbolTableEntry * Find(const SymbolTableEntry & entry, int & level);

            /**
             * @brief Insert the lexeme into the table.
             * @param lexeme The lexeme to be inserted.
             * @param token The value of the token (found in TOKEN_VALUE)
             * @pre lexeme does not exist in the table already.
             * @return SymbolTableEntry pointer of the new entry.
             *
             * Check that the symbol is not already in the table and then insert
             * it into the table if this is the case.  If the entry exists it will
             * be overwritten.  CHECK THAT THE ENTRY DOES NOT EXIST BEFORE CALLING!
             */
            SymbolTableEntry * Insert(const std::string & lexeme, const yytokentype & token = ID);

            /**
             * @brief Insert the Symbol Table Entry into the table.
             * @param entry SymbolTableEntry reference to insert into the table.
             * @pre entry does not exist in the table already.
             * @return SymbolTableEntry pointer of the new entry.
             *
             * Check that the entry is not already in the table and then insert it.
             * If the entry is already in the table it will be overwritten.
             * CHECK THAT THE ENTRY DOES NOT EXIST BEFORE CALLING!
             */
            SymbolTableEntry * Insert(const SymbolTableEntry & entry);

            /**
             * @brief Push a new scope onto the System Table.
             *
             * Add another scope to the symbol table by putting it on an internal stack.
             */
            void Push();

            /**
             * @brief Pop the top most scope out of the Symbol Table.
             *
             * Remove the top level scoping from the Symbol Table.
             */
            void Pop();

            /**
             * @brief Stream insertion operator.
             * @param out The output stream to send to.
             * @param table The table to output.
             * @return Output stream reference to stack stream insertions.
             *
             * Insert the table into the output stream.
             */
            friend std::ostream & operator<<(std::ostream & out, SymbolTable & table)
            {
                return table.print(out);
            }

        protected:
            /**
             * @brief Print the table with proper level indication.
             * @param out The output stream to send to.
             * @return Output the stream reference to stack stream insertions.
             *
             * Print the table with an appropriate level mark.
             */
            std::ostream & print(std::ostream &out);

            class InternalSymbolTable
            {
                public:
                    /**
                     * @brief Constructor.
                     *
                     * Constructor.
                     */
                    InternalSymbolTable();

                    /**
                     * @brief Destructor.
                     *
                     * Destructor.
                     */
                    ~InternalSymbolTable();

                    /**
                     * @brief Count of elements in table.
                     * @return The number of elements at this level.
                     */
                    int Count();

                    /**
                    * @brief Find an entry in the table based on the lexeme.
                    * @param lexeme The unique identifier of the word representing the token.
                    * @return SymbolTableEntry pointer of the found entry with the desired
                    *   lexeme or null if not found.
                    *
                    * Search for the lexeme in the table and return a reference to the entry if
                    * it is found.  If it is not in the table return null so we can have testing
                    * on the return value for the user.
                    */
                    SymbolTableEntry * Find(const std::string & lexeme);

                    /**
                    * @brief Overloaded version of the previous that takes an entry.
                    * @param entry The Symbol Table Entry reference to find.
                    * @return SymbolTableEntry pointer of the found entry with the desired
                    *   lexeme or null if not found.
                    *
                    * Same as the previous definition of Find, but takes a Symbol Table Entry
                    * rather than a lexeme.
                    */
                    SymbolTableEntry * Find(const SymbolTableEntry & entry);

                    /**
                    * @brief Insert the lexeme into the table.
                    * @param lexeme The lexeme to be inserted.
                    * @param token The value of the token (found in TOKEN_VALUE)
                    * @pre lexeme does not exist in the table already.
                    * @return SymbolTableEntry pointer to the newly inserted entry.
                    *
                    * Check that the symbol is not already in the table and then insert
                    * it into the table if this is the case.  If the entry exists it will
                    * be overwritten.  CHECK THAT THE ENTRY DOES NOT EXIST BEFORE CALLING!
                    */
                    SymbolTableEntry * Insert(const std::string & lexeme, const yytokentype & token = ID);

                    /**
                    * @brief Insert the Symbol Table Entry into the table.
                    * @param entry SymbolTableEntry reference to insert into the table.
                    * @pre entry does not exist in the table already.
                    * @return SymbolTableEntry pointer to the newly inserted entry.
                    *
                    * Check that the entry is not already in the table and then insert it.
                    * If the entry is already in the table it will be overwritten.
                    * CHECK THAT THE ENTRY DOES NOT EXIST BEFORE CALLING!
                    */
                    SymbolTableEntry * Insert(SymbolTableEntry & entry);

                    /**
                     * @brief Stream insertion operator.
                     * @param out The output stream to send to.
                     * @param table The table to output.
                     * @return Output stream reference to stack stream insertions.
                     *
                     * Insert the table into the output stream.
                     */
                    friend std::ostream & operator<<(std::ostream & out, InternalSymbolTable & table)
                    {
                        return table.print(out);
                    }

                protected:
                    /**
                     * @brief Print the table with proper level indication.
                     * @param out The output stream to send to.
                     * @return Output the stream reference to stack stream insertions.
                     *
                     * Insert the table with an appropriate level mark.
                     */
                    std::ostream & print(std::ostream & out);

                private:
                    std::map<std::string, SymbolTableEntry *> table;
                    int count;
            };
        private:
            std::list<InternalSymbolTable *> tables;
    };
};

#endif // SYMBOLTABLE_H
// kate: indent-mode cstyle; space-indent on; indent-width 4;
