

program Main;
	    
var num : any;

procedure foo();
var i : real;
begin
   i := 0;
   while (i != 10000) do begin
      println("hello ");
      sleep(1);
      i := i * 1.4;
      dump(i);
   end;
end;

begin { Main }
   foo();
end.  { Main }
