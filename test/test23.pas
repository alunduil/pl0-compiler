{
  Passing an array to a function ...
}
program test23();
var i : array [ 0 .. 1 ] of integer;

function test23a(sum : array [ 0 .. 1 ] of integer) : integer;
begin
  writeln sum[0];
  writeln sum[1];
  test23a := sum[0] + sum[1]
end;

begin
  i[0] := 5;
  i[1] := 10;
  writeln test23a(i)
end
.
