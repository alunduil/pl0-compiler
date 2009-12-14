{ 
  The simplest program to check the program production:
    program: PROGRAM ID '(' ')' ';' declarations subprogram_declarations compound_statement '.'
  Also, tests the compound_statement production:
    compound_statement: BEGIN optional_statements END
}
PROGRAM test01();
BEGIN
  writeln 0
END
.
