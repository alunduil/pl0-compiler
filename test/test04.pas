{
  Test the subprogram declarations, but not the calling yet ...
  Test the production subprogram_declarations:
    subprogram_declarations: subprogram_declarations subprogram_declaration ';' | /* empty */
  Also, tests the subprogram_declaration by default:
    subprogram_declaration: subprogram_head declarations subprogram_declarations compound_statement
  Also, tests the subprogram_head:
    subprogram_head: FUNCTION ID arguments ':' standard_type ';' | PROCEDURE ID arguments ';'
  Also, tests the arguments production:
    arguments: '(' parameter_list ')' | /* empty */
  Also, tests the paramater_list production:
    parameter_list: identifier_list ':' type | parameter_list ';' identifier_list ':' type
}
PROGRAM test04();

FUNCTION test04a(bar, baz : integer) : integer;

FUNCTION test04aa : real;
BEGIN
END;

BEGIN
END;

PROCEDURE test04b(qux, quux: integer; plugh, xyzzy, thud: real; corge : integer);
BEGIN
END;

{
  The question of arrays being passed or returned is now in question:
    let's do it if there is time!
}
FUNCTION test04c(corge : array [ -5 .. 5 ] of integer) : integer;
BEGIN
END;

{
  Returning arrays is not part of the grammar.  Will not be checking that!
}
FUNCTION test04d(corge : array [ 0 .. 10 ] of real) : real;
BEGIN
END;

BEGIN
  writeln 0
END
.
