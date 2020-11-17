
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
var d :  integer;

procedure dumpln(v : any);
begin
   dump(v);
   println("");
end;

procedure getRootTwo() -> real;
var i, squared, tmp:  real;
begin
   i := 1;
   squared := 1;
   while (.000000000001 < (2 - squared)) do
   begin
      i := i + .001;
      squared := i * i;
   end;
   return i;
end;

procedure isFive(b : real) -> real;
begin
   if (b = 5) then begin
      return 1;
   end;
end;

begin { Main }

   b := getRootTwo();
   
   dump(isFive(4));
end.  { Main }
