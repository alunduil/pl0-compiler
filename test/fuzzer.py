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
        self._options, self._arguments = self._parseOptions()
        self._tokenCount = 0
        self._identifiers = {"functions" : [], "variables" : [], "constants" : []}
        self._output = ""
        self._generateProgram()

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

        return parser.parse_args()

    def _addTerminal(self, terminal):
        self._output += terminal
        self._tokenCount += 1

    def _addLevel(self):
        self._identifiers["functions"].append([])
        self._identifiers["variables"].append([])
        self._identifiers["constants"].append([])

    def _removeLevel(self):
        self._identifiers["functions"].pop()
        self._identifiers["variables"].pop()
        self._identifiers["constants"].pop()

    def _maxItems(self):
        tmp = (self._options.tokens)/random.randint(max(self._tokenCount, 1),max(self._tokenCount, self._options.tokens))
        return tmp

    def _haveMaxTokens(self):
        return (self._tokenCount > self._options.tokens - 1)

    def GetCode(self):
        return self._output

    def _generateProgram(self):
        self._addTerminal("PROGRAM\n")
        self._generateBlock(-1)
        self._addTerminal("\n.\n")

    def _generateBlock(self, level):
        self._addLevel()
        self._generateConstantDeclarations(level + 1)
        self._generateVariableDeclarations(level + 1)
        self._generateProcedureDeclarations(level + 1)
        self._generateStatement(level + 1)
        self._removeLevel()

    def _generateConstantDeclarations(self, level):
        if self._tokenCount == self._maxItems() or self._haveMaxTokens():
            return
        choice = random.randint(0, 1)
        if choice == 0:
            identifier = self._generateIdentifier("constants", level)
            if identifier != False:
                return
            self._addTerminal("  "*level + "CONST ")
            self._addTerminal(identifier + " ")
            self._addTerminal("= ")
            self._generateSign()
            self._generateNumber()
            self._generateMoreConstants(level)
            self._addTerminal(";\n")
            self._generateConstantDeclarations(level)
        return

    def _generateVariableDeclarations(self, level):
        if self._tokenCount == self._maxItems() or self._haveMaxTokens():
            return
        choice = random.randint(0, 1)
        if choice == 0:
            self._addTerminal("  "*level + "VAR ")
            self._generateIdentifier("variables", level)
            self._generateMoreVariables(level)
            self._addTerminal(";\n")
            self._generateVariableDeclarations(level)
        return

    def _generateProcedureDeclarations(self, level):
        if self._tokenCount == self._maxItems() or self._haveMaxTokens():
            return
        choice = random.randint(0, 1)
        if choice == 0:
            self._addTerminal("  "*level + "PROCEDURE ")
            self._generateIdentifier("functions", level)
            self._addTerminal(";\n")
            self._generateBlock(level)
            self._addTerminal("  "*level + ";\n")
            self._generateProcedureDeclarations(level)
        return

    def _generateMoreConstants(self, level):
        if self._tokenCount == self._maxItems() or self._haveMaxTokens():
            return
        choice = random.randint(0, 1)
        if choice == 0:
            self._addTerminal(", ")
            self._generateIdentifier("constants", level)
            self._addTerminal("= ")
            self._generateSign()
            self._generateNumber()
            self._generateMoreConstants(level)
        return

    def _generateMoreVariables(self, level):
        if self._tokenCount == self._maxItems() or self._haveMaxTokens():
            return
        choice = random.randint(0, 1)
        if choice == 0:
            self._addTerminal(", ")
            self._generateIdentifier("variables", level)
            self._generateMoreVariables(level)
        return

    def _generateStatement(self, level):
        if self._tokenCount == self._maxItems() or self._haveMaxTokens():
            return
        choice = random.randint(0, 7)
        if choice == 0:
            identifier = self._getIdentifier("variables", level)
            if identifier != False:
                self._addTerminal(identifier + " ")
                self._addTerminal(":= ")
                self._generateExpression(level)
        elif choice == 1:
            self._addTerminal("BEGIN\n")
            self._generateStatement(level)
            self._generateMoreStatements(level)
            self._addTerminal("END\n")
        elif choice == 2:
            self._addTerminal("IF ")
            self._generateCondition(level)
            self._addTerminal("THEN ")
            self._generateStatement(level)
        elif choice == 3:
            self._addTerminal("WHILE ")
            self._generateCondition(level)
            self._addTerminal("DO ")
            self._generateStatement(level)
        elif choice == 4:
            identifier = self._getIdentifier("variables", level)
            if identifier != False:
                self._addTerminal("READ ")
                self._addTerminal(identifier + " ")
        elif choice == 5:
            identifier = self._getIdentifier("functions", level)
            if identifier != False:
                self._addTerminal("CALL ")
                self._addTerminal(identifier)
        elif choice == 6:
            self._addTerminal("PRINT ")
            self._generateExpression(level)
        return

    def _generateMoreStatements(self, level):
        if self._tokenCount == self._maxItems() or self._haveMaxTokens():
            return
        choice = random.randint(0, 1)
        if choice == 0:
            self._addTerminal(";\n")
            self._generateStatement(level)
            self._generateMoreStatements(level)
        return

    def _generateExpression(self, level):
        self._generateTerm(level)
        self._generateExpressionRHS(level)

    def _generateExpressionRHS(self, level):
        if self._tokenCount == self._maxItems() or self._haveMaxTokens():
            return
        choice = random.randint(0, 2)
        if choice == 0:
            self._addTerminal("+ ")
            self._generateTerm(level)
            self._generateExpressionRHS(level)
        elif choice == 1:
            self._addTerminal("- ")
            self._generateTerm(level)
            self._generateExpressionRHS(level)
        return

    def _generateTerm(self, level):
        self._generateSign()
        self._generateFactor(level)
        self._generateTermRHS(level)

    def _generateTermRHS(self, level):
        if self._tokenCount == self._maxItems() or self._haveMaxTokens():
            return
        choice = random.randint(0, 4)
        if choice == 0:
            self._addTerminal("* ")
            self._generateFactor(level)
            self._generateTermRHS(level)
        elif choice == 1:
            self._addTerminal("/ ")
            self._generateFactor(level)
            self._generateTermRHS(level)
        elif choice == 2:
            self._addTerminal("DIV ")
            self._generateFactor(level)
            self._generateTermRHS(level)
        elif choice == 3:
            self._addTerminal("MOD ")
            self._generateFactor(level)
            self._generateTermRHS(level)
        return

    def _generateFactor(self, level):
        choice = random.randint(0, 2)
        if choice == 0:
            self._addTerminal("(")
            self._generateExpression(level)
            self._addTerminal(")")
        elif choice == 1:
            tipe = random.randint(0, 1)
            if tipe == 1:
                identifier = self._getIdentifier("constants", level)
                if not identifier:
                    self._generateNumber()
            else:
                identifier = self._getIdentifier("variables", level)
                if not identifier:
                    self._generateNumber()
        else:
            self._generateNumber()

    def _generateCondition(self, level):
        choice = random.randint(0, 1)
        if choice == 0:
            self._addTerminal("ODD ")
            self._generateExpression(level)
        else:
            self._generateExpression(level)
            self._generateBinaryCondition(level)
        return

    def _generateBinaryCondition(self, level):
        choice = random.randint(0, 5)
        if choice == 0:
            self._addTerminal("= ")
            self._generateExpression(level)
        elif choice == 1:
            self._addTerminal("<> ")
            self._generateExpression(level)
        elif choice == 2:
            self._addTerminal("< ")
            self._generateExpression(level)
        elif choice == 3:
            self._addTerminal("> ")
            self._generateExpression(level)
        elif choice == 4:
            self._addTerminal("<= ")
            self._generateExpression(level)
        else:
            self._addTerminal(">= ")
            self._generateExpression(level)
        return

    def _generateSign(self):
        if self._tokenCount == self._maxItems() or self._haveMaxTokens():
            return
        choice = random.randint(0, 2)
        if choice == 0:
            self._addTerminal("+ ")
        elif choice == 1:
            self._addTerminal("- ")
        return

    def _generateIdentifier(self, tipe, level):
        output = self._generateCharacter()
        identifierLength = random.randint(0, self._options.varlength - 1)
        for i in range(identifierLength):
            choice = random.randint(0, 1)
            if choice == 0:
                output += self._generateCharacter()
            else:
                output += self._generateDigit()
        if (self._identifiers["functions"][level].count(output) > 0
            or self._identifiers["variables"][level].count(output) > 0
            or self._identifiers["constants"][level].count(output) > 0
            ):
            output = self._generateIdentifier(tipe, level)
        # Check that we don't use keywords as identifiers.
        if (output == "DO"
            or output == "IF"
            or output == "VAR"
            or output == "END"
            or output == "MOD"
            ):
            output = self._generateIdentifier(tipe, level)
        self._identifiers[tipe][level].append(output)
        self._addTerminal(output)
        return

    def _generateCharacter(self):
        return random.choice('ABCDEFGHIJKLMNOPQRSTUVWYXZ')

    def _generateNumber(self):
        output = ""
        numberLength = random.randint(1, 9)
        for i in range(numberLength):
            output += self._generateDigit()
        self._addTerminal(output + " ")
        return

    def _generateDigit(self):
        return random.choice('0123456789')

    def _getIdentifier(self, tipe, level):
        realLevel = random.randint(0, level)
        if len(self._identifiers[tipe][realLevel]) < 1:
            return False
        identifier = random.sample(self._identifiers[tipe][realLevel], 1)[0]
        for l in range(realLevel, level):
            if self._identifiers[tipe][l].count(identifier) > 0 and l < level:
                return False
        return identifier

if __name__ == "__main__":
    foo = Fuzzer()
    print foo.GetCode()
