{
  Testing procedure calls, but not function calls.
  Test production:
    procedure_statement: id
}
PROGRAM test08();

PROCEDURE test08a;
VAR bar : real;
BEGIN
  bar := 1;
  WRITELN bar
END;

PROCEDURE test08b(baz : real);
BEGIN
  WRITELN baz
END;

PROCEDURE test08c(baz : integer);
BEGIN
  writeln baz
end;

BEGIN
  writeln 3;
  test08a;
  test08b(2.);
  test08c(3)
END
.
