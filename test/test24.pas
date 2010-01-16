{ 
  Passing a function as a parameter 
}
PROGRAM test24();

FUNCTION test24a : integer;
BEGIN
  test24a := 5
END;

procedure test24b(foo : integer);
BEGIN
  writeln foo
END;

BEGIN
  writeln test24a;
  test24b(test24a)
END
.

