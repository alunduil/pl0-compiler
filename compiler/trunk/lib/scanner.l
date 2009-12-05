%{
    #include <boost/lexical_cast.hpp>

    #include "parser.hpp"

    #define yyterminate() return Analyzer::parser::token::END
%}

%option c++
%option yylineno
%option noyywrap
%option nounput
%option batch
%option debug
%option caseless

identifier          [[:alpha:]][[:alnum:]]*
digits              [[:digit:]]+
optional_fraction   (\.{digits})?
optional_exponent   (E(\+|-)?{digits})?
i_num               {digits}
r_num               {digits}{optional_fraction}{optional_exponent}

%{
    #define YY_USER_ACTION yylloc->columns(yyleng);
%}

%x comment

%%

%{
    yylloc->step();
%}

\{ {
    BEGIN(comment);
    }

<*>\n+ {
    yylloc->lines(yyleng);
    }

<comment>\} {
    BEGIN(INITIAL);
    }

<comment>[^\n\}<<EOF>>]+ { /* empty */
    }

[[:blank:]]+ { /* empty */
    }

program {
    return Analyzer::parser::token::PROGRAM;
    }

var {
    return Analyzer::parser::token::VAR;
    }

array {
    return Analyzer::parser::token::ARRAY;
    }

{i_num} {
    yylval.inum = boost::lexical_cast<int>(yytext);
    return Analyzer::parser::token::INT_NUM;
    }

{r_num} {
    yylval.rnum = boost::lexical_cast<float>(yytext);
    return Analyzer::parser::token::REAL_NUM;
    }

\.\. {
    return Analyzer::parser::token::RANGE_OP;
    }

of {
    return Analyzer::parser::token::OF;
    }

integer {
    return Analyzer::parser::token::INTEGER;
    }

real {
    return Analyzer::parser::token::REAL;
    }

function {
    return Analyzer::parser::token::FUNCTION;
    }

procedure {
    return Analyzer::parser::token::PROCEDURE;
    }

begin {
    return Analyzer::parser::token::IBEGIN;
    }

end {
    return Analyzer::parser::token::IEND;
    }

:= {
    return Analyzer::parser::token::ASSIGN_OP;
    }

if {
    return Analyzer::parser::token::IF;
    }

then {
    return Analyzer::parser::token::THEN;
    }

else {
    return Analyzer::parser::token::ELSE;
    }

while {
    return Analyzer::parser::token::WHILE;
    }

do {
    return Analyzer::parser::token::DO;
    }

write {
    return Analyzer::parser::token::WRITE;
    }

writeln {
    return Analyzer::parser::token::WRITELN;
    }

read {
    return Analyzer::parser::token::READ;
    }

\<\> {
    return Analyzer::parser::token::NOT_EQUAL;
    }

\<= {
    return Analyzer::parser::token::LESS_THAN_EQUAL;
    }

\>= {
    return Analyzer::parser::token::GREATER_THAN_EQUAL;
    }

or {
    return Analyzer::parser::token::OR;
    }

div {
    return Analyzer::parser::token::DIV;
    }

mod {
    return Analyzer::parser::token::MOD;
    }

and {
    return Analyzer::parser::token::AND;
    }

not {
    return Analyzer::parser::token::NOT;
    }

{identifier} {
    yylval.str = new std::string(yytext);
    return Analyzer::parser::token::ID;
    }
