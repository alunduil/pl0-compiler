{
  Test variable accesses and assignments.
  Testing the production variable:
    variable: ID | ID '[' expression ']'
  Also, testing the array accesses and assignments.
}
PROGRAM test07();
VAR i, j : integer;
VAR bar : array [ -10 .. 10 ] of real;
BEGIN
  writeln 20;

  j := 1;
  i := -10;
  WHILE i <> 10 DO
  BEGIN
    bar[i] := i*2.0;
    WRITE bar[i];
    writeln j;
    j := j + 1;
    i := i + 1
  END
END
.
