/*
 * hbPQdyn - Harbour PostgreSQL API
 * Harbour bindings for libpq - dynamic linking
 *
 * Copyright 2025 Alexander S.Kresin <alex@kresin.ru>
 * www - http://www.kresin.ru
 */

#include "hbapi.h"
#include "hbapifs.h"
#include "hbapiitm.h"
#include "hbvm.h"
#include "item.api"

#include "pg.h"

/* pq_Init( [ cDllName ] ) --> nResult
 */
HB_FUNC( PQ_INIT ) {

   hb_retni( pq_Init( (HB_ISCHAR(1))?  hb_parc(1) : NULL ) );
}

/* pq_Exit()
 */
HB_FUNC( PQ_EXIT ) {

   pq_Exit();
}

/* pq_LibVersion() -> nVersion
 */
HB_FUNC( PQ_LIBVERSION ) {

   hb_retni( pq_libVersion() );
}

/* pq_SrvVersion( pConnection ) -> nVersion
 */
HB_FUNC( PQ_SRVVERSION ) {

   hb_retni( pq_srvVersion( hb_parptr(1) ) );
}

/* pq_Oing( cConnInfo ) -> nResult
 */
HB_FUNC( PQ_PING ) {

   hb_retni( pq_ping( hb_parc(1) ) );
}

/* pq_ConnectDB( cConnInfo ) -> pConnection
 */
HB_FUNC( PQ_CONNECTDB ) {

   hb_retptr( pq_connectDb( hb_parc(1) ) );
}

/* pq_Status( pConnection ) -> nStatus
 */
HB_FUNC( PQ_STATUS ) {

   hb_retni( pq_status( hb_parptr(1) ) );
}

/* pq_Finish( pConnection ) -> nStatus
 */
HB_FUNC( PQ_FINISH ) {

   pq_finish( hb_parptr(1) );
}

/* pq_Exec( pConnection, cQuery ) -> pResult
 */
HB_FUNC( PQ_EXEC ) {

   hb_retptr( pq_exec( hb_parptr(1), hb_parc(2) ) );
}

/* pq_ExecParams( pConnection, cQuery, aValues ) -> pResult
 */
HB_FUNC( PQ_EXECPARAMS ) {

   PHB_ITEM pArr = NULL;

   if( HB_ISARRAY(3) ) {
      int i, iArrLen;
      pArr = hb_param( 3, HB_IT_ARRAY );
      iArrLen = hb_arrayLen( pArr );
      char **pValues = (char**) malloc( sizeof(char*) * iArrLen );
      for( i = 0; i <= iArrLen; i ++ ) {
         pValues[i] = (char*) hb_arrayGetCPtr( pArr, i+1 );
      }
      PGresult* pResult = pq_execParams( hb_parptr(1), hb_parc(2), iArrLen,
         NULL, (const char *const *) pValues, NULL, NULL, 0 );
      free( pValues );
      hb_retptr( pResult );
      return;
   } else {
      hb_retptr( pq_execParams( hb_parptr(1), hb_parc(2), 0,
         NULL, NULL, NULL, NULL, 0 ) );
   }
}

/* pq_ResultStatus( pResult ) -> nStatus
 */
HB_FUNC( PQ_RESULTSTATUS ) {

   hb_retni( pq_resultStatus( hb_parptr(1) ) );
}

/* pq_Clear( pResult )
 */
HB_FUNC( PQ_CLEAR ) {

   pq_clear( hb_parptr(1) );
}

/* pq_GetValue( pResult, nRow, nCol ) -> cValue
 */
HB_FUNC( PQ_GETVALUE ) {

   hb_retc( pq_getvalue( hb_parptr(1), hb_parni(2), hb_parni(3) ) );
}

/* pq_NTuples( pResult ) -> nTuples
 */
HB_FUNC( PQ_NTUPLES ) {

   hb_retni( pq_ntuples( hb_parptr(1) ) );
}

/* pq_NFields( pResult ) -> nFields
 */
HB_FUNC( PQ_NFIELDS ) {

   hb_retni( pq_nfields( hb_parptr(1) ) );
}

/* pq_FName( pResult, int nField ) -> cName
 */
HB_FUNC( PQ_FNAME ) {

   hb_retc( pq_fname( hb_parptr(1), hb_parni(2) ) );
}