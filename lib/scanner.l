%option c++
%option yylineno
%option noyywrap
%option caseless

%{

#include <token.h>
#include <symboltable.h>
#include <symboltableentry.h>

extern Environment::Token *yytoken;
extern Environment::SymbolTable *yysymtab;

%}

letter      [a-zA-Z]
digit       [0-9]
number      {digit}+
identifier  {letter}({letter}|{digit})*

%%

\{.*\} {
    yytoken = new Environment::Token(yytext, Environment::COMMENT);
    return Environment::COMMENT;
}

, {
    yytoken = new Environment::Token(yytext, Environment::COMMA);
    return Environment::COMMA;
}

{number} {
    yytoken = new Environment::Token(yytext, Environment::NUMBER);
    return Environment::NUMBER;
}

:= {
    yytoken = new Environment::Token(yytext, Environment::ASSIGNMENT);
    return Environment::ASSIGNMENT;
}

\+ {
    yytoken = new Environment::Token(yytext, Environment::PLUS);
    return Environment::PLUS;
}

- {
    yytoken = new Environment::Token(yytext, Environment::MINUS);
    return Environment::MINUS;
}

\* {
    yytoken = new Environment::Token(yytext, Environment::MULTIPLY);
    return Environment::MULTIPLY;
}

\/ {
    yytoken = new Environment::Token(yytext, Environment::DIVIDE);
    return Environment::DIVIDE;
}

= {
    yytoken = new Environment::Token(yytext, Environment::EQUALS);
    return Environment::EQUALS;
}

\( {
    yytoken = new Environment::Token(yytext, Environment::LEFTPAREN);
    return Environment::LEFTPAREN;
}

\) {
    yytoken = new Environment::Token(yytext, Environment::RIGHTPAREN);
    return Environment::RIGHTPAREN;
}

; {
    yytoken = new Environment::Token(yytext, Environment::SEMICOLON);
    return Environment::SEMICOLON;
}

\. {
    yytoken = new Environment::Token(yytext, Environment::DOT);
    return Environment::DOT;
}

\< {
    yytoken = new Environment::Token(yytext, Environment::LESSTHAN);
    return Environment::LESSTHAN;
}

\<\> {
    yytoken = new Environment::Token(yytext, Environment::NOTEQUAL);
    return Environment::NOTEQUAL;
}

\<= {
    yytoken = new Environment::Token(yytext, Environment::LESSTHANEQUAL);
    return Environment::LESSTHANEQUAL;
}

\> {
    yytoken = new Environment::Token(yytext, Environment::GREATERTHAN);
    return Environment::GREATERTHAN;
}

\>= {
    yytoken = new Environment::Token(yytext, Environment::GREATERTHANEQUAL);
    return Environment::GREATERTHANEQUAL;
}

program {
    yytoken = new Environment::Token(yytext, Environment::PROGRAM);
    return Environment::PROGRAM;
}

const {
    yytoken = new Environment::Token(yytext, Environment::CONST);
    return Environment::CONST;
}

var {
    yytoken = new Environment::Token(yytext, Environment::VAR);
    return Environment::VAR;
}

procedure {
    yytoken = new Environment::Token(yytext, Environment::PROCEDURE);
    return Environment::PROCEDURE;
}

begin {
    yytoken = new Environment::Token(yytext, Environment::IBEGIN);
    return Environment::IBEGIN;
}

print {
    yytoken = new Environment::Token(yytext, Environment::PRINT);
    return Environment::PRINT;
}

div {
    yytoken = new Environment::Token(yytext, Environment::DIV);
    return Environment::DIV;
}

mod {
    yytoken = new Environment::Token(yytext, Environment::MOD);
    return Environment::MOD;
}

end {
    yytoken = new Environment::Token(yytext, Environment::END);
    return Environment::END;
}

odd {
    yytoken = new Environment::Token(yytext, Environment::ODD);
    return Environment::ODD;
}

if {
    yytoken = new Environment::Token(yytext, Environment::IF);
    return Environment::IF;
}

then {
    yytoken = new Environment::Token(yytext, Environment::THEN);
    return Environment::THEN;
}

call {
    yytoken = new Environment::Token(yytext, Environment::CALL);
    return Environment::CALL;
}

read {
    yytoken = new Environment::Token(yytext, Environment::READ);
    return Environment::READ;
}

while {
    yytoken = new Environment::Token(yytext, Environment::WHILE);
    return Environment::WHILE;
}

do {
    yytoken = new Environment::Token(yytext, Environment::DO);
    return Environment::DO;
}

<<EOF>> {
    yytoken = new Environment::Token(yytext, Environment::EOFL);
    return Environment::EOFL;
}

{identifier} {
    int level = 0;
    Environment::SymbolTableEntry *entry = yysymtab->Find(yytext, level);
    yytoken = new Environment::Token(yytext, Environment::IDENTIFIER, entry, level);
    return Environment::IDENTIFIER;
}

%%
