{
  Testing the multiplication operations.
  The production being tested:
    term: factor | sign term | term MULOP factor
}
PROGRAM test12();
VAR bar : integer;
BEGIN
  writeln 16;

  { The multiplication operators for floats. }
  IF 2. * 2. = 4. THEN WRITELN 1;
  IF 4. / 2. = 2. THEN WRITELN 2;
  IF 4.1 div 2. = 2 THEN WRITELN 3;
  IF 1. and 1. THEN WRITELN 4;
  IF 4.2935 mod 3.42 = 0.8735 THEN WRITELN 5;

  { The multiplication operators for ints. }
  IF 2 * 2 = 4 THEN WRITELN 6;
  IF 4 / 2 = 2 THEN WRITELN 7;
  IF 4 div 2 = 2 THEN WRITELN 8;
  IF 1 and 1 THEN WRITELN 9;
  IF 4 mod 2 = 0 THEN WRITELN 10;

  { Looking at the other side of AND }
  IF 0 and 1 THEN writeln -1 else WRITELN 11;
  IF 0 and 0 THEN writeln -1 else WRITELN 12;

  { Coercion both ways. }
  IF 2.0 * 2 = 4.0 THEN WRITELN 13;
  IF 2 * 2.0 = 4.0 THEN WRITELN 14;

  { Mix and match a little. }
  IF 2.25 * 2 / 2. mod 2 = .25 THEN WRITELN 15;
  IF 6.23e-3 / 2.0 * 4.22e2 = 1.31453 THEN WRITELN 16
END
.
