%debug
%error-verbose
%require "2.4.1"
/* Expect the s/r conflict for the if then else ambiguity. */
%expect 1

%code top {
    #include <iostream>
    #include <string>
    #include <list>
    #include <queue>
    #include <boost/lexical_cast.hpp>

    #include <output.h>
    #include <symboltableentry.h>
    #include <symboltable.h>
    #include <programstore.h>
    #include <instruction.h>
}

%code requires {
    #ifndef __ENVIRONMENT_PARSER_H
    #define __ENVIRONMENT_PARSER_H
    namespace Environment
    {
        class SymbolTableEntry;
        /**
         * @brief A definition of the different types.
         * @todo Define these values.
         */
        enum TOKEN_TYPE
        {
            REAL = 513,
            INTEGER
        };
    }
    #endif

    #ifndef __TOKEN_TYPES_PARSER_H
    #define __TOKEN_TYPES_PARSER_H
    enum RELOP_TYPE { EQ, NE, LT, GT, LE, GE };
    enum SIGN { UPLUS, UMINUS };
    enum ADDOP { BPLUS, BMINUS, BOR };
    enum MULOP { BMULTIPLY, BDIVIDE, BDIV, BMOD, BAND };
    #endif

    #define YY_DECL int yylex(YYSTYPE * yylval)
    #define YYLEX_PARAM &yylval
}

%parse-param { const std::string & filename }
%parse-param { Generator::ProgramStore * output_code }
/**
 * @note Possible BISON bug?  This should define YYLEX_PARAM right?
%lex-param { YYSTYPE * yylval }
 */

%union {
    int inum;
    float rnum;
    std::string * str;
    Environment::TOKEN_TYPE token_type;
    RELOP_TYPE relop_type;
    SIGN sign;
    ADDOP addop;
    MULOP mulop;
    Environment::SymbolTableEntry * symtabentry;
}

%code {
    YY_DECL;
    extern int yylineno;
    /**
     * @note DIRTY DIRTY HACK FOR YYERROR ...
     */
    #define yyerror(filename, output_code, msg) dirty_yyerror((filename), (msg));
    void dirty_yyerror(const std::string &, const std::string &);

    void array_index_code_generator(Environment::SymbolTableEntry *, Generator::ProgramStore *);

    std::list<Environment::SymbolTableEntry *> id_list;
    Environment::SymbolTableEntry * callable = NULL;
    std::queue<Environment::TOKEN_TYPE> param_list;
    Environment::TOKEN_TYPE used_type;
    Environment::TOKEN_TYPE index_type;
    std::string assign_lexeme;

    Environment::SymbolTable * table = new Environment::SymbolTable();
}

%token PROGRAM VAR ARRAY OF INTEGER REAL FUNCTION PROCEDURE
%token IBEGIN IEND IF THEN ELSE WHILE DO WRITE WRITELN READ NOT COMMENT
%token <str> ID
%token <inum> INT_NUM
%token <rnum> REAL_NUM

%type <token_type> standard_type expression simple_expression term factor
%type <symtabentry> variable
%type <relop_type> relop
%type <sign> sign
%type <addop> addop
%type <mulop> mulop

%destructor { delete ($$); } <str>

%printer { std::cerr << *$$; } <str>
%printer { std::cerr << $$; } <inum>
%printer { std::cerr << $$; } <rnum>

%token RANGE_OP ".."
%token NOT_EQUAL "<>"
%token LESS_THAN_EQUAL "<="
%token GREATER_THAN_EQUAL ">="
%token ASSIGN_OP ":="
%token END 0 "EOF"

%right ":="
%left '=' "<>" '<' "<=" ">=" '>'
%left '+' '-' OR
%left '*' '/' DIV MOD AND

%%

%start program;

program:
    PROGRAM ID
        {
            table->Insert(std::string(*$2) + std::string("r"))->SetIdentifierType(Environment::RETURN_ID);
        }
    '(' ')' ';'
        {
            output_code->Mark(); // Mark the increment to increment over the variables to be declared.
            output_code->Push(new Generator::Instruction("int", 0, 3));
        }
    declarations
        {
            /**
             * HACK If this works then I need to fix offsets if I get time ...
             * Not sure if this contributes or allays the return passing problems ...
             */
            table->Count();
        }
    subprogram_declarations
        {
            (*output_code)[output_code->GetMark()].SetAddress(table->Count()); // Backpatch
            #ifndef NDEBUG
            DEBUG(*table);
            #endif
        }
    compound_statement '.' END
        {
            /**
             * Print a newline after finishing so our prompt comes back the way we want.
             */
            output_code->Push(new Generator::Instruction("lit", 0, '\n'));
            output_code->Push(new Generator::Instruction("opr", 0, 17));
            output_code->Push(new Generator::Instruction("opr", 0, 0));
        }
    ;

