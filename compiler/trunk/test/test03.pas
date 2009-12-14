{
  Testing the type production:
    type: standard_type | ARRAY '[' NUM RANGE_OP NUM ']' OF standard_type
}
PROGRAM test03();
VAR bar, baz, qux : array [ -10 .. 10 ] of integer;
VAR quux, corge, grault : array [ 0 .. 20 ] of real;
BEGIN
  writeln 0
END
.
