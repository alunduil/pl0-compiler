{
  Testing the various factors that can be utilized as the base of an expression.
  The production for a factor will be tested.
    factor: ID 
            | ID '(' expression_list ')' 
            | id '[' simple_expression ']' 
            | NUM 
            | '(' expression ')'
            | NOT factor
}
PROGRAM test13();
VAR bar : integer;
VAR baz : array [ 0 .. 1 ] of integer;

FUNCTION test13a(bar : integer) : integer;
BEGIN
  test13a := bar
END;

BEGIN
  writeln 6;

  bar := 1;
  baz[0] := 1;
  baz[1] := 0;
  IF bar THEN WRITELN 1;
  IF test13a(bar) THEN WRITELN 2;
  IF baz[0] THEN WRITELN 3;
  IF 1 THEN WRITELN 4;
  IF ( baz[0] or baz[1] ) and bar THEN WRITELN 5;
  IF NOT baz[1] THEN WRITELN 6
END
.