declarations:
    declarations VAR identifier_list ':' type ';'
    | /* empty */
    ;

identifier_list:
    id
    | identifier_list ',' id
    ;

id:
    ID
    {
        int level;
        if (table->Find(*$1, level) && level < 1)
        <%
            yyerror(filename, 0, "redeclaration of '" + *$1 + "'");
            YYERROR;
        %>
        id_list.push_back(table->Insert(*$1));
    }
    ;

type:
    standard_type
        {
            using namespace Environment;
            using namespace std;

            /**
             * Set types of identifiers.
             */
            for (list<SymbolTableEntry *>::iterator i = id_list.begin(); i != id_list.end(); ++i)
            <%
                (*i)->SetType($1);
                if (callable) callable->AddParameter(**i);
            %>
            id_list.clear();
        }
    | ARRAY '[' sign INT_NUM ".." sign INT_NUM ']' OF standard_type
        {
            /** Check that the lower bound is less than the upper bound. */
            int lower = ($3 == UMINUS) ? -$4 : $4;
            int upper = ($6 == UMINUS) ? -$7 : $7;
            if (lower > upper)
            <%
                yyerror(filename, 0, "lower bound '" + boost::lexical_cast<std::string>(lower) + "' is larger than upper bound '" + boost::lexical_cast<std::string>(upper) + "'");
                YYERROR;
            %>
            /** Set type of identifiers. */
            for (std::list<Environment::SymbolTableEntry *>::iterator i = id_list.begin(); i != id_list.end(); ++i)
            <%
                (*i)->SetType($10);
                (*i)->SetIdentifierType(Environment::ARRAY_ID);
                (*i)->SetBounds(lower, upper);
                if (callable) callable->AddParameter(**i);
            %>
            id_list.clear();
        }
    ;

standard_type:
    INTEGER { $$ = Environment::INTEGER; }
    | REAL { $$ = Environment::REAL; }
    ;

subprogram_declarations:
    subprogram_declarations subprogram_declaration ';'
    | /* empty */
    ;

subprogram_declaration:
    subprogram_head
        {
            output_code->Mark(); // Mark the jump over the function body.
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
            output_code->Mark(); // Mark the increment.
            output_code->Push(new Generator::Instruction("int", 0, 3));
        }
    declarations subprogram_declarations
        {
            (*output_code)[output_code->GetMark()].SetAddress(table->Count()); // Backpatch the increment.
            #ifndef NDEBUG
            DEBUG(*table);
            #endif
        }
    compound_statement
        {
            output_code->Push(new Generator::Instruction("opr", 0, 0)); // Return
            (*output_code)[output_code->GetMark()].SetAddress(output_code->TopAddress()); // Backpatch the jump over the function.
            table->Pop();
        }
    ;

subprogram_head:
    FUNCTION call_id arguments ':' standard_type ';'
        {
            callable->SetType($5);
            callable->SetIdentifierType(Environment::FUNC_ID);
            callable->SetAddress(output_code->TopAddress() + 1);
            callable = NULL;
        }
    | PROCEDURE call_id arguments ';'
        {
            callable->SetIdentifierType(Environment::PROC_ID);
            callable->SetAddress(output_code->TopAddress() + 1);
            callable = NULL;
        }
    ;

call_id:
    ID
        {
            int level;
            if (table->Find(*$1, level) && level < 1)
            <%
                yyerror(filename, 0, "redeclaration of '" + *$1 + "'");
                YYERROR;
            %>
            callable = table->Insert(*$1);
            table->Push();
            table->Insert(std::string(*$1) + std::string("r"))->SetIdentifierType(Environment::RETURN_ID);
        }
    ;

arguments:
    '(' parameter_list ')'
    | /* empty */
    ;

