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

#include "../include/programstore.h"
#include "../include/instruction.h"

namespace Generator
{
    void ProgramStore::Mark()
    {
        this->marked_addresses.push(this->TopAddress());
    }

    int ProgramStore::GetMark()
    {
        int ret = this->marked_addresses.top();
        this->marked_addresses.pop();
        return ret;
    }

    void ProgramStore::Push(const Instruction & instruction)
    {
        this->instructions.push_back(instruction);
    }

    void ProgramStore::Push(const Instruction * instruction)
    {
        this->Push(*instruction);
    }

    int ProgramStore::TopAddress() const
    {
        return this->instructions.size();
    }

    Instruction & ProgramStore::operator[](const int & address)
    {
        return this->instructions[address];
    }

    std::string ProgramStore::ToString()
    {
        using namespace boost;
        std::string output = "";
        int count = 0;
        for (std::vector<Instruction>::iterator i = this->instructions.begin(); i != this->instructions.end(); ++i)
            output += lexical_cast<std::string>(count++) + " " + i->ToString();
        return output;
    }
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
