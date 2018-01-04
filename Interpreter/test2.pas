program Main;
var x, y :  real;

procedure Alpha(a, b :  integer);
var s :  integer;
begin
   x := a + y + b;
   if (x) then begin
y := x + 3;
    end;
end;

procedure Recurse(a: integer);
begin
if (a) then begin
    Recurse(a - 1);
    Alpha(a, x);
end;
end;


begin { Main }

x := 7;
y := x + 3;
Recurse(x);

end.  { Main }
