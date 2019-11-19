
program sqrt2;

procedure getRootTwo();
var i, squared, tmp:  real;
begin
   i := 1;
   squared := 1;
   while (.00001 < (2 - squared)) do begin
      i := i + .000001;
      squared := i * i;
   end;
   dump(i);
   println("");
   dump(squared);
end;

begin
   getRootTwo();
end.
