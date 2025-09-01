

#include "hbapi.h"
#include "hbapifs.h"
#include "hbapiitm.h"
#include "hbvm.h"
#include "item.api"

#include "pg.h"

HB_FUNC( PG_INIT ) {

   hb_retni( pg_Init( (HB_ISCHAR(1))?  hb_parc(1) : NULL ) );
}

HB_FUNC( PG_EXIT ) {

   pg_Exit();
}

HB_FUNC( PG_LIBVERSION ) {

   hb_retni( pg_libVersion() );
}

HB_FUNC( PG_SRVVERSION ) {

   hb_retni( pg_srvVersion( hb_parptr(1) ) );
}

HB_FUNC( PG_CONNECTDB ) {

   hb_retptr( pg_connectDb( hb_parc(1) ) );
}

HB_FUNC( PG_STATUS ) {

   hb_retni( pg_status( hb_parptr(1) ) );
}

HB_FUNC( PG_FINISH ) {

   pg_finish( hb_parptr(1) );
}

HB_FUNC( PG_EXEC ) {

   hb_retptr( pg_exec( hb_parptr(1), hb_parc(2) ) );
}

HB_FUNC( PG_RESULTSTATUS ) {

   hb_retni( pg_resultStatus( hb_parptr(1) ) );
}

HB_FUNC( PG_CLEAR ) {

   pg_clear( hb_parptr(1) );
}

HB_FUNC( PG_GETVALUE ) {

   hb_retc( pg_getvalue( hb_parptr(1), hb_parni(2), hb_parni(3) ) );
}

HB_FUNC( PG_NTUPLES ) {

   hb_retni( pg_ntuples( hb_parptr(1) ) );
}
