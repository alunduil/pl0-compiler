{The holy grail: fibonnaci sequence. }
PROGRAM test09();
var i : integer;

FUNCTION fib(n : integer) : integer;
BEGIN
  if n < 2 then fib:= 1
  else fib := fib(n - 2) + fib(n - 1) 
END;

BEGIN
  i := 0;
  while i < 5 do
  begin
    WRITE fib(i);
    i := i + 1
  end
END
.
