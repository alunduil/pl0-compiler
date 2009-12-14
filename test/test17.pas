{ function calls self and parent level's function }
program test17();

function fib(n : integer) : integer;
begin
  if (n <= 2) then fib := 1
  else fib := fib(n - 2) + fib(n - 1);
  write fib(n)
end;

begin
  writeln 1;

  write fib(5)
end
.
