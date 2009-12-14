{
  Will throw a compiler error because of the bounds on the array to be allocated.
}
PROGRAM test20();
VAR bar : array [ 20 .. -10 ] of integer;
BEGIN
  WRITELN 0;
END
.