parameter_list:
    identifier_list ':' type
    | parameter_list ';' identifier_list ':' type
    ;

compound_statement:
    IBEGIN optional_statements IEND
    ;

optional_statements:
    statement_list
    | /* empty */
    ;

statement_list:
    statement
    | statement_list ';' statement
    ;

statement:
    variable ":="
        {
            int level;
            if (!$1) // Check that the variable exists.
            <%
                yyerror(filename, 0, std::string("'") + assign_lexeme + std::string("' was not declared in this scope"));
                YYERROR;
            %>
            #ifndef NDEBUG
            DEBUG(table->Find(*$1, level));
            DEBUG($1);
            #endif
            table->Find(*$1, level);

            #ifndef NDEBUG
            DEBUG($1->GetIdentifierType() & Environment::FUNC_ID);
            DEBUG(Environment::FUNC_ID);
            #endif

            if (($1->GetIdentifierType() & Environment::FUNC_ID) == Environment::FUNC_ID && !table->Find(std::string($1->GetLexeme()) + std::string("r"), level)) // Don't assign to another function's return value.
            <%
                yyerror(filename, 0, "Can only assign a return value to this function!  Not another function!");
                YYERROR;
            %>

            #ifndef NDEBUG
            DEBUG($1);
            DEBUG($1->GetIdentifierType());
            DEBUG(Environment::ARRAY_ID);
            #endif
            if (($1->GetIdentifierType() & Environment::ARRAY_ID) == Environment::ARRAY_ID)
            <%
                /** @note Check that we have an int index ... */
                if (index_type != Environment::INTEGER)
                <%
                    yyerror(filename, 0, std::string("invalid types '[") + boost::lexical_cast<std::string>($1->GetLowerBound()) + std::string("..") + boost::lexical_cast<std::string>($1->GetUpperBound()) + std::string("] [") + boost::lexical_cast<std::string>(index_type) + std::string("] of ") + boost::lexical_cast<std::string>($1->GetType()) + std::string("' for array subscript"));
                    YYERROR;
                %>

                array_index_code_generator($1, output_code);
            %>
        }
    expression
        {
            int level;
            table->Find(*$1, level);

            if (($1->GetType() & $4) != $4) // Coerce $4
                switch ($1->GetType())
                {
                    case Environment::REAL:
                        output_code->Push(new Generator::Instruction("opr", 0, 20));
                        break;
                    case Environment::INTEGER:
                        output_code->Push(new Generator::Instruction("opr", 0, 52));
                        break;
                };


            #ifndef NDEBUG
            DEBUG($1->GetLexeme());
            DEBUG($1->GetIdentifierType());
            DEBUG(level);
            DEBUG($1->GetOffSet());
            DEBUG(table);
            DEBUG((($1->GetIdentifierType() & Environment::FUNC_ID) == Environment::FUNC_ID) ? level : 0);
            #endif

            /** Interestingly numbers get messed up at lower levels ... or ... not ... WTF! */
            if (($1->GetIdentifierType() & Environment::ARRAY_ID) != Environment::ARRAY_ID)
                output_code->Push(new Generator::Instruction("sto", level, $1->GetOffSet() - ((($1->GetIdentifierType() & Environment::FUNC_ID) == Environment::FUNC_ID) ? level : 0)));
            else
                output_code->Push(new Generator::Instruction("sar", level, $1->GetOffSet()));
        }
    | procedure_statement
    | compound_statement
    | ifthen statement ELSE
        {
            int addr = output_code->GetMark();
            output_code->Mark();
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
            (*output_code)[addr].SetFunction("jpc");
            (*output_code)[addr].SetAddress(output_code->TopAddress());
        }
    statement
        {
            (*output_code)[output_code->GetMark()].SetAddress(output_code->TopAddress());
        }
    | ifthen statement
        {
            int addr = output_code->GetMark();
            (*output_code)[addr].SetFunction("jpc");
            (*output_code)[addr].SetAddress(output_code->TopAddress());
        }
    | WHILE
        {
            #ifndef NDEBUG
            DEBUG(output_code->TopAddress());
            #endif
            output_code->Mark();
        }
    expression DO
        {
            #ifndef NDEBUG
            DEBUG(output_code->TopAddress());
            #endif
            output_code->Mark();
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
        }
    statement
        {
            int addr = output_code->GetMark();
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->GetMark()));
            (*output_code)[addr].SetFunction("jpc");
            (*output_code)[addr].SetAddress(output_code->TopAddress());
        }
    | WRITE expression
        {
            switch ($2)
            {
                case Environment::REAL:
                    output_code->Push(new Generator::Instruction("opr", 0, 47));
                    break;
                case Environment::INTEGER:
                    output_code->Push(new Generator::Instruction("opr", 0, 15));
                    break;
            };
        }
    | WRITELN expression
        {
            switch ($2)
            {
                case Environment::REAL:
                    output_code->Push(new Generator::Instruction("opr", 0, 47));
                    break;
                case Environment::INTEGER:
                    output_code->Push(new Generator::Instruction("opr", 0, 15));
                    break;
            };
            output_code->Push(new Generator::Instruction("lit", 0, static_cast<int>('\n')));
            output_code->Push(new Generator::Instruction("opr", 0, 17));
        }
    | READ variable
        {
            int level;
            if (!$2) // Check that the variable exists.
            <%
                yyerror(filename, 0, std::string("'") + assign_lexeme + std::string("' was not declared in this scope"));
                YYERROR;
            %>
            table->Find(*$2, level);

            switch ($2->GetType())
            {
                case Environment::REAL:
                    output_code->Push(new Generator::Instruction("opr", 0, 46));
                    break;
                case Environment::INTEGER:
                    output_code->Push(new Generator::Instruction("opr", 0, 14));
                    break;
            };

            if ($2->GetIdentifierType() & Environment::FUNC_ID == Environment::FUNC_ID && !table->Find(std::string($2->GetLexeme()) + std::string("r"), level))
            <%
                yyerror(filename, 0, "Cannot read into a return value for a function!");
                YYERROR;
            %>

            if (($2->GetIdentifierType() & Environment::ARRAY_ID) != Environment::ARRAY_ID)
            <%
            #ifndef NDEBUG
                DEBUG($2->GetOffSet());
            #endif
                output_code->Push(new Generator::Instruction("sto", level, $2->GetOffSet()));
            %>
            else
            <%
                /** @note Check that we have an int index ... */
                if (index_type != Environment::INTEGER)
                <%
                    yyerror(filename, 0, std::string("invalid types '[") + boost::lexical_cast<std::string>($2->GetLowerBound()) + std::string("..") + boost::lexical_cast<std::string>($2->GetUpperBound()) + std::string("] [") + boost::lexical_cast<std::string>(index_type) + std::string("] of ") + boost::lexical_cast<std::string>($2->GetType()) + std::string("' for array subscript"));
                    YYERROR;
                %>

                array_index_code_generator($2, output_code);

                /** @note Finally, we can load the value in the array ... wow */
                output_code->Push(new Generator::Instruction("sar", level, $2->GetOffSet()));
            %>
        }
    ;

