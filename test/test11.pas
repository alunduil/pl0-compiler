{
  Testing the addition operators and all behaviour surrounding them.
  The production being tested:
    simple_expression: term | sign term | simple_expression ADDOP term
}
PROGRAM test11();
BEGIN
  writeln 16;

  { Signs of numbers float and int. }
  IF 5 = 5 THEN WRITELN 1;
  IF -5 = -5 THEN WRITELN 2;
  IF 5.0e20 = 5.0e20 THEN WRITELN 3;
  IF -5.0e-20 = -5.0e-20 THEN WRITELN 4;

  { The addition operators for floats. }
  IF 2.0 + 2.0 = 4.0 THEN WRITELN 5;
  IF 2.0 - 2.0 = .0 THEN WRITELN 6;
  IF 1.0 OR 0.0 THEN WRITELN 7;

  { The addition operator for ints. }
  IF 2 + 2 = 4 THEN WRITELN 8;
  IF 2 - 2 = 0 THEN WRITELN 9;
  IF 1 or 0 THEN WRITELN 10;

  { Looking at the other side of OR }
  IF -1 OR 0 THEN WRITELN 11;
  IF 0 OR 0 THEN writeln 0 else WRITELN 12;

  { Coercion both ways. }
  IF 2.0 + 2 = 4.0 THEN WRITELN 13;
  IF 2 + 2.0 = 4.0 THEN WRITELN 14;

  { Mix and match a little. }
  IF 2.25 - .25 + 2. - 4 = 0 THEN WRITELN 15;
  IF 6.23e-3 + 4.22e2 - 3e-2 = 421.97623000000004 THEN WRITELN 16
END
.
