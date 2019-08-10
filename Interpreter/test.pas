

program Main;

type Date = record
	       Year  : integer;
	       Month :  integer;
	    end;
	    
var str1, str2: string;
var num:real;
var date : Date;
var a, b :  string;
var c :  integer;

procedure PrintTwice(s : string);
begin
   println(s);
   println(s);
end;

begin { Main }
   str1 := "hello";
   str2 := "goodbye";
   {print(str2);}
   c := stoi(123)
   println(str2 + str1);	     
   
end.  { Main }
