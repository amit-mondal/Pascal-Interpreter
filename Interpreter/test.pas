
program Main;

type Date = record
	       Year  : integer;
	       Month :  integer;
	    end;

var str1, str2: string;
var num:real;
var date : Date;
var a :  string;
var b, c :  real;

procedure PrintTwice(s : string);
begin
   println(s);
   println(s);
end;

procedure getRootTwo();
var i, squared, tmp:  real;
begin
   i := 1;
   squared := 1;
   while (.00001 < (2 - squared)) do begin
      i := i + .001;
      squared := i * i;
      dump(squared);
      println("");
   end;
   dump(i);
   dump(squared);
end;

begin { Main }

   {date.Year = 33;}

   getRootTwo();
end.  { Main }
