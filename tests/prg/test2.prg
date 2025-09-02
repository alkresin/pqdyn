/*
 */

FUNCTION Main( cHost, cDbName, cUser, cPass )

   LOCAL hConn

   IF Empty( cHost ) .OR. Empty( cDbName )
      ? "Parameters absent"
      RETURN Nil
   ENDIF

   IF pq_Init( "c:\softools\postgres\libpq.dll" ) != 0
      ? "Can't load libpq"
      RETURN Nil
   ENDIF

   IF Empty( cUser )
      cUser := "postgres"
   ENDIF
   IF Empty( cPass )
      ACCEPT "Password" TO cPass
   ENDIF
   IF Empty( cPass )
      RETURN Nil
   ENDIF

   //IF Empty( hConn := pq_ConnectDb( "host=" + cHost + " user=" + cUser + " password=" + cPass + " dbname=" + cDbName ) )
   IF Empty( hConn := pq_ConnectDb( "postgresql://" + cUser + ":" + cPass + "@" + cHost + "/" + cDbName ) )
      pq_Exit( )
      ? "Can't connect"
      FWait()
      RETURN Nil
   ENDIF

   IF pq_Status( hConn ) != 0
      ? "Can't connect to " + cDbName
      pq_Exit( )
      FWait()
      RETURN Nil
   ENDIF

   ? "Server version:", pq_SrvVersion( hConn )

   pq_Finish( hConn )
   pq_Exit( )

   FWait()

   RETURN Nil

STATIC FUNCTION FWait()

   ? "Press ant key"
   Inkey(0)
   RETURN Nil