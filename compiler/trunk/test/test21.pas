{Checking the assignment variable}
{VAR should be assignable, PROCEDURE and function should not.}
program test21();
var baz : integer;

procedure test21a;
begin
end;

function test21b : integer;
begin
end;

begin
    writeln 0;

    baz := 1;
    {test21a := 2;}
    test21b := 3
end
.