ifthen:
    IF expression THEN
        {
            output_code->Mark();
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
        }
    ;

variable:
    ID
        {
            int level;
            $$ = table->Find(*$1, level);
            assign_lexeme = *$1;
        }
    | ID '[' expression ']'
        {
            int level;
            $$ = table->Find(*$1, level);
            index_type = $3;
            assign_lexeme = *$1;
        }
    ;

procedure_statement:
    ID
        {
            int level;
            Environment::SymbolTableEntry * entry = table->Find(*$1, level);

            if (!entry) // Check that the variable exists.
            <%
                yyerror(filename, 0, std::string("'") + *$1 + std::string("' was not declared in this scope"));
                YYERROR;
            %>

            if ((entry->GetIdentifierType() & Environment::PROC_ID) != Environment::PROC_ID)
            <%
                yyerror(filename, 0, std::string("'") + entry->GetLexeme() + std::string("' cannot be used as a procedure"));
                YYERROR;
            %>

            output_code->Push(new Generator::Instruction("cal", level, entry->GetAddress()));
        }
    | ID '(' expression_list ')'
        {
            int level;
            Environment::SymbolTableEntry * entry = table->Find(*$1, level);

            if (!entry) // Check that the variable exists.
            <%
                yyerror(filename, 0, std::string("'") + *$1 + std::string("' was not declared in this scope"));
                YYERROR;
            %>

            if ((entry->GetIdentifierType() & Environment::PROC_ID) != Environment::PROC_ID)
            <%
                yyerror(filename, 0, std::string("'") + entry->GetLexeme() + std::string("' cannot be used as a procedure"));
                YYERROR;
            %>

            /** Parameter list handling. */
            std::queue<Environment::TOKEN_TYPE> tmp = entry->GetParameterList();

            int nparams = param_list.size();
            if (tmp.size() != param_list.size())
            <%
                yyerror(filename, 0, std::string("expected '") + boost::lexical_cast<std::string>(tmp.size()) + std::string("' parameters but '") + boost::lexical_cast<std::string>(param_list.size()) + std::string("' parameters were passed"));
                YYERROR;
            %>
            for (; !param_list.empty() && !tmp.empty(); param_list.pop(), tmp.pop())
            <%
            #ifndef NDEBUG
                DEBUG(Environment::REAL);
                DEBUG(tmp.front());
                DEBUG(param_list.front());
            #endif
                if (tmp.front() != param_list.front())
                <%
                    yyerror(filename, 0, "cannot convert a parameter's type to the necessary type");
                    /** @todo Make this recover a little better and say more ... */
                    YYERROR;
                %>
            %>

            output_code->Push(new Generator::Instruction("pas", level, nparams));
            output_code->Push(new Generator::Instruction("cal", level, entry->GetAddress()));
        }
    ;

