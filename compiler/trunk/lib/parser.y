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

    Environment::SymbolTable *table = NULL;
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

%parse-param { Generator::ProgramStore * output_code }
/**
 * @note Possible BISON bug?  This should define YYLEX_PARAM right?
 */
%lex-param { YYSTYPE * yylval }

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
    /**
     * @note DIRTY DIRTY HACK FOR YYERROR ...
     */
    #define yyerror(output_code, msg) dirty_yyerror((output_code), (msg));
    void dirty_yyerror(Generator::ProgramStore *, const std::string &);

    std::list<Environment::SymbolTableEntry *> id_list;
    Environment::SymbolTableEntry * callable = NULL;
    std::queue<Environment::TOKEN_TYPE> param_list;
}

%initial-action {
    table = new Environment::SymbolTable();
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
            table->Insert(*$2)->SetIdentifierType(Environment::RETURN_ID);
        }
    '(' ')' ';'
        {
            output_code->Mark();
            output_code->Push(new Generator::Instruction("int", 0, 3));
        }
    declarations subprogram_declarations
        {
            (*output_code)[output_code->GetMark()].SetAddress(table->Count());
        }
    compound_statement '.'
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
            yyerror(output_code, *$1);
            YYERROR;
        %>
        id_list.push_back(table->Insert(*$1));
    }
    ;

type:
    standard_type
        {
            for (; !id_list.empty(); id_list.pop_back()) id_list.front()->SetType($1);
        }
    | ARRAY '[' INT_NUM ".." INT_NUM ']' OF standard_type
        {
            for (; !id_list.empty(); id_list.pop_back())
            <%
                id_list.front()->SetType($8);
                id_list.front()->SetIdentifierType(Environment::ARRAY_ID);
                id_list.front()->SetBounds($3, $5);
            %>
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
            output_code->Mark();
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
        }
    declarations subprogram_declarations compound_statement
        {
            output_code->Push(new Generator::Instruction("opr", 0, 0));
            (*output_code)[output_code->GetMark()].SetAddress(output_code->TopAddress());
            table->Pop();
        }
    ;

subprogram_head:
    FUNCTION call_id arguments ':' standard_type ';'
        {
            callable->SetType($5);
            callable = NULL;
        }
    | PROCEDURE call_id arguments ';'
        {
            callable = NULL;
        }
    ;

call_id:
    ID
        {
            int level;
            if (table->Find(*$1, level) && level < 1)
            <%
                yyerror(output_code, *$1);
                YYERROR;
            %>
            callable = table->Insert(*$1);
            callable->SetIdentifierType(Environment::FUNC_ID);
            table->Push();
            table->Insert(*$1)->SetIdentifierType(Environment::RETURN_ID);
        }
    ;

arguments:
    '(' parameter_list ')'
    | /* empty */
    ;

parameter_list:
    identifier_list
        {
            callable->SetParameterList(id_list);
        }
    ':' type
    | parameter_list ';' identifier_list
        {
            callable->AppendParameterList(id_list);
        }
    ':' type
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
    variable ":=" expression
        {
            int level;
            table->Find(*$1, level);
            if ($1->GetIdentifierType() & Environment::ARRAY_ID != Environment::ARRAY_ID)
                output_code->Push(new Generator::Instruction("sto", level, $1->GetOffSet()));
            else
                /** @note Array store in element here. */;
        }
    | procedure_statement
    | compound_statement
    | ifthen statement ELSE
        {
            int addr = output_code->GetMark();
            (*output_code)[addr].SetFunction("jpc");
            (*output_code)[addr].SetAddress(output_code->TopAddress());
            output_code->Mark();
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
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
            output_code->Mark();
        }
    expression DO
        {
            output_code->Mark();
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->TopAddress() + 1));
        }
    statement
        {
            int addr = output_code->GetMark();
            output_code->Push(new Generator::Instruction("jmp", 0, output_code->GetMark()));
            (*output_code)[addr].SetAddress(output_code->TopAddress());
        }
    | WRITE expression
        {
            switch ($2)
            {
                case Environment::REAL:
                    output_code->Push(new Generator::Instruction("opr", 0, /** @note floating point print code */ 0));
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
                    output_code->Push(new Generator::Instruction("opr", 0, /** @note floating point print code */ 0));
                    break;
                case Environment::INTEGER:
                    output_code->Push(new Generator::Instruction("opr", 0, 15));
                    break;
            };
            output_code->Push(new Generator::Instruction("opr", 0, /** @note code to print newline */ 0));
        }
    | READ variable
        {
            switch ($2->GetTokenType())
            {
                case Environment::REAL:
                    output_code->Push(new Generator::Instruction("opr", 0, /** @note floating point read code */ 0));
                    break;
                case Environment::INTEGER:
                    output_code->Push(new Generator::Instruction("opr", 0, 14));
                    break;
            };
            int level;
            table->Find(*$2, level);
            if ($2->GetIdentifierType() & Environment::ARRAY_ID != Environment::ARRAY_ID)
                output_code->Push(new Generator::Instruction("sto", level, $2->GetOffSet()));
            else
                /** @note Array store in element here. */;
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
        }
    | ID '[' expression ']'
        {
            int level;
            $$ = table->Find(*$1, level);
            /** @note Oh hell ... */
        }
    ;

