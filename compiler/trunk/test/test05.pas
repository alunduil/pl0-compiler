{
  Testing the optional_statements, statement_list, and statement production:
    optional_statements: statement_list | /* empty */
    statement_list: statement | statement_list ';' statement
    statement: variable ASSIGN_OP expression
               | procedure_statement
               | compound_statement
               | IF expression THEN statement
               | IF expression THEN statement ELSE statement
               | WHILE expression DO statement
               | WRITE expression
               | WRITELN expression
               | READ variable
}
PROGRAM test05();
VAR bar, i : integer;

PROCEDURE test05a;
BEGIN
  WRITELN 26
END;

BEGIN
  writeln 26;
  { Empty case is tested in tests 1, 2, 3, and 4 extensively. }
  bar := 1;
  WRITELN bar;

  IF bar THEN
  BEGIN
    bar := 2;
    WRITELN bar
  END;

  IF not bar THEn
  BEGIN
    bar := -1;
    WRITELN bar
  END
  ELSE
  BEGIN
    bar := 3;
    WRITELN bar
  END;

  if 1 then
  begin
    i := 0;
    while i < 10 DO
    begin
      if 1 then writeln i + 4;
      i := i + 1
    end
  end;

  i := 0;
  WHILE i < 10 DO 
  BEGIN
    if i mod 1 = 0 then WRITEln i + 14;
    i := i + 1
  END;

  test05a
END
.
