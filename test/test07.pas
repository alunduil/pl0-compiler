program
{ function calls self and parent level's function }
const foo = -4938;
procedure bar;
const barney = 3024;
begin
    print barney;
    print foo;
end
;
procedure fred;
begin
    call bar;
end
;
call fred
.