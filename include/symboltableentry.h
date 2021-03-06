/*
    Definition of the Symbol Table Entry defined for CSIS 435.
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

#ifndef SYMBOLTABLEENTRY_H
#define SYMBOLTABLEENTRY_H

#include <string>
#include <map>
#include <list>
#include <queue>

#include <parser.hpp>

namespace Environment
{
    /**
     * @brief A definition of ID types.
     */
    enum ID_TYPE
    {
        ARRAY_ID = 1,
        VAR_ID = 2,
        PROC_ID = 4,
        FUNC_ID = 8,
        RETURN_ID = 16
    };

    /**
     * @brief An entry in a symbol table.
     *
     * This class requires 4 elements (lexeme, type, offset, token value), but
     * allows itself to have other properties added to it by using the indexing
     * operator.
     *
     * @note Add the others functionality later if time.
     */
    class SymbolTableEntry
    {
        public:
            /**
             * @brief Set the lexeme for this entry.
             * @param lexeme The name of the entry in the table.
             *
             * Change the name or lexeme of this entry in the symbol table.
             */
            void SetLexeme(const std::string &lexeme);

            /**
             * @brief Get the lexeme for this entry.
             * @return std::string containing entry's identifier.
             *
             * Get the name of the entry in the table.
             */
            std::string GetLexeme() const;

            /**
             * @brief Set the token's type.
             * @param value The type of token stored.
             *
             * Change the type of token of this entry in the symbol table.
             */
            void SetTokenValue(const yytokentype & value);

            /**
             * @brief Get the token's type.
             * @return TOKEN_VALUE specifying the type of the entry.
             *
             * Get the type of token of the entry.
             */
            yytokentype GetTokenValue() const;

            /**
             * @brief Get the entry's value.
             * @return Value of the entry.
             */
            int GetAddress() const;

            /**
             * @brief Set the entry's value.
             * @param value The value of the entry.
             *
             * Change the value of the entry.
             */
            void SetAddress(const int &value);

            /**
             * @brief Set the type of the identifier.
             * @param type The type of the identifier.
             *
             * Change the type of the identifer.
             */
            void SetIdentifierType(const ID_TYPE &type);

            /**
             * @brief Get the type of the identifier.
             * @return ID_TYPE specifying the type of the identifier.
             */
            ID_TYPE GetIdentifierType() const;

            /**
             * @brief Set the ARI addressing offset.
             * @param offset The offset (in bytes) of the entry.
             *
             * Change the offset of the value's position in the ARI.
             */
            void SetOffSet(const int &offset);

            /**
             * @brief Get the ARI addressing offset.
             * @return int specifying the address offset of the value in the ARI.
             *
             * Get the offset of the value's position in the ARI.
             * Result is measured in bytes.
             */
            int GetOffSet() const;

            /**
             * @brief Set the type of the entry.
             * @param type The type of the entry.
             *
             * Set the type of the entry to the type passed.
             */
            void SetType(const TOKEN_TYPE &type);

            /**
             * @brief Get the type of the entry.
             * @return The type of the entry.
             *
             * Gets the type of the entry.
             */
            TOKEN_TYPE GetType() const;

            /**
             * @brief Constructor
             * @param lexeme The identifier for the token.
             * @param offset The ARI addressing offset.
             * @param value The value of the token.
             *
             * Construct Symbol Table Entry.
             */
            SymbolTableEntry(const std::string &lexeme, const int &offset = 0, const yytokentype & value = ID);

            /**
             * @brief Equivalency Operator
             * @param that The other entry to compare against.
             * @return bool indicating similarity between the two entries.
             *
             * Checks if the two entries contain the same lexeme.  The lexeme is our unique
             * identifier and our primary key in this ad-hoc database.
             */
            bool operator==(const SymbolTableEntry &that) const;

            /**
             * @brief Stream insertion operator.
             * @param out The output stream to send to.
             * @param entry The entry to output.
             * @return Output stream reference to stack stream insertions.
             *
             * Insert the entry into the output stream.
             */
            friend std::ostream & operator<<(std::ostream &out, const SymbolTableEntry &entry)
            {
                return entry.print(out);
            }

            /**
             * @brief Set bounds for an array entry.
             * @param lower The lower bound on the array.
             * @param upper The upper bound on the array.
             */
            void SetBounds(const int & lower, const int & upper);

            /**
             * @brief Get the lower bound for an array entry.
             * @return The lower bound of the array.
             */
            int GetLowerBound() const;

            /**
             * @brief Get the upper bound for an array entry.
             * @return The upper bound of the array.
             */
            int GetUpperBound() const;

            /**
             * @brief Return the size of the entry.
             * @return The number of stack elements of this entry.
             */
            int GetSize() const;

            /**
             * @brief Append to the parameter list for a function or procedure.
             * @param param The parameter as a SymbolTableEntry *.
             */
            void AddParameter(const SymbolTableEntry & param);

            /**
             * @brief Get the parameter list for a function or procedure.
             * @return The list of parameters.
             */
            std::queue<SymbolTableEntry> GetParameterList();

        protected:
            /**
             * @brief Print out the entry to the specified stream.
             * @param out The stream to print to.
             * @return A reference to the original stream.
             *
             * Insert the entry into the output stream.
             */
            std::ostream & print(std::ostream &out) const;

        private:
            std::string lexeme;
            int offset;
            yytokentype value;
            TOKEN_TYPE tokenType;
            ID_TYPE idType;
            int address;
            int array_bottom;
            int array_top;
            std::list<SymbolTableEntry> param_list;
    };
};

#endif // SYMBOLTABLEENTRY_H
// kate: indent-mode cstyle; space-indent on; indent-width 4;
