#!/bin/python
# -*- coding: utf-8 -*-

############################################################################
#    Copyright (C) 2008 by Alex Brandt <alunduil@alunduil.com>             #
#                                                                          #
#    This program is free software; you can redistribute it and#or modify  #
#    it under the terms of the GNU General Public License as published by  #
#    the Free Software Foundation; either version 2 of the License, or     #
#    (at your option) any later version.                                   #
#                                                                          #
#    This program is distributed in the hope that it will be useful,       #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#    GNU General Public License for more details.                          #
#                                                                          #
#    You should have received a copy of the GNU General Public License     #
#    along with this program; if not, write to the                         #
#    Free Software Foundation, Inc.,                                       #
#    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
############################################################################

import optparse
import random
import sys

class Fuzzer:
    def __init__(self):
        self._parseOptions()
        self._tokenCount = 0
        self._identifiers = {}
        self._identifiers["functions"] = []
        self._identifiers["variables"] = []
        self._identifiers["constants"] = []

    def _parseOptions(self):
        description_list = [
            "%prog creates programs in the PL/0 language that are ",
            "syntactically correct and perfect for testing a compiler ",
            "for this language."
            ]

        parser = optparse.OptionParser(usage="usage: %prog [options]",
            version="%prog 1.0", description="".join(description_list))

        num_tokens_list = [
            "The minimum number of tokens to put in the output.  This ",
            "will default to a random number between: program . and a ",
            "program containing 1000 tokens."
            ]

        parser.add_option('--tokens', '-t', type="int",
            default=random.randint(500, 1000),
            help="".join(num_tokens_list))

        identifier_length_list = [
            "The maximum length of the identifiers in the output programs."
            ]

        parser.add_option('--varlength', '-l', type="int",
            default = random.randint(16, 64),
            help="".join(identifier_length_list))

        self._options, self._arguments = parser.parse_args()

    def _incrementTokenCount(self):
        self._tokenCount += 1

    def GenerateCode(self):
        output = "PROGRAM\n"
        self._incrementTokenCount()
        self._addLevel()
        output += self._generateBlock(0)
        self._removeLevel()
        output += "\n."
        self._incrementTokenCount()
        return output

    def _addLevel(self):
        self._identifiers["functions"].append([])
        self._identifiers["variables"].append([])
        self._identifiers["constants"].append([])

    def _removeLevel(self):
        self._identifiers["functions"].pop()
        self._identifiers["variables"].pop()
        self._identifiers["constants"].pop()

    def _generateBlock(self, level):
        output = ""
        output += self._generateConstIdentifiers(level)
        output += self._generateVarIdentifiers(level)
        output += self._generateProcedureIdentifiers(level)
        output += self._generateStatement(level)
        return output

    def _maxItems(self):
        return ((self._options.tokens - self._tokenCount)/4)/random.randint(4,16)

    def _haveMaxTokens(self):
        return (self._tokenCount > self._options.tokens - 1)

    def _generateConstIdentifier(self, level):
        output = self._generateIdentifier("constants", level)
        output += " = "
        self._incrementTokenCount()
        choice = random.randint(0, 2)
        if (choice == 0):
            output += " +"
            self._incrementTokenCount()
        elif (choice == 1):
            output += " -"
            self._incrementTokenCount()
        output += self._generateNumber()
        return output

    def _generateProcedureIdentifier(self, level):
        output = "\t"*level + "PROCEDURE "
        self._incrementTokenCount()
        output += self._generateIdentifier("functions", level)
        output += ";\n"
        self._incrementTokenCount()
        self._addLevel()
        output += self._generateBlock(level + 1)
        self._removeLevel()
        output += "\t"*level + ";\n"
        self._incrementTokenCount()
        return output

    def _generateConstIdentifiers(self, level):
        output = ""
        constBlocks = 0
        while (constBlocks < self._maxItems() and not self._haveMaxTokens()):
            output += "\t"*level + "CONST "
            self._incrementTokenCount()
            output += self._generateConstIdentifier(level)
            constIdentifiers = 0
            while (constIdentifiers < self._maxItems() and not self._haveMaxTokens()):
                output += ", "
                self._incrementTokenCount()
                output += self._generateConstIdentifier(level)
                constIdentifiers += 1
            output += ";\n"
            self._incrementTokenCount()
            constBlocks += 1
        return output

    def _generateProcedureIdentifiers(self, level):
        output = ""
        procedures = 0
        while (procedures < self._maxItems() and not self._haveMaxTokens()):
            output += self._generateProcedureIdentifier(level)
        return output

    def _generateVarIdentifiers(self, level):
        output = ""
        varDeclarations = 0
        while (varDeclarations < self._maxItems() and not self._haveMaxTokens()):
            output += "\t"*level + "VAR "
            self._incrementTokenCount()
            output += self._generateIdentifier("variables", level)
            varIdentifiers = 0
            while (varIdentifiers < self._maxItems() and not self._haveMaxTokens()):
                output += ", "
                self._incrementTokenCount()
                output += self._generateIdentifier("variables", level)
                varIdentifiers += 1
            output += ";\n"
            self._incrementTokenCount()
            varDeclarations += 1
        return output

    def _generateIdentifier(self, tipe, level):
        output = self._generateCharacter()
        identifierLength = random.randint(0, self._options.varlength - 1)
        for i in range(identifierLength):
            choice = random.randint(0, 1)
            if (choice == 0):
                output += self._generateCharacter()
            else:
                output += self._generateDigit()
        self._incrementTokenCount()
        self._identifiers[tipe][level].append(output)
        return output

    def _generateCharacter(self):
        return random.choice('ABCDEFGHIJKLMNOPQRSTUVWYXZ')

    def _generateNumber(self):
        output = ""
        numberLength = random.randint(1, 9)
        for i in range(numberLength):
            output += self._generateDigit()
        self._incrementTokenCount()
        return output

    def _generateDigit(self):
        return random.choice('0123456789')

    def _getIdentifier(self, tipe, level):
        if len(self._identifiers[tipe][level]) == 0:
            return False
        identifier = random.sample(self._identifiers[tipe][level], 1)[0]
        self._incrementTokenCount()
        return identifier

    def _generateStatement(self, level):
        output = "\t"*level
        choice = random.randint(0,7)
        if (choice == 0):
            tmp = self._getIdentifier("variables", level)
            if tmp == False:
                return ""
            output += tmp
            output += " := "
            self._incrementTokenCount()
            output += self._generateExpression(level)
        elif (choice == 1):
            output += "BEGIN\n"
            self._incrementTokenCount()
            self._addLevel()
            output += self._generateStatement(level + 1)
            self._removeLevel()
            statements = self._maxItems()
            while (self._tokenCount < statements):
                output += ";\n"
                self._incrementTokenCount()
                self._addLevel()
                output += self._generateStatement(level + 1)
                self._removeLevel()
                statements += 1
            output += "\n" + "\t"*level + "END\n"
            self._incrementTokenCount()
        elif (choice == 2):
            output += "IF "
            self._incrementTokenCount()
            output += self._generateCondition(level)
            output += " THEN\n"
            self._incrementTokenCount()
            self._addLevel()
            output += self._generateStatement(level + 1)
            self._removeLevel()
        elif (choice == 3):
            output += "WHILE "
            self._incrementTokenCount()
            output += self._generateCondition(level)
            output += " DO\n"
            self._incrementTokenCount()
            self._addLevel()
            output += self._generateStatement(level + 1)
            self._removeLevel()
        elif (choice == 4):
            output += "READ "
            self._incrementTokenCount()
            tmp = self._getIdentifier("variables", level)
            if tmp == False:
                return ""
            output += tmp
        elif (choice == 5):
            output += "CALL "
            self._incrementTokenCount()
            tmp = self._getIdentifier("functions", level)
            if tmp == False:
                return ""
            output += tmp
        elif (choice == 6):
            output += "PRINT "
            self._incrementTokenCount()
            tipe = random.randint(0, 1)
            if (tipe == 0):
                tipe = "constants"
            else:
                tipe = "variables"
            tmp = self._getIdentifier(tipe, level)
            if tmp == False:
                return ""
            output += tmp
        else:
            output += "\n"
        output += "\n"
        return output

    def _generateCondition(self, level):
        output = ""
        choice = random.randint(0, 1)
        if (choice == 0):
            output += " ODD "
            self._incrementTokenCount()
            output += self._generateExpression(level)
        else:
            output += self._generateExpression(level)
            choice = random.randint(0, 5)
            if (choice == 0):
                output += " = "
                self._incrementTokenCount()
            elif (choice == 1):
                output += " <> "
                self._incrementTokenCount()
            elif (choice == 2):
                output += " < "
                self._incrementTokenCount()
            elif (choice == 3):
                output += " > "
                self._incrementTokenCount()
            elif (choice == 4):
                output += " <= "
                self._incrementTokenCount()
            elif (choice == 5):
                output += " >= "
                self._incrementTokenCount()
            output += self._generateExpression(level)
        return output

    def _generateExpression(self, level):
        output = ""
        output += self._generateTerm(level)
        terms = 0
        while (terms < self._maxItems() and not self._haveMaxTokens()):
            choice = random.randint(0, 1)
            if (choice == 0):
                output += " + "
                self._incrementTokenCount()
            else:
                output += " - "
                self._incrementTokenCount()
            output += self._generateTerm(level)
            terms += 1
        return output

    def _generateTerm(self, level):
        output = ""
        choice = random.randint(0, 2)
        if (choice == 0):
            output += " +"
            self._incrementTokenCount()
        elif (choice == 1):
            output += " -"
            self._incrementTokenCount()
        output += self._generateFactor(level)
        factors = 0
        while (factors < self._maxItems() and not self._haveMaxTokens()):
            choice = random.randint(0, 3)
            if (choice == 0):
                output += " * "
                self._incrementTokenCount()
            elif (choice == 1):
                output += " / "
                self._incrementTokenCount()
            elif (choice == 2):
                output += " DIV "
                self._incrementTokenCount()
            elif (choice == 3):
                output += " MOD "
                self._incrementTokenCount()
            output += self._generateFactor(level)
            factors += 1
        return output

    def _generateFactor(self, level):
        output = ""
        choice = random.randint(0, 2)
        if (choice == 0):
            output += "("
            self._incrementTokenCount()
            output += self._generateExpression(level)
            output += ")"
            self._incrementTokenCount()
        elif (choice == 1):
            tipe = random.randint(0, 2)
            if (choice == 0):
                tipe = "functions"
            elif (choice == 1):
                tipe = "constants"
            else:
                tipe = "variables"
            tmp = self._getIdentifier(tipe, level)
            if tmp == False:
                return ""
            output += tmp
        elif (choice == 2):
            output += self._generateNumber()
        return output

def main():
    foo = Fuzzer()
    print foo.GenerateCode()

if __name__ == "__main__":
    main()
