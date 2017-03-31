PROGRAM Part12;
VAR
   a : INTEGER;
   b : INTEGER;
PROCEDURE P1;
VAR
   a : REAL;
   k : INTEGER;

   PROCEDURE P2;
   VAR
      a, z : INTEGER;
   BEGIN {P2}
      z := 777 * 32 / 22;
      z := a;
      a := z;
      b := z;
      a := b;
      b := a;
   END;  {P2}

BEGIN {P1}

END;  {P1}

BEGIN {Part12}
   a := 10;
   b := 22222222222222222;
END.  {Part12}