expression_list:
    expression { param_list.push($1); }
    | expression_list ',' expression { param_list.push($3); }
    ;

expression:
    simple_expression
    | simple_expression
        {
            output_code->Mark();
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
            used_type = $1;
        }
    relop simple_expression
        {
            /** Coercion of types. */
            if ($1 == Environment::REAL && $4 != Environment::REAL) // Coerce $4 to REAL.
            <%
                output_code->GetMark();
                output_code->Push(new Generator::Instruction("opr", 0, 20));
            %>
            else if ($4 == Environment::REAL && $1 != Environment::REAL) // Coerce $1 to REAL.
            <%
                int addr = output_code->GetMark();
                (*output_code)[addr].SetFunction("opr");
                (*output_code)[addr].SetAddress(20);
                used_type = $4;
            %>
            else output_code->GetMark();

            switch (used_type)
            {
                case Environment::REAL:
                    switch ($3)
                    {
                        case EQ:
                            output_code->Push(new Generator::Instruction("opr", 0, 40));
                            break;
                        case NE:
                            output_code->Push(new Generator::Instruction("opr", 0, 41));
                            break;
                        case LT:
                            output_code->Push(new Generator::Instruction("opr", 0, 42));
                            break;
                        case GT:
                            output_code->Push(new Generator::Instruction("opr", 0, 44));
                            break;
                        case LE:
                            output_code->Push(new Generator::Instruction("opr", 0, 45));
                            break;
                        case GE:
                            output_code->Push(new Generator::Instruction("opr", 0, 43));
                            break;
                    };
                    break;
                case Environment::INTEGER:
                    switch ($3)
                    {
                        case EQ:
                            output_code->Push(new Generator::Instruction("opr", 0, 8));
                            break;
                        case NE:
                            output_code->Push(new Generator::Instruction("opr", 0, 9));
                            break;
                        case LT:
                            output_code->Push(new Generator::Instruction("opr", 0, 10));
                            break;
                        case GT:
                            output_code->Push(new Generator::Instruction("opr", 0, 12));
                            break;
                        case LE:
                            output_code->Push(new Generator::Instruction("opr", 0, 13));
                            break;
                        case GE:
                            output_code->Push(new Generator::Instruction("opr", 0, 11));
                            break;
                    };
                    break;
            };
            $$ = used_type;
        }
    ;

