{Here you go alex, since it can change a bit with the numbers you put in, here is one test I did}
{Enter 3 and -3 for the 2 reads in the beginning}
{output should be 3 -3 12 10 5 20 4 4 0 0 80 80 100 100 25 25 -25 -25 5 5 10 10 -10 -10 2 2 1 1 3 3 555553 99999 3 3 3 3 3 3 3 99999 2 1 -2 -1 0 1 777 99999}

program
const con=10 , don = 20;

var jack, jill, phil;
var rabbit, fox, chill;

procedure trueoptest;
	procedure falseoptest;
		procedure math;
			const two = 2;
			const five = 5, ten = 10;
			const negTwo = -2;
			const negFive = -5, negTen = -10;
			var add, sub, mult, divis, total;
			var chill;

			begin
			{testing setting variables}
			add := 12;
			print add;
			sub := 10;
			print sub;
			mult := 5;
			print mult;
			divis := 20;
			print divis;

		{testing math functions}
			print 4;
			total := 2 + two;
			print total;

			print 0;
			total := 2 + negTwo;
			print total;

			print 80;
			total := 90 - sub;
			print total;
			
			print 100;
			total := 90 - -10;
			print total;

			print 25;
			total := five * mult;
			print total;

			print -25;
			total := negFive * mult;
			print total;

			print 5;
			total := 100/divis;
			print total;

			print 10;
			total := 100 DIV ten;
			print total;

			print -10;
			total := 100 DIV negTen;
			print total;

			print 2;
			print 8 MOD 3;

			print 1;
			print 10 MOD 3;

			print 3;
			print 13 MOD 5;

			chill := 555553;
			print chill;
			end;

	begin
	{testing condition statements when they are wrong, no output from these!!!}
		if odd 8 then
	    	    print jack;
		if 26 < 4 then
	    	    print jack;
		if 3 > 290 then
		    print jack;
		if 2 >= 10 then
		    print jack;	
		if 104 <= 12 then
		    print jack;
		if 12 = 121 then
		    print jack;
		if 13 <> 13 then
		    print jack;
		call math;
		print phil;
	end;

begin 
	{call false condition test}
	call falseoptest;

	{test condition statements when they are true, will output 3 seven times}
	if odd 7 then
	    print jill;
	if 2 < 4 then
	    print jill;
	if 3>2 then
	    print jill;
	if 2 >= 1 then
	    print jill;	
	if 10 <= 12 then
	    print jill;
	if 12 = 12 then
	    print jill;
	if 13 <> 11 then
	    print jill;
	print phil
end;

begin
{read in 2 variables}
read rabbit;
read fox;

{print the 2 variables read in}
print rabbit;
print fox;

{set variables}
jill := 3;
jack := 777;
phil := 99999;
chill := 777;

{call true condition test}
call trueoptest;

{test loops, while rabbit is greater than one, count down until it is one}
while rabbit > 1 do
begin
	rabbit := rabbit - 1;
	print rabbit
end;

{if fox is less than one, count it up to one}
while fox <= 0 do
begin
	fox := fox + 1;
	print fox
end;

print chill;
print phil
end
.
