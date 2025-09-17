# hbPQdyn
A small library - bindings to the PostgreSQL C API for C and Harbour.

## Short introduction

Why was it needed to write this library? C API exists already, Harbour bindings - too, why write
another one?.

The main goal is to simplify as much as possible compiling, linking and using applications,
which uses postgreSQL.

Building the app doesn't require libpq headers and binaries. Libpq shared libraries will be called
dynamically when the ready application needs them, no need for libpq files before.

hbPQdyn provides a minimum set of functions, those only, which are really necessary, IMO, to use PostgreSQL.
This approach makes life easier for the developer.

## Functions list

#### pq_Init( [ cDllName ] ) --> nResult

#### pq_Exit()

#### pq_LibVersion() -> nVersion

#### pq_SrvVersion( pConnection ) -> nVersion

#### pq_Oing( cConnInfo ) -> nResult

#### pq_ConnectDB( cConnInfo ) -> pConnection

#### pq_Status( pConnection ) -> nStatus

#### pq_Finish( pConnection ) -> nStatus

#### pq_Exec( pConnection, cQuery ) -> pResult

#### pq_ExecParams( pConnection, cQuery, aValues ) -> pResult

#### pq_ResultStatus( pResult ) -> nStatus

#### pq_Clear( pResult )

#### pq_GetValue( pResult, nRow, nCol ) -> cValue

#### pq_NTuples( pResult ) -> nTuples

#### pq_NFields( pResult ) -> nFields

#### pq_FName( pResult, int nField ) -> cName
