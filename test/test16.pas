{
  Testing all uses of return values from functions ...
}
PROGRAM test16();
VAR bar : integer;

FUNCTION test16a(baz : integer) : integer;
VAR foo : integer;
BEGIN
  writeln bar;
  foo := baz;
  test16a := foo
END;

BEGIN
  writeln 2;

  bar := 1;
  writeln test16a(2)
END
.
