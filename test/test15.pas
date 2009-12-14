{
  Checking scoping rules to see if we can access variables in the containing scope.
}
PROGRAM test15();
VAR bar : integer;

PROCEDURE test15a;
VAR baz : integer;
BEGIN
  bar := 1;
  baz := bar;
  write bar;
  writeln baz
END;

BEGIN
  writeln 1;
  test15a;
  writeln bar
END
.
