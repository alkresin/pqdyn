/*
 */

FUNCTION Main()

   IF pg_Init( "c:\softools\postgres\libpq.dll" ) != 0
      ? "Can't load libpq"
      RETURN Nil
   ENDIF

   ? "Library version:", pg_LibVersion()

   pg_Exit( )

   ? "Press ant key"
   Inkey(0)

   RETURN Nil