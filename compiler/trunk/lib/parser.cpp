/*
    Top-down recursive parser implementation.
    Copyright (C) 2009  Alex Brandt

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

#include <iostream>
#include <boost/lexical_cast.hpp>

#include "../include/parser.h"
#include "../include/output.h"
#include "../include/instruction.h"

namespace Analyzer
{
    Parser::Parser(std::istream & in, const int & maxErrors, const bool & verbose, const bool & debug, const bool & warnings)
            : errors(Environment::ERROR_T, maxErrors), debug(debug), table(), tokenizer(in, &table, verbose, debug),
            warnings(Environment::WARNING_T, -1), verbose(verbose), warn(warnings)
    {
    }

    Parser::Parser(const std::string & fileName, const int & maxErrors, const bool & verbose, const bool & debug, const bool & warnings)
            : errors(Environment::ERROR_T, maxErrors), debug(debug), table(),
            tokenizer(fileName, &table, verbose, debug), warnings(Environment::WARNING_T, -1), verbose(verbose),
            warn(warnings)
    {
    }

    std::string Parser::Code()
    {
        return this->code.ToString();
    }

    Environment::TOKEN_VALUE Parser::peek() const
    {
        return this->tokenizer.Peek().GetTokenValue();
    }

    bool Parser::match(Environment::Token & token, const Environment::TOKEN_VALUE & value, const Environment::ERROR_TYPE & level)
    {
        using namespace Environment;

        token = this->tokenizer.Peek();
        if (token.GetTokenValue() != value)
        {
            switch (level)
            {
                case ERROR_T:
                    this->errors.Push(token, value, this->tokenizer.GetCurrentLine());
                    return false;
                case WARNING_T:
                    this->warnings.Push(token, value, this->tokenizer.GetCurrentLine());
                    return true;
            }
        }
        this->tokenizer.Ignore();
#ifndef NDEBUG
        DEBUG(token);
#endif
        return true;
    }

    void Parser::Parse()
    {
        using namespace Environment;
        /**
         * @note Should see the start symbol production followed by $.
         */
        Token tmp;
        this->program();
        this->match(tmp, EOFL);
        VERBOSE("\n" + this->code.ToString());
    }

    void Parser::program()
    {
        using namespace Environment;
        /**
         * program -> PROGRAM block .
         */
        Token tmp;
        this->match(tmp, PROGRAM);
        this->block();
        this->match(tmp, DOT, static_cast<ERROR_TYPE>(this->warn));
    }

    void Parser::block()
    {
        using namespace Environment;
        /**
         * block -> constant_declarations variable_declarations procedure_declarations statement
         */
        Token tmp;
        /**
         * Increment over the variables that may be declared, but don't need space for procedures.
         */
        int stack_increment_instruction = this->code.TopAddress();
        this->code.Push(new Generator::Instruction("int", 0, 3));
        this->constant_declarations();
        this->variable_declarations();
        this->code[stack_increment_instruction].SetAddress(this->table.Count());
        this->procedure_declarations();
        this->statement();
    }

    void Parser::constant_declarations()
    {
        using namespace Environment;
        using namespace Generator;
        using namespace boost;
        /**
         * constant_declarations -> CONST identifier =  sign number more_constants ; constant_declarations | epsilon
         */
        Token tmp;
        std::string id;
        bool negate;
        SymbolTableEntry * entry;

        switch (this->peek())
        {
            case CONST:
                this->match(tmp, CONST);
                this->identifier(tmp, CONST_ID, DECLARE);
                id = tmp.GetLexeme(); // The ID to find in the symbol table.
                this->match(tmp, EQUALS, static_cast<ERROR_TYPE>(this->warn));
                negate = this->sign(); // Whether to negate our number or not.
                if (!this->match(tmp, NUMBER))
                {
                    /**
                     * Panic mode error recovery.
                     * If we don't see a number then we need to resync to an
                     * item in the follow set and die.
                     * Follow(constant_declarations) = {
                     *   VAR, PROCEDURE, IDENTIFIER, IBEGIN, IF, WHILE, READ, CALL, PRINT, DOT, SEMICOLON
                     *   }
                     */
                    while (this->peek() != VAR
                            || this->peek() != PROCEDURE
                            || this->peek() != IDENTIFIER
                            || this->peek() != IBEGIN
                            || this->peek() != IF
                            || this->peek() != WHILE
                            || this->peek() != READ
                            || this->peek() != CALL
                            || this->peek() != PRINT
                            || this->peek() != DOT
                            || this->peek() != SEMICOLON
                          )
                        this->match(tmp, this->peek());
                    break;
                }

                /**
                 * Load our number on the stack.
                 * Negate if necessary.
                 * Store the number in the variable.
                 */
                this->code.Push(new Instruction("lit", 0, lexical_cast<int>(tmp.GetLexeme())));
                if (negate) this->code.Push(new Instruction("opr", 0, 1));
                int level;
                entry = this->table.Find(id, level);
                this->code.Push(new Instruction("sto", level, entry->GetOffSet()));

                this->more_constants();
                this->match(tmp, SEMICOLON, static_cast<ERROR_TYPE>(this->warn));
                this->constant_declarations();
                break;
            default:
                break;
        }
    }

    void Parser::variable_declarations()
    {
        using namespace Environment;
        /**
         * variable_declarations -> VAR identifier more_variables ; variable_declarations | epsilon
         */
        Token tmp;

        switch (this->peek())
        {
            case VAR:
                this->match(tmp, VAR);
                this->identifier(tmp, VAR_ID, DECLARE);
                this->more_variables();
                this->match(tmp, SEMICOLON, static_cast<ERROR_TYPE>(this->warn));
                this->variable_declarations();
                break;
            default:
                break;
        }
    }

    void Parser::procedure_declarations()
    {
        using namespace Environment;
        using namespace Generator;
        /**
         * procedure_declarations -> PROCEDURE identifier ; block ; procedure_declarations | epsilon
         */
        Token tmp;
        int procedure_jump;
        int level;

        switch (this->peek())
        {
            case PROCEDURE:
                this->match(tmp, PROCEDURE);

                procedure_jump = this->code.TopAddress(); // Jump over the procedure's code.
                this->code.Push(new Instruction("jmp", 0, this->code.TopAddress() + 1));

                /**
                 * Get the procedure's name and then set the address in the symbol table.
                 */
                this->identifier(tmp, PROC_ID, DECLARE);
                this->table.Find(tmp.GetLexeme(), level)->SetAddress(this->code.TopAddress());

                this->match(tmp, SEMICOLON, static_cast<ERROR_TYPE>(this->warn));

#ifndef NDEBUG
                DEBUG(this->table);
#endif

                this->table.Push();
                this->block();

#ifndef NDEBUG
                DEBUG(this->table);
#endif

                this->table.Pop();

                this->match(tmp, SEMICOLON, static_cast<ERROR_TYPE>(this->warn));

                this->code.Push(new Instruction("opr", 0, 0));
                this->code[procedure_jump].SetAddress(this->code.TopAddress());

                this->procedure_declarations();
                break;
            default:
                break;
        }
    }

    void Parser::more_constants()
    {
        using namespace Environment;
        using namespace Generator;
        using namespace boost;
        /**
         * more_constants -> , identifier = sign number more_constants | epsilon
         */
        Token tmp;
        std::string id;
        bool negate;
        int level;
        SymbolTableEntry * entry;

        switch (this->peek())
        {
            case COMMA:
                this->match(tmp, COMMA);
                this->identifier(tmp, CONST_ID, DECLARE);
                id = tmp.GetLexeme(); // The ID to find in the symbol table.
                this->match(tmp, EQUALS, static_cast<ERROR_TYPE>(this->warn));
                negate = this->sign(); // Whether to negate our number or not.
                if (!this->match(tmp, NUMBER))
                {
                    /**
                    * Panic mode error recovery.
                    * If we don't see a number then we need to resync to an
                    * item in the follow set and die.
                    * Follow(more_constants) = { SEMICOLON }
                    */
                    while (this->peek() != SEMICOLON)
                        this->match(tmp, this->peek());
                    break;
                }

                /**
                 * Load our number on the stack.
                 * Negate if necessary.
                 * Store the number in the variable.
                 */
                this->code.Push(new Instruction("lit", 0, lexical_cast<int>(tmp.GetLexeme())));
                if (negate) this->code.Push(new Instruction("opr", 0, 1));
                entry = this->table.Find(id, level);
                this->code.Push(new Instruction("sto", level, entry->GetOffSet()));

                this->more_constants();
                break;
            default:
                break;
        }
    }

    void Parser::more_variables()
    {
        using namespace Environment;
        /**
         * more_variables -> , identifier more_variables | epsilon
         */
        Token tmp;

        switch (this->peek())
        {
            case COMMA:
                this->match(tmp, COMMA);
                this->identifier(tmp, VAR_ID, DECLARE);
                this->more_variables();
                break;
            default:
                break;
        }
    }

    void Parser::statement()
    {
        using namespace Environment;
        using namespace Generator;
        /**
         * statement -> identifier := expression
         *            | BEGIN statement more_statements END
         *            | IF condition THEN statement
         *            | WHILE condition DO statement
         *            | READ identifier
         *            | CALL identifier
         *            | PRINT expression
         *            | epsilon
         */
        Token tmp;
        std::string id;
        int level;
        SymbolTableEntry * entry;
        int jump_over_statement;
        int top_of_loop;

        switch (this->peek())
        {
            case IDENTIFIER:
                this->identifier(tmp, VAR_ID, USE);
                id = tmp.GetLexeme(); // Get ID to store into.
                this->match(tmp, ASSIGNMENT, static_cast<ERROR_TYPE>(this->warn));
                this->expression();

                entry = this->table.Find(id, level);
                this->code.Push(new Instruction("sto", level, entry->GetOffSet()));
                break;
            case IBEGIN:
                this->match(tmp, IBEGIN);
                this->statement();
                this->more_statements();
                this->match(tmp, END, static_cast<ERROR_TYPE>(this->warn));
                break;
            case IF:
                this->match(tmp, IF);
                this->condition();

                /**
                 * Conditional jump over the statement.
                 */
                jump_over_statement = this->code.TopAddress();
                this->code.Push(new Instruction("jpc", 0, this->code.TopAddress() + 1));

                this->match(tmp, THEN, static_cast<ERROR_TYPE>(this->warn));
                this->statement();

                /**
                 * Jump to this point.
                 */
                this->code[jump_over_statement].SetAddress(this->code.TopAddress());
                break;
            case WHILE:
                this->match(tmp, WHILE);
                top_of_loop = this->code.TopAddress(); // Top of loop to continue.
                this->condition();

                /**
                 * Jump over the statement if the condition is false.
                 */
                jump_over_statement = this->code.TopAddress();
                this->code.Push(new Instruction("jpc", 0, this->code.TopAddress() + 1));

                this->match(tmp, DO, static_cast<ERROR_TYPE>(this->warn));
                this->statement();

                this->code.Push(new Instruction("jmp", 0, top_of_loop)); // Continue.
                /**
                 * Jump to here if condition is false.
                 */
                this->code[jump_over_statement].SetAddress(this->code.TopAddress());
                break;
            case READ:
                this->match(tmp, READ);
                this->code.Push(new Instruction("opr", 0, 14));
                this->identifier(tmp, VAR_ID, USE);
                entry = this->table.Find(tmp.GetLexeme(), level);
                this->code.Push(new Instruction("sto", level, entry->GetOffSet()));
                break;
            case CALL:
                this->match(tmp, CALL);
                this->identifier(tmp, PROC_ID, USE);
                entry = this->table.Find(tmp.GetLexeme(), level);
                this->code.Push(new Instruction("cal", level, entry->GetAddress()));
                break;
            case PRINT:
                this->match(tmp, PRINT);
                this->expression();
                this->code.Push(new Instruction("opr", 0, 15));
                break;
            default:
                break;
        }
    }

    void Parser::more_statements()
    {
        using namespace Environment;
        /**
         * more_statements -> ; statement more_statements | epsilon
         */
        Token tmp;

        switch (this->peek())
        {
            case SEMICOLON:
                this->match(tmp, SEMICOLON);
                this->statement();
                this->more_statements();
            default:
                break;
        }
    }

    void Parser::expression()
    {
        using namespace Environment;
        /**
         * expression -> term expression_rhs
         */
        this->term();
        this->expression_rhs();
    }

    void Parser::expression_rhs()
    {
        using namespace Environment;
        using namespace Generator;
        /**
         * expression_rhs -> + term expression_rhs | - term epxression_rhs | epsilon
         */
        Token tmp;
        switch (this->peek())
        {
            case PLUS:
                this->match(tmp, PLUS);
                this->term();
                this->expression_rhs();
                this->code.Push(new Instruction("opr", 0, 2));
                break;
            case MINUS:
                this->match(tmp, MINUS);
                this->term();
                this->expression_rhs();
                this->code.Push(new Instruction("opr", 0, 3));
                break;
            default:
                break;
        }
    }

    void Parser::term()
    {
        using namespace Environment;
        using namespace Generator;
        /**
         * term -> sign factor term_rhs
         */
        bool negate = this->sign();
        this->factor();
        if (negate) this->code.Push(new Instruction("opr", 0, 1));
        this->term_rhs();
    }

    void Parser::term_rhs()
    {
        using namespace Environment;
        using namespace Generator;
        /**
         * term_rhs -> * factor term_rhs | / factor term_rhs | DIV factor term_rhs | MOD factor term_rhs | epsilon
         */
        Token tmp;
        switch (this->peek())
        {
            case MULTIPLY:
                this->match(tmp, MULTIPLY);
                this->factor();
                this->code.Push(new Instruction("opr", 0, 4));
                this->term_rhs();
                break;
            case DIVIDE:
                this->match(tmp, DIVIDE);
                this->factor();
                this->code.Push(new Instruction("opr", 0, 5));
                this->term_rhs();
                break;
            case DIV:
                this->match(tmp, DIV);
                this->factor();
                this->code.Push(new Instruction("opr", 0, 5));
                this->term_rhs();
                break;
            case MOD:
                this->match(tmp, MOD);
                this->code.Push(new Instruction("opr", 0, 7));
                this->factor();
                this->code.Push(new Instruction("opr", 0, 5));
                this->code.Push(new Instruction("int", 0, 1));
                this->code.Push(new Instruction("opr", 0, 4));
                this->code.Push(new Instruction("opr", 0, 3));
                this->term_rhs();
                break;
            default:
                break;
        }
    }

    void Parser::factor()
    {
        using namespace Environment;
        using namespace Generator;
        using namespace boost;
        /**
         * factor -> ( expression ) | identifier | number
         */
        Token tmp;
        int level;
        SymbolTableEntry * entry;

        switch (this->peek())
        {
            case LEFTPAREN:
                this->match(tmp, LEFTPAREN);
                this->expression();
                this->match(tmp, RIGHTPAREN, static_cast<ERROR_TYPE>(this->warn));
                break;
            case IDENTIFIER:
                this->identifier(tmp, static_cast<ID_TYPE>(VAR_ID | CONST_ID), USE);
                entry = this->table.Find(tmp.GetLexeme(), level);
                this->code.Push(new Instruction("lod", level, entry->GetOffSet()));
                break;
            case NUMBER:
                this->match(tmp, NUMBER);
                this->code.Push(new Instruction("lit", 0, lexical_cast<int>(tmp.GetLexeme())));
                break;
        }
    }

    void Parser::condition()
    {
        using namespace Environment;
        using namespace Generator;
        /**
         * condition -> ODD expression | expression binary_condition
         */
        Token tmp;

        switch (this->peek())
        {
            case ODD:
                this->match(tmp, ODD);
                this->expression();
                this->code.Push(new Instruction("opr", 0, 6));
                break;
            default:
                this->expression();
                this->binary_condition();
                break;
        }
    }

    void Parser::binary_condition()
    {
        using namespace Environment;
        using namespace Generator;
        /**
         * binary_condition -> ( = | <> | < | > | <= | >= ) expression
         */
        Token tmp;
        std::string error;

        switch (this->peek())
        {
            case EQUALS:
                this->match(tmp, EQUALS);
                this->expression();
                this->code.Push(new Instruction("opr", 0, 8));
                break;
            case NOTEQUAL:
                this->match(tmp, NOTEQUAL);
                this->expression();
                this->code.Push(new Instruction("opr", 0, 9));
                break;
            case LESSTHAN:
                this->match(tmp, LESSTHAN);
                this->expression();
                this->code.Push(new Instruction("opr", 0, 10));
                break;
            case GREATERTHAN:
                this->match(tmp, GREATERTHAN);
                this->expression();
                this->code.Push(new Instruction("opr", 0, 12));
                break;
            case LESSTHANEQUAL:
                this->match(tmp, LESSTHANEQUAL);
                this->expression();
                this->code.Push(new Instruction("opr", 0, 13));
                break;
            case GREATERTHANEQUAL:
                this->match(tmp, GREATERTHANEQUAL);
                this->expression();
                this->code.Push(new Instruction("opr", 0, 11));
                break;
            default:
                this->errors.Push(tmp, BINARYCONDITIONALOPERATOR, this->tokenizer.GetCurrentLine());
                break;
        }
    }

    void Parser::identifier(Environment::Token & token, const Environment::ID_TYPE & type, const Environment::ID_PURPOSE & declaration)
    {
        using namespace Environment;
        this->match(token, IDENTIFIER);

        switch (declaration)
        {
            case DECLARE:
                if (token.GetSymbolTableEntry() && token.GetLevel() < 1)
                    this->errors.Push(token, DECLARE, this->tokenizer.GetCurrentLine());
                /**
                 * @note Potential memory leak if the lexeme already existed in the table.
                 */
                this->table.Insert(token.GetLexeme(), token.GetTokenValue())->SetIdentifierType(type);
                break;
            case USE:
                if (!token.GetSymbolTableEntry()) // Not in SymbolTable
                {
                    this->errors.Push(token, USE, this->tokenizer.GetCurrentLine());
                    throw ErrorQueueError();
                }
                else // Found in SymbolTable
                {
                    ID_TYPE got = token.GetSymbolTableEntry()->GetIdentifierType();
                    if ((type & got) != got)
                    {
                        this->errors.Push(got, type, this->tokenizer.GetCurrentLine(), token.GetLexeme());
                        throw ErrorQueueError();
                    }
                }
                break;
            default:
                /** Cannot get here as ID_PURPOSE has only two values! */
                break;
        }
    }

    bool Parser::sign()
    {
        using namespace Environment;
        /**
         * sign -> + | - | epsilon
         */
        Token tmp;
        switch (this->peek())
        {
            case MINUS:
                this->match(tmp, MINUS);
                return true;
            case PLUS:
                this->match(tmp, PLUS);
            default:
                return false;
        }
    }

    bool Parser::HaveErrors() const
    {
        return this->errors.HaveErrors();
    }
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