simple_expression:
    sign term
        {
            $$ = $2;
            if ($1 == UMINUS) output_code->Push(new Generator::Instruction("opr", 0, 1));
        }
    | simple_expression
        {
            output_code->Mark();
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
            used_type = $1;
        }
    addop term
        {
            /** Coercion of types. */
            Environment::TOKEN_TYPE used_type = $1;
            if ($1 == Environment::REAL && $4 != Environment::REAL) // Coerce $4 to REAL.
            {
                output_code->GetMark();
                output_code->Push(new Generator::Instruction("opr", 0, 20));
            }
            else if ($4 == Environment::REAL && $1 != Environment::REAL) // Coerce $1 to REAL.
            {
                int addr = output_code->GetMark();
                (*output_code)[addr].SetFunction("opr");
                (*output_code)[addr].SetAddress(20);
                used_type = $4;
            }
            else output_code->GetMark();

            switch (used_type)
            {
                case Environment::REAL:
                    switch ($3)
                    {
                        case BPLUS:
                            output_code->Push(new Generator::Instruction("opr", 0, 34));
                            break;
                        case BMINUS:
                            output_code->Push(new Generator::Instruction("opr", 0, 35));
                            break;
                        case BOR:
                            output_code->Push(new Generator::Instruction("opr", 0, 34));
                            break;
                    };
                    break;
                case Environment::INTEGER:
                    switch ($3)
                    {
                        case BPLUS:
                            output_code->Push(new Generator::Instruction("opr", 0, 2));
                            break;
                        case BMINUS:
                            output_code->Push(new Generator::Instruction("opr", 0, 3));
                            break;
                        case BOR:
                            output_code->Push(new Generator::Instruction("opr", 0, 2));
                            break;
                    };
                    break;
            };
            $$ = used_type;
        }
    ;

term:
    factor
    | term
        {
            output_code->Mark();
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
            used_type = $1;
        }
    mulop
        {
            if ($3 == BMOD) output_code->Push(new Generator::Instruction("opr", 0, 7));
        }
    factor
        {
            /** Coercion of types. */
            if ($1 == Environment::REAL && $5 != Environment::REAL) // Coerce $5 to REAL
            {
                output_code->GetMark();
                output_code->Push(new Generator::Instruction("opr", 0, 20));
            }
            else if ($5 == Environment::REAL && $1 != Environment::REAL) // Coerce $1 to REAL
            {
                int addr = output_code->GetMark();
                (*output_code)[addr].SetFunction("opr");
                (*output_code)[addr].SetAddress(20);
                used_type = $5;
            }
            else output_code->GetMark();

            switch (used_type)
            {
                case Environment::REAL:
                    switch ($3)
                    {
                        case BMULTIPLY:
                            output_code->Push(new Generator::Instruction("opr", 0, 36));
                            break;
                        case BDIVIDE:
                            output_code->Push(new Generator::Instruction("opr", 0, 37));
                            break;
                        case BDIV:
                            output_code->Push(new Generator::Instruction("opr", 0, 37));
                            output_code->Push(new Generator::Instruction("opr", 0, 52));
                            used_type = Environment::INTEGER;
                            break;
                        case BMOD:
                            output_code->Push(new Generator::Instruction("opr", 0, 37));
                            output_code->Push(new Generator::Instruction("opr", 0, 52));
                            output_code->Push(new Generator::Instruction("opr", 0, 20));
                            output_code->Push(new Generator::Instruction("int", 0, 1));
                            output_code->Push(new Generator::Instruction("opr", 0, 36));
                            output_code->Push(new Generator::Instruction("opr", 0, 35));
                            break;
                        case BAND:
                            output_code->Push(new Generator::Instruction("opr", 0, 36));
                            output_code->Push(new Generator::Instruction("opr", 0, 52));
                            used_type = Environment::INTEGER;
                            break;
                    };
                    break;
                case Environment::INTEGER:
                    switch ($3)
                    {
                        case BMULTIPLY:
                            output_code->Push(new Generator::Instruction("opr", 0, 4));
                            break;
                        case BDIVIDE:
                            output_code->Push(new Generator::Instruction("opr", 0, 5));
                            break;
                        case BDIV:
                            output_code->Push(new Generator::Instruction("opr", 0, 5));
                            break;
                        case BMOD:
                            output_code->Push(new Generator::Instruction("opr", 0, 5));
                            output_code->Push(new Generator::Instruction("int", 0, 1));
                            output_code->Push(new Generator::Instruction("opr", 0, 4));
                            output_code->Push(new Generator::Instruction("opr", 0, 3));
                            break;
                        case BAND:
                            output_code->Push(new Generator::Instruction("opr", 0, 4));
                            break;
                    };
                    break;
            };
            $$ = used_type;
        }
    ;

