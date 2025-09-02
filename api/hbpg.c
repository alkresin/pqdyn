

#include "hbapi.h"
#include "hbapifs.h"
#include "hbapiitm.h"
#include "hbvm.h"
#include "item.api"

#include "pg.h"

HB_FUNC( PQ_INIT ) {

   hb_retni( pq_Init( (HB_ISCHAR(1))?  hb_parc(1) : NULL ) );
}

HB_FUNC( PQ_EXIT ) {

   pq_Exit();
}

HB_FUNC( PQ_LIBVERSION ) {

   hb_retni( pq_libVersion() );
}

HB_FUNC( PQ_SRVVERSION ) {

   hb_retni( pq_srvVersion( hb_parptr(1) ) );
}

HB_FUNC( PQ_PING ) {

   hb_retni( pq_ping( hb_parc(1) ) );
}

HB_FUNC( PQ_CONNECTDB ) {

   hb_retptr( pq_connectDb( hb_parc(1) ) );
}

HB_FUNC( PQ_STATUS ) {

   hb_retni( pq_status( hb_parptr(1) ) );
}

HB_FUNC( PQ_FINISH ) {

   pq_finish( hb_parptr(1) );
}

HB_FUNC( PQ_EXEC ) {

   hb_retptr( pq_exec( hb_parptr(1), hb_parc(2) ) );
}

/*
HB_FUNC( PQ_EXECPARAMS ) {

   hb_retptr( pq_execParams( hb_parptr(1), hb_parc(2), hb_parni(3),
      const Oid *paramTypes, const char *const *paramValues, const int *paramLengths,
      const int *paramFormats, int resultFormat ) );
}
*/

HB_FUNC( PQ_RESULTSTATUS ) {

   hb_retni( pq_resultStatus( hb_parptr(1) ) );
}

HB_FUNC( PQ_CLEAR ) {

   pq_clear( hb_parptr(1) );
}

HB_FUNC( PQ_GETVALUE ) {

   hb_retc( pq_getvalue( hb_parptr(1), hb_parni(2), hb_parni(3) ) );
}

HB_FUNC( PQ_NTUPLES ) {

   hb_retni( pq_ntuples( hb_parptr(1) ) );
}