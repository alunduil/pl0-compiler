{
  Testing the relational operations and in the process the following production:
    expression: simple_expression | simple_expression RELOP simple_expression
}
PROGRAM test10();
BEGIN
  writeln 21;

  { Testing int operations }
  IF 1 = 1 THEN WRITELN 1;
  IF 1 <> 2 THEN WRITELN 2;
  IF 1 < 2 THEN WRITELN 3;
  IF 2 > 1 THEN WRITELN 4;
  IF 1 <= 1 THEN WRITELN 5;
  IF 1 >= 1 THEN WRITELN 6;

  { Testing float operations }
  IF 1.0 = 1.0 THEN WRITELN 7;
  IF 1.0 <> 1.01 THEN WRITELN 8;
  IF 1.0 < 1.1 THEN WRITELN 9;
  IF 1.0 > 0.9 THEN WRITELN 10;
  IF -1.0 <= 1.0 THEN WRITELN 11;
  IF 1.0 >= 1.0 THEN WRITELN 12;

  { Testing the opposite results }
  IF 1 = 2 THEN writeln 0 else WRITELN 13;
  IF 1 <> 1 THEN writeln 0 else WRITELN 14;
  IF 1 < 1 THEN writeln 0 else WRITELN 15;
  IF 1 > 1 THEN writeln 0 else WRITELN 16;
  IF 2 <= 1 THEN writeln 0 else WRITELN 17;
  IF 1 >= 2 THEN writeln 0  else WRITELN 18;
  IF not 1 = 1 THEN writeln 0 else writeln 19;

  { Testing coercion of relop }
  IF 1.0 < 2 THEN WRITELN 20;
  IF 1 < 2.0 THEN WRITELN 21
END
.
