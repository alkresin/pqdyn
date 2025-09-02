
/* PGconn encapsulates a connection to the backend.
 * The contents of this struct are not supposed to be known to applications.
 */
typedef struct pg_conn PGconn;

/* PGresult encapsulates the result of a query (or more precisely, of a single
 * SQL command --- a query string given to PQsendQuery can contain multiple
 * commands and thus return multiple PGresult objects).
 * The contents of this struct are not supposed to be known to applications.
 */
typedef struct pg_result PGresult;

/*
 * Object ID is a fundamental type in Postgres.
 */
typedef unsigned int Oid;

/*
 * PGPing - The ordering of this enum should not be altered because the
 * values are exposed externally via pg_isready.
 */
typedef enum
{
	PQPING_OK,					/* server is accepting connections */
	PQPING_REJECT,				/* server is alive but rejecting connections */
	PQPING_NO_RESPONSE,			/* could not establish connection */
	PQPING_NO_ATTEMPT			/* connection not attempted (bad params) */
} PGPing;

typedef enum
{
	CONNECTION_OK,
	CONNECTION_BAD,
	/* Non-blocking mode only below here */

	/*
	 * The existence of these should never be relied upon - they should only
	 * be used for user feedback or similar purposes.
	 */
	CONNECTION_STARTED,			/* Waiting for connection to be made.  */
	CONNECTION_MADE,			/* Connection OK; waiting to send.     */
	CONNECTION_AWAITING_RESPONSE,	/* Waiting for a response from the
									 * postmaster.        */
	CONNECTION_AUTH_OK,			/* Received authentication; waiting for
								 * backend startup. */
	CONNECTION_SETENV,			/* This state is no longer used. */
	CONNECTION_SSL_STARTUP,		/* Performing SSL handshake. */
	CONNECTION_NEEDED,			/* Internal state: connect() needed. */
	CONNECTION_CHECK_WRITABLE,	/* Checking if session is read-write. */
	CONNECTION_CONSUME,			/* Consuming any extra messages. */
	CONNECTION_GSS_STARTUP,		/* Negotiating GSSAPI. */
	CONNECTION_CHECK_TARGET,	/* Internal state: checking target server
								 * properties. */
	CONNECTION_CHECK_STANDBY,	/* Checking if server is in standby mode. */
	CONNECTION_ALLOCATED,		/* Waiting for connection attempt to be
								 * started.  */
} ConnStatusType;

typedef enum
{
	PGRES_EMPTY_QUERY = 0,		/* empty query string was executed */
	PGRES_COMMAND_OK,			/* a query command that doesn't return
								 * anything was executed properly by the
								 * backend */
	PGRES_TUPLES_OK,			/* a query command that returns tuples was
								 * executed properly by the backend, PGresult
								 * contains the result tuples */
	PGRES_COPY_OUT,				/* Copy Out data transfer in progress */
	PGRES_COPY_IN,				/* Copy In data transfer in progress */
	PGRES_BAD_RESPONSE,			/* an unexpected response was recv'd from the
								 * backend */
	PGRES_NONFATAL_ERROR,		/* notice or warning message */
	PGRES_FATAL_ERROR,			/* query failed */
	PGRES_COPY_BOTH,			/* Copy In/Out data transfer in progress */
	PGRES_SINGLE_TUPLE,			/* single tuple from larger resultset */
	PGRES_PIPELINE_SYNC,		/* pipeline synchronization point */
	PGRES_PIPELINE_ABORTED,		/* Command didn't run because of an abort
								 * earlier in a pipeline */
	PGRES_TUPLES_CHUNK			/* chunk of tuples from larger resultset */
} ExecStatusType;

extern int pq_Init( const char* szDllName);
extern void pq_Exit();
extern int pq_libVersion();
extern int pq_srvVersion( const PGconn *conn );
extern PGPing pq_ping( const char * szConnInfo );
extern PGconn * pq_connectDb( const char * szConnInfo );
extern void pq_finish( PGconn *conn );
extern ConnStatusType pq_status( const PGconn *conn );
extern PGresult* pq_exec( PGconn *conn, const char *query );
extern PGresult* pq_execParams( PGconn *conn, const char *command, int nParams,
   const Oid *paramTypes, const char *const *paramValues, const int *paramLengths,
   const int *paramFormats, int resultFormat );
extern ExecStatusType pq_resultStatus( const PGresult *res );
extern void pq_clear( PGresult *res );
extern char* pq_getvalue( const PGresult *res, int tup_num, int field_num );
extern int pq_ntuples( PGresult *res );
extern int pq_nfields( PGresult *res );
extern char* pq_fname( PGresult *res, int field_num );