factor:
    ID
        {
            int level;
            Environment::SymbolTableEntry * entry = table->Find(*$1, level);

            if (!entry)
            <%
                yyerror(filename, 0, std::string("'") + *$1 + std::string("' was not declared in this scope"));
                YYERROR;
            %>

            $$ = entry->GetType();

            output_code->Push(new Generator::Instruction("lod", level, entry->GetOffSet()));
        }
    | ID '[' expression ']'
        {
            int level;
            Environment::SymbolTableEntry * entry = table->Find(*$1, level);

            if (!entry) // Check that the variable exists.
            <%
                yyerror(filename, 0, std::string("'") + *$1 + std::string("' was not declared in this scope"));
                YYERROR;
            %>

            $$ = entry->GetType();

            if ((entry->GetIdentifierType() & Environment::ARRAY_ID) != Environment::ARRAY_ID)
            <%
                yyerror(filename, 0, std::string("'") + entry->GetLexeme() + std::string("' was not declared as an array in this scope"));
                YYERROR;
            %>
            else
            <%
                /** @note Check that we have an int index ... */
                if ($3 != Environment::INTEGER)
                <%
                    yyerror(filename, 0, std::string("invalid types '[") + boost::lexical_cast<std::string>(entry->GetLowerBound()) + std::string("..") + boost::lexical_cast<std::string>(entry->GetUpperBound()) + std::string("] [") + boost::lexical_cast<std::string>($3) + std::string("] of ") + boost::lexical_cast<std::string>(entry->GetType()) + std::string("' for array subscript"));
                    YYERROR;
                %>

                array_index_code_generator(entry, output_code);

                /** @note Finally, we can load the value in the array ... wow */
                output_code->Push(new Generator::Instruction("lar", level, entry->GetOffSet()));
            %>
        }
    | ID '(' expression_list ')'
        {
            int level;
            Environment::SymbolTableEntry * entry = table->Find(*$1, level);

            if (!entry) // Check that the variable exists.
            <%
                yyerror(filename, 0, std::string("'") + *$1 + std::string("' was not declared in this scope"));
                YYERROR;
            %>

            if ((entry->GetIdentifierType() & Environment::FUNC_ID) != Environment::FUNC_ID)
            <%
                yyerror(filename, 0, std::string("'") + entry->GetLexeme() + std::string("' cannot be used as a function"));
                YYERROR;
            %>

            /** Parameter list handling. */
            std::queue<Environment::TOKEN_TYPE> tmp = entry->GetParameterList();

            int nparams = param_list.size();
            if (tmp.size() != param_list.size())
            <%
                yyerror(filename, 0, std::string("expected '") + boost::lexical_cast<std::string>(tmp.size()) + std::string("' parameters but '") + boost::lexical_cast<std::string>(param_list.size()) + std::string("' parameters were passed"));
                YYERROR;
            %>
            for (; !param_list.empty() && !tmp.empty(); param_list.pop(), tmp.pop())
            <%
            #ifndef NDEBUG
                DEBUG(Environment::REAL);
                DEBUG(tmp.front());
                DEBUG(param_list.front());
            #endif
                if (tmp.front() != param_list.front())
                <%
                    yyerror(filename, 0, "cannot convert a parameter's type to the necessary type");
                    /** @todo Make this recover a little better and say more ... */
                    YYERROR;
                %>
            %>

            output_code->Push(new Generator::Instruction("pas", level, nparams));
            output_code->Push(new Generator::Instruction("cal", level, entry->GetAddress()));
            output_code->Push(new Generator::Instruction("lod", level, entry->GetOffSet()));

            $$ = entry->GetType();
        }
    | INT_NUM
        {
            $$ = Environment::INTEGER;
            output_code->Push(new Generator::Instruction("lit", 0, $1));
        }
    | REAL_NUM
        {
            $$ = Environment::REAL;
            output_code->Push(new Generator::Instruction("flt", 0, $1));
        }
    | '(' expression ')' { $$ = $2; }
    | NOT factor
        {
            $$ = Environment::INTEGER;
            /** Load a 0 and then do the boolean op: factor > 0 */
            if ($2 == Environment::REAL)
            <%
                output_code->Push(new Generator::Instruction("flt", 0, 0));
                output_code->Push(new Generator::Instruction("opr", 0, 44));
            %>
            else
            <%
                output_code->Push(new Generator::Instruction("lit", 0, 0));
                output_code->Push(new Generator::Instruction("opr", 0, 12));
            %>
            output_code->Mark();
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
            /** if factor > 0 then put 0 on the stack */
            output_code->Push(new Generator::Instruction("lit", 0, 0));
            int addr = output_code->GetMark();
            output_code->Mark();
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
            (*output_code)[addr].SetFunction("jpc");
            (*output_code)[addr].SetAddress(output_code->TopAddress());
            /** else put 1 on the stack */
            output_code->Push(new Generator::Instruction("lit", 0, 1));
            (*output_code)[output_code->GetMark()].SetAddress(output_code->TopAddress());
        }
    ;

