program Main;
var x, y :  real;
var str: string;
var varname :  string;
var myvar :  any;
var i : integer;

procedure Alpha(a, b :  real);
var s :  real;
begin
   x := a + y + b;
   
   if (x) then begin
      y := x + 3;
   end;

   panic()
   
end;


procedure fib(i : integer) -> integer;
begin
   if (i = real_to_int(0)) then begin
      return real_to_int(1);
   end
   else if (i = real_to_int(1)) begin
      return real_to_int(1);
   end
   else begin
      return fib(i - real_to_int(1)) + fib(i - real_to_int(2));
   end;
end;

begin { Main }
  {print(fib(4));}
   i := real_to_int(0);
   while (i < real_to_int(10000)) do
   begin
      i := i - real_to_int(1);
   end;

end.  { Main }
