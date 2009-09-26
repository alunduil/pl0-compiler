program
var up, lo; 
procedure A;
	var y, z;
	procedure C;
	var z, m ;
	begin
		z := 12;
		m := 7;
		y := z;
		y := m;
		y := y
	end;
	procedure D;
	var z, n;
	begin
		z := 1;
		z := z * 10;
		n := (z mod 4) / 7 + 6;
		CALL C;
	end;
	begin
		y := 0;
		if y = 0 then
			z := 10 mod 4;
		CALL D
	end;
procedure B;
var f,g;
begin
	f := 7;
	g := 25;
	y := f;
	y := g;
	CALL A;
end;
begin
	CALL B;
	print y;
	print y;
	print y
end.
