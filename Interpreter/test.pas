

program Main;

type Date = record
	       Year  : integer;
	       Month :  integer;
	    end;
	    
var str: string;
var num:real;
var date : Date;
var n :  any;

procedure print(a: string);
begin
end;

begin { Main }
   str := "look at this great string";
   n := str;
   num := 3;

   if (num != 4) then begin      
      print(str);
   end;
   
   
   print(num);
end.  { Main }
