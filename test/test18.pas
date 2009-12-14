{
  Nested function declaration and call.
}
program test18();
var baz : integer;

procedure test18a;

  procedure test18aa;
  begin
    baz := 1;
    writeln baz
  end;

begin
  test18aa;
  baz := 2;
  writeln baz
end;

begin
  writeln 2;

  test18a
end
.

