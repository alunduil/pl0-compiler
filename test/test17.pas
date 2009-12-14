{ function calls self and parent level's function }
program test17();
var numelements, i: integer;

function fib(n : integer) : integer;
begin
  if (n <= 2) then fib := 1
  else fib := fib(n - 2) + fib(n - 1);
  write fib(n)
end;

begin
  writeln 1;
  read numelements;

  i := 1;
  while i < numelements do
  begin
    write fib(i);
    i := i + 1
  end
end
.
