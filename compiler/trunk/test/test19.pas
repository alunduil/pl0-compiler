{
  Nested function declaration and call.
}
program test19();
var baz : integer;

procedure test19a(stop : integer);
begin
  if stop = 1 then writeln baz
  else
  begin
    writeln baz;
    baz := baz + 1;
    test19a(1)
  end
end;

begin
  writeln 2;

  baz := 1;
  test19a(0)
end
.

