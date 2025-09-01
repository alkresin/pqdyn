#include <stdio.h>
#include "pg.h"

int main( void ) {

   if( pg_Init( "c:\\softools\\postgres\\libpq.dll" ) ) {
   //if( pg_Init( NULL ) ) {
      printf( "Can't load libpq" );
      return -1;
   }

   printf( "Library version: %d\n", pg_libVersion() );

   pg_Exit();

   return 0;
}