{
  Testing some operator precedence.
  Looking at how mulop and addop interact ...
}
PROGRAM test14();
BEGIN
  writeln 7;

  IF (4 * 5) + 3 = 23 THEN WRITELN 1;
  IF 4 * 5 + 13 = 33 THEN WRITELN 2;
  IF 4 * ( 5 + 12 ) = 68 THEN WRITELN 3;
  IF 4 - 2 * 6 = -8 THEN WRITELN 4;
  IF 4 + 4 * 3 + 2 = 18 THEN WRITELN 5;
  IF (4 - 2 / 2) mod 2 = 1 then writeln 6;
  if (4 * 4 + 4) mod 4 = 0 then writeln 7
END
.
