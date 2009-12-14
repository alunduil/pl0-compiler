%{
    #include <boost/lexical_cast.hpp>
    #include <iostream>

    #include "parser.hpp"

    #include <output.h>

    #define yyterminate() return END
%}

%option yylineno
%option noyywrap
%option nounput
%option batch
%option caseless

identifier          [[:alpha:]][[:alnum:]]*
digits              [[:digit:]]+
optional_fraction   (\.{digits})
optional_exponent   (E(\+|-)?{digits})
i_num               {digits}
r_num               ({digits}{optional_fraction}?{optional_exponent}?|{digits}?{optional_fraction}{optional_exponent}?|{digits}\.)

%x comment

%%

\{ { BEGIN(comment); }

<comment>\} { BEGIN(INITIAL); }

<comment>([^\n\}<<EOF>>]|e|o|f)+ { /* empty */ }

<*>[\n] { /* empty */ }

program { return PROGRAM; }

var { return VAR; }

array { return ARRAY; }

{i_num}/[^\.E] {
    yylval->inum = boost::lexical_cast<int>(yytext);
    return INT_NUM;
    }

{r_num} {
    yylval->rnum = boost::lexical_cast<float>(yytext);
    return REAL_NUM;
    }

\.\. { return RANGE_OP; }

of { return OF; }

integer { return INTEGER; }

real { return REAL; }

function { return FUNCTION; }

procedure { return PROCEDURE; }

begin { return IBEGIN; }

end { return IEND; }

:= { return ASSIGN_OP; }

if { return IF; }

then { return THEN; }

else { return ELSE; }

while { return WHILE; }

do { return DO; }

write { return WRITE; }

writeln { return WRITELN; }

read { return READ; }

\<\> { return NOT_EQUAL; }

\<= { return LESS_THAN_EQUAL; }

\>= { return GREATER_THAN_EQUAL; }

or { return OR; }

div { return DIV; }

mod { return MOD; }

and { return AND; }

not { return NOT; }

[[:blank:]] { /* empty */ }

{identifier} {
    yylval->str = new std::string(yytext);
    return ID;
    }

. { return yytext[0]; }
