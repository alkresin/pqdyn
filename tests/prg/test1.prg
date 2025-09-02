/*
 */

FUNCTION Main( cHost )

   IF pq_Init( "c:\softools\postgres\libpq.dll" ) != 0
      ? "Can't load libpq"
      RETURN Nil
   ENDIF

   ? "Library version:", pq_LibVersion()
   IF !Empty( cHost )
      ? "Ping:", pq_ping( "host=" + cHost )
      // ? "Ping:", pq_ping( "postgresql://" + cHost )
   ENDIF

   pq_Exit( )

   ? "Press ant key"
   Inkey(0)

   RETURN Nil