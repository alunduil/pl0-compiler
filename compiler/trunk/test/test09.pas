{
  Test the expression list that gets passed to a function call ...
  The production:
    expression_list: expression | expression_list ',' expression
  Apparently, testing return values of functions as well ...
}
PROGRAM test09();

FUNCTION test09a(bar : integer) : integer;
BEGIN
  WRITE bar;
  test09a := bar
END;

FUNCTION test09b(bar : integer; baz : real) : real;
BEGIN
  WRITE bar;
  WRITE baz;
  test09b := baz
END;

BEGIN
  writeln 2;
  WRITELN test09a(1);
  WRITELN test09b(2, 2.)
END
.
