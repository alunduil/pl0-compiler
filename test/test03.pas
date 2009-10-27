program
{Checking the assignment variable}
{CONST should not be assignable, VAR should, PROCEDURE should not.}
const foo = 27;
var bar;
procedure fred;
    begin
    end
;
begin
    bar := 42;
    foo := 32;
    fred := 52
end
.
