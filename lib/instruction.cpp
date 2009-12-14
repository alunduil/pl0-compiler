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

#include <boost/lexical_cast.hpp>
#include <sstream>
#include <iomanip>

#include "../include/instruction.h"

namespace Generator
{
    Instruction::Instruction(const std::string & function, const int & level, const int & address)
            : function(function), level(level), type(INUM)
    {
        this->address.inum = address;
    }

    Instruction::Instruction(const std::string & function, const int & level, const float & address)
            : function(function), level(level), type(RNUM)
    {
        this->address.rnum = address;
    }

    void Instruction::SetAddress(const int & address)
    {
        this->address.inum = address;
    }

    void Instruction::SetAddress(const float & address)
    {
        this->address.rnum = address;
    }

    std::string Instruction::ToString() const
    {
        using namespace boost;
        using namespace std;

        stringstream fltcvt;

        if (this->type == RNUM) fltcvt << setprecision(15) << scientific << this->address.rnum;
        else fltcvt << this->address.inum;

        return this->function + " " + lexical_cast<std::string>(this->level) + " " + fltcvt.str() + "\n";
    }

    void Instruction::SetFunction(const std::string & function)
    {
        this->function = function;
    }

    std::string Instruction::GetFunction() const
    {
        return this->function;
    }

    int Instruction::GetAddress() const
    {
        /**
         * @todo fix this to handle the floating point case.
         */
        return this->address.inum;
    }
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