sign:
    '+' { $$ = UPLUS; }
    | '-' { $$ = UMINUS; }
    | /* empty */ { $$ = UPLUS; }
    ;

relop:
    '=' { $$ = EQ; }
    | "<>" { $$ = NE; }
    | '<' { $$ = LT; }
    | "<=" { $$ = LE; }
    | ">=" { $$ = GE; }
    | '>' { $$ = GT; }
    ;

addop:
    '+' { $$ = BPLUS; }
    | '-' { $$ = BMINUS; }
    | OR { $$ = BOR; }
    ;

mulop:
    '*' { $$ = BMULTIPLY; }
    | '/' { $$ = BDIVIDE; }
    | DIV { $$ = BDIV; }
    | MOD { $$ = BMOD; }
    | AND { $$ = BAND; }
    ;

%%

void dirty_yyerror(const std::string & filename, const std::string & msg)
{
    using namespace boost;
    using namespace std;
    ERROR(filename + string(":") + lexical_cast<string>(yylineno) + string(": error: ") + msg);
}

void array_index_code_generator(Environment::SymbolTableEntry * entry, Generator::ProgramStore * output_code)
{
    /** @note Calculate the index because the machine expects start 1 but we allow anything ... */
    output_code->Push(new Generator::Instruction("lit", 0, entry->GetLowerBound())); // Push the lower bound.
    output_code->Push(new Generator::Instruction("opr", 0, 3)); // Difference the two. L - I = 0 based index.
    output_code->Push(new Generator::Instruction("opr", 0, 7)); // Duplicate the top (tmp storage).
    output_code->Push(new Generator::Instruction("lit", 0, 0)); // See if the index is negative.
    output_code->Push(new Generator::Instruction("opr", 0, 10)); // 0 < 0 based index.
    output_code->Mark();
    output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
    output_code->Push(new Generator::Instruction("opr", 0, 1));

    /** @note backpatch the jump over negation. */
    int addr = output_code->GetMark();
    (*output_code)[addr].SetFunction("jpc");
    (*output_code)[addr].SetAddress(output_code->TopAddress());
    output_code->Push(new Generator::Instruction("lit", 0, 1)); // Load a one to add to our index.
    output_code->Push(new Generator::Instruction("opr", 0, 2)); // Add the one so we have a 1 based index.

    /** @note bounds checking ... */
    output_code->Push(new Generator::Instruction("opr", 0, 7));
    output_code->Push(new Generator::Instruction("lit", 0, entry->GetSize()));
    output_code->Push(new Generator::Instruction("opr", 0, 12));
    output_code->Mark();
    output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
    std::string run_error_msg = "Index "; /* print expr */ // " out of bounds [" " .. " "]!"
    for (int i = 0; i < run_error_msg.length(); ++i)
    {
        output_code->Push(new Generator::Instruction("lit", 0, run_error_msg[i]));
        output_code->Push(new Generator::Instruction("opr", 0, 17));
    }
    output_code->Push(new Generator::Instruction("opr", 0, 15));
    run_error_msg = "out of bounds [" + boost::lexical_cast<std::string>(entry->GetLowerBound()) + ".." + boost::lexical_cast<std::string>(entry->GetUpperBound()) + "]!\n";
    for (int i = 0; i < run_error_msg.length(); ++i)
    {
        output_code->Push(new Generator::Instruction("lit", 0, run_error_msg[i]));
        output_code->Push(new Generator::Instruction("opr", 0, 17));
    }

    /** @todo Find a way to interrupt execution at this point ... */

    /** @note backpatch the jump over error handling. */
    addr = output_code->GetMark();
    (*output_code)[addr].SetFunction("jpc");
    (*output_code)[addr].SetAddress(output_code->TopAddress());
}