procedure_statement:
    ID
        {
            int level;
            Environment::SymbolTableEntry * entry = table->Find(*$1, level);
            output_code->Push(new Generator::Instruction("cal", level, entry->GetAddress()));
        }
    | ID '(' expression_list ')'
        {
            int level;
            Environment::SymbolTableEntry * entry = table->Find(*$1, level);
            std::queue<Environment::SymbolTableEntry *> tmp(entry->GetParameterList());
            for (; !param_list.empty() && !tmp.empty(); param_list.pop(), tmp.pop())
                if (tmp.front()->GetTokenType() != param_list.front())
                <%
                    yyerror(output_code, "Wrong parameters passed!");
                    YYERROR;
                %>
            for (; !param_list.empty(); param_list.pop()); /* Clear the param list if it isn't already! */
            output_code->Push(new Generator::Instruction("int", 0, entry->GetParameterList().size())); /* Jump over parameters. */
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
        }
    relop simple_expression
        {
            /** Coercion of types. */
            Environment::TOKEN_TYPE used_type = $1;
            if ($1 == Environment::REAL && $4 != Environment::REAL)
                /** @note Coerce $3 to REAL. */
                ;
            else if ($4 == Environment::REAL && $1 != Environment::REAL)
                /** @note Coerce $1 to REAL. */
                used_type = $4;

            switch (used_type)
            {
                case Environment::REAL:
                    switch ($3)
                    {
                        case EQ:
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float eq oper. */ 0));
                            break;
                        case NE:
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float ne oper. */ 0));
                            break;
                        case LT:
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float lt oper. */ 0));
                            break;
                        case GT:
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float gt oper. */ 0));
                            break;
                        case LE:
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float le oper. */ 0));
                            break;
                        case GE:
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float ge oper. */ 0));
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
        }
    ;

simple_expression:
    term
    | sign term
        {
            $$ = $2;
            if ($1 == UMINUS) output_code->Push(new Generator::Instruction("opr", 0, 1));
        }
    | simple_expression
        {
            output_code->Mark();
        }
    addop term
        {
            /** Coercion of types. */
            Environment::TOKEN_TYPE used_type = $1;
            if ($1 == Environment::REAL && $4 != Environment::REAL)
                /** @note Coerce $3 to REAL. */
                ;
            else if ($4 == Environment::REAL && $1 != Environment::REAL)
                /** @note Coerce $1 to REAL. */
                used_type = $4;

            switch (used_type)
            {
                case Environment::REAL:
                    switch ($3)
                    {
                        case BPLUS:
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float plus oper. */ 0));
                            break;
                        case BMINUS:
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float minus oper. */ 0));
                            break;
                        case BOR:
                            /** @note coerce it all to int. */
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float or oper. */ 0));
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
        }
    mulop
        {
            if ($3 == BMOD) output_code->Push(new Generator::Instruction("opr", 0, 7));
        }
    factor
        {
            /** Coercion of types. */
            Environment::TOKEN_TYPE used_type = $1;
            if ($1 == Environment::REAL && $5 != Environment::REAL)
                /** @note Coerce $3 to REAL. */
                ;
            else if ($5 == Environment::REAL && $1 != Environment::REAL)
                /** @note Coerce $1 to REAL. */
                used_type = $5;

            switch (used_type)
            {
                case Environment::REAL:
                    switch ($3)
                    {
                        case BMULTIPLY:
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float multiply oper. */ 0));
                            break;
                        case BDIVIDE:
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float divide oper. */ 0));
                            break;
                        case BDIV:
                            /** @note coerce both to integer. */
                            output_code->Push(new Generator::Instruction("opr", 0, 5));
                            break;
                        case BMOD:
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float oper. */ 5));
                            output_code->Push(new Generator::Instruction("int", 0, 1));
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float oper. */ 4));
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float oper. */ 3));
                            break;
                        case BAND:
                            /** @note coerce both to integer. */
                            output_code->Push(new Generator::Instruction("opr", 0, /** @note float multiply oper. */ 0));
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
            $$ = table->Find(*$1, level)->GetTokenType();
            /** Load the ID */
        }
    | ID '(' expression_list ')'
        {
            int level;
            Environment::SymbolTableEntry * entry = table->Find(*$1, level);
            $<token_type>$ = entry->GetTokenType();
            /** Call the ID with the following parameters. */
        }
    | INT_NUM { $$ = Environment::INTEGER; }
    | REAL_NUM { $$ = Environment::REAL; }
    | '(' expression ')' { $$ = $2; }
    | NOT factor { $$ = $2; }
    ;

sign:
    '+' { $$ = UPLUS; }
    | '-' { $$ = UMINUS; }
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

void dirty_yyerror(Generator::ProgramStore * output_code, const std::string & msg)
{
    using namespace boost;
    using namespace std;
    ERROR(/*lexical_cast<string>(l.first_line) + string(":") + lexical_cast<string>(l.last_line) + */string(": error: ") + msg);
}
