
program Main;

var num:string;
var a :  string;
var b, c :  real;

procedure PrintTwice(s : string);
begin
   println(s);
   println(s);
end;

procedure Append(s1, s2 : string);
begin
   println(s1 + s2);
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

procedure calculatePI();
var pi, f, denom, ctr, op :  real;
begin
   pi := 4;
   ctr := 0;
   op := 0;
   denom := 3;

   while (ctr < 1000000) do begin
      f := 4 / denom;
      if (op = 0) then begin
	 pi := pi - f;
	 op := 1;
      end
      else begin
	 pi := pi + f;
	 op := 0;
      end;
      denom := denom + 2;
      ctr := ctr + 1;
   end;

   dump(pi);
end;

begin { Main }

   {date.Year = 33;}
   a := "hello";
   PrintTwice(a);
   strmodify(a, "e", 1);
   Append(a, "blah");
   calculatePI();
   
end.  { Main }
