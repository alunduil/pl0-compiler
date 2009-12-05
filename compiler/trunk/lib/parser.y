%language "C++"
%locations
%define namespace "Analyzer"
%debug
%error-verbose
%require "2.4.1"
/* Expect the s/r conflict for the if then else ambiguity. */
%expect 1

%code top {
    #include <iostream>
    #include <string>
    #include <boost/lexical_cast.hpp>

    #include <output.h>
}

%code requires {
    #define YY_DECL                                     \
        Analyzer::parser::token_type                    \
        yylex(Analyzer::parser::semantic_type * yylval, \
              Analyzer::parser::location_type * yylloc) \
}

%parse-param { std::string filename }

%union {
    int inum;
    float rnum;
    std::string * str;
}

%code {
    YY_DECL;
}

%initial-action {
    @$.begin.filename = @$.end.filename = &filename;
}

%token PROGRAM VAR ARRAY OF INTEGER REAL FUNCTION PROCEDURE
%token IBEGIN IEND IF THEN ELSE WHILE DO WRITE WRITELN READ NOT COMMENT
%token <str> ID
%token <inum> INT_NUM
%token <rnum> REAL_NUM

%destructor { delete ($$); } <str>

%printer { debug_stream() << *$$; } <str>
%printer { debug_stream() << $$; } <inum>
%printer { debug_stream() << $$; } <rnum>

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

program: PROGRAM ID '(' ')' ';' declarations subprogram_declarations compound_statement '.' {}
    ;

identifier_list: ID
    | identifier_list ',' ID {}
    ;

declarations: declarations VAR identifier_list ':' type ';'
    | /* empty */
    ;

type: standard_type
    | ARRAY '[' INT_NUM ".." INT_NUM ']' OF standard_type
    ;

standard_type: INTEGER
    | REAL
    ;

subprogram_declarations: subprogram_declarations subprogram_declaration ';'
    | /* empty */
    ;

subprogram_declaration: subprogram_head declarations subprogram_declarations compound_statement
    ;

subprogram_head: FUNCTION ID arguments ':' standard_type ';'
    | PROCEDURE ID arguments ';'
    ;

arguments: '(' parameter_list ')'
    | /* empty */
    ;

parameter_list: identifier_list ':' type
    | parameter_list ';' identifier_list : type
    ;

compound_statement: IBEGIN optional_statements IEND
    ;

optional_statements: statement_list
    | /* empty */
    ;

statement_list: statement
    | statement_list ';' statement
    ;

statement: variable ":=" expression
    | procedure_statement
    | compound_statement
    | IF expression THEN statement ELSE statement
    | IF expression THEN statement
    | WHILE expression DO statement
    | WRITE expression
    | WRITELN expression
    | READ variable
    ;

variable: ID
    | ID '[' expression ']'
    ;

procedure_statement: ID
    | ID '(' expression_list ')'
    ;

expression_list: expression
    | expression_list ',' expression
    ;

expression: simple_expression
    | simple_expression relop simple_expression
    ;

simple_expression: term
    | sign term
    | simple_expression addop term
    ;

term: factor
    | term mulop factor
    ;

factor: ID
    | ID '(' expression_list ')'
    | INT_NUM
    | REAL_NUM
    | '(' expression ')'
    | NOT factor
    ;

sign: '+'
    | '-'
    ;

relop: '='
    | "<>"
    | '<'
    | "<="
    | ">="
    | '>'
    ;

addop: '+'
    | '-'
    | OR
    ;

mulop: '*'
    | '/'
    | DIV
    | MOD
    | AND
    ;

%%

void Analyzer::parser::error(const Analyzer::parser::location_type & l, const std::string & msg)
{
    using namespace boost;
    using namespace std;
    ERROR(filename + string(":") + lexical_cast<string>(l.lines) + string(": error: ") + msg);
}
