{
  Adding identifier lists to test production identifier_list:
    identifier_list: ID | identifier ',' ID
  Also, happens to test production declarations:
    declarations: declarations VAR identifier_list ':' type ';' | /* empty */
  Also, happens to test production standard_type:
    type: INTEGER | REAL
}
PROGRAM test02();
VAR bar : integer;
VAR baz, qux : real;
VAR quux, corge, grault : integer;
VAR garply, waldo, fred, plugh, xyzzy, thud : real;
BEGIN
  writeln 0
END
.
