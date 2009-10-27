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

#include <instruction.h>
#include <boost/lexical_cast.hpp>

namespace LexicalAnalyzer
{
    Instruction::Instruction(std::string function, int level, int address)
            : function(function), level(level), address(address)
    {
    }

    void Instruction::SetAddress(const int &address)
    {
        this->address = address;
    }

    std::string Instruction::ToString() const
    {
        return this->function + " " + boost::lexical_cast<std::string, int>(this->level) + " " + boost::lexical_cast<std::string, int>(this->address) + "\n";
    }

    void Instruction::SetFunction(const std::string &function)
    {
        this->function = function;
    }
}
// kate: indent-mode cstyle; space-indent on; indent-width 4; 
