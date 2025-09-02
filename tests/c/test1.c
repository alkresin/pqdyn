#include <stdio.h>
#include "pg.h"

int main( void ) {

   if( pq_Init( "c:\\softools\\postgres\\libpq.dll" ) ) {
   //if( pq_Init( NULL ) ) {
      printf( "Can't load libpq" );
      return -1;
   }

   printf( "Library version: %d\n", pq_libVersion() );

   pq_Exit();

   return 0;
}