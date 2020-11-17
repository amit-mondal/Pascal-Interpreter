

program Main;
	    
var num : integer;


type Date = record
	       Year  : integer;
	       Month :  integer;
	    end;
var blah: Date;
var s : string;


procedure foo() -> real;
var i : real;
begin
   i := 1;
   while (i < 100000) do begin
      i := i * 1.4;
   end;
   return i;
end;

procedure anyTest(a : real) -> any;
begin
   if (a = 5) then begin
      return 12;
   end
   else begin
      return "hello"
   end;
end;

procedure fib(n : real) -> real;
begin
   if (n = 0) then begin
      return 0
   end
   else if (n = 1) begin
      return 1
   end
   else begin
      return fib(n - 1) + fib(n - 2);
   end;
end;

procedure check(d : Date) -> string;
begin
   return "Hello";
end;

begin { Main }
   println("enter a number");
   num := parseint(input());
   while (real_to_int(0) < num) do begin
      println("nice");
      num := num - real_to_int(1);
   end;
   dump(anyTest(5));

   s := -"hello!";;
   
end.  { Main }
