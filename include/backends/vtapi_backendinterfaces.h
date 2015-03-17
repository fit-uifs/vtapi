#ifndef VTAPI_BACKENDLIBS_H
#define	VTAPI_BACKENDLIBS_H

#include "../common/vtapi_config.h"

namespace vtapi
{

// ========================================= LIBPQTYPES ====================================================
#if HAVE_POSTGRESQL

typedef char *(*PQT_PQgeterror)(void);
typedef void (*PQT_PQseterror)(const char *, ...);
typedef char * (*PQT_PQgetErrorField)(int);
typedef int (*PQT_PQspecPrepare)(PGconn *, const char *, const char *, int);
typedef int (*PQT_PQclearSpecs)(PGconn *);
typedef int (*PQT_PQinitTypes)(PGconn *);
typedef int (*PQT_PQregisterSubClasses)(PGconn *, PGregisterType *, int);
typedef int (*PQT_PQregisterComposites)(PGconn *, PGregisterType *, int);
typedef int (*PQT_PQregisterUserDefinedTypes)(PGconn *, PGregisterType *, int);
typedef int (*PQT_PQregisterTypes)(PGconn *, int, PGregisterType *, int, int);
typedef int (*PQT_PQregisterResult)(PGconn *, int, PGregisterType *, int, PGresult *);
typedef int (*PQT_PQclearTypes)(PGconn *);
typedef PGparam * (*PQT_PQparamCreate)(const PGconn *);
typedef PGparam * (*PQT_PQparamDup)(PGparam *);
typedef int (*PQT_PQparamCount)(PGparam *);
typedef void (*PQT_PQparamReset)(PGparam *);
typedef void (*PQT_PQparamClear)(PGparam *);
typedef int (*PQT_PQputf)(PGparam *, const char *, ...);
typedef int (*PQT_PQputvf)(PGparam *, char *, size_t, const char *, va_list);
typedef int (*PQT_PQgetf)(const PGresult *, int, const char *, ...);
typedef int (*PQT_PQgetvf)(const PGresult *, int, const char *, va_list);
typedef PGresult * (*PQT_PQexecf)(PGconn *, const char *, ...);
typedef PGresult * (*PQT_PQexecvf)(PGconn *, const char *, va_list);
typedef int (*PQT_PQsendf)(PGconn *, const char *, ...);
typedef int (*PQT_PQsendvf)(PGconn *, const char *, va_list);
typedef PGresult * (*PQT_PQparamExec)(PGconn *, PGparam *, const char *, int);
typedef int (*PQT_PQparamSendQuery)(PGconn *, PGparam *, const char *, int);
typedef PGresult * (*PQT_PQparamExecPrepared)(PGconn *, PGparam *, const char *, int);
typedef int (*PQT_PQparamSendQueryPrepared)(PGconn *, PGparam *, const char *, int);
typedef void (*PQT_PQlocalTZInfo)(time_t *, int *, int *, char **);

typedef struct _PQTYPES_INTERFACE
{
    PQT_PQgeterror PQgeterror;
    PQT_PQseterror PQseterror;
    PQT_PQgetErrorField PQgetErrorField;
    PQT_PQspecPrepare PQspecPrepare;
    PQT_PQclearSpecs PQclearSpecs;
    PQT_PQinitTypes PQinitTypes;
    PQT_PQregisterSubClasses PQregisterSubClasses;
    PQT_PQregisterComposites PQregisterComposites;
    PQT_PQregisterUserDefinedTypes PQregisterUserDefinedTypes;
    PQT_PQregisterTypes PQregisterTypes;
    PQT_PQregisterResult PQregisterResult;
    PQT_PQclearTypes PQclearTypes;
    PQT_PQparamCreate PQparamCreate;
    PQT_PQparamDup PQparamDup;
    PQT_PQparamCount PQparamCount;
    PQT_PQparamReset PQparamReset;
    PQT_PQparamClear PQparamClear;
    PQT_PQputf PQputf;
    PQT_PQputvf PQputvf;
    PQT_PQgetf PQgetf;
    PQT_PQgetvf PQgetvf;
    PQT_PQexecf PQexecf;
    PQT_PQexecvf PQexecvf;
    PQT_PQsendf PQsendf;
    PQT_PQsendvf PQsendvf;
    PQT_PQparamExec PQparamExec;
    PQT_PQparamSendQuery PQparamSendQuery;
    PQT_PQparamExecPrepared PQparamExecPrepared;
    PQT_PQparamSendQueryPrepared PQparamSendQueryPrepared;
    PQT_PQlocalTZInfo PQlocalTZInfo;
} PQTYPES_INTERFACE;

// ================================================= LIBPQ ==========================================================

typedef PGconn *(*PQ_PQconnectdb)(const char *);
typedef ConnStatusType (*PQ_PQstatus)(const PGconn *);
typedef char * (*PQ_PQerrorMessage)(const PGconn *);
typedef void (*PQ_PQfinish)(PGconn *);
typedef void (*PQ_PQclear)(PGresult *);
typedef void * (*PQ_PQresultAlloc)(PGresult *, size_t);
typedef void (*PQ_PQfreemem)(void *);
typedef ExecStatusType (*PQ_PQresultStatus)(const PGresult *);
typedef int (*PQ_PQntuples)(const PGresult *);
typedef char * (*PQ_PQcmdStatus)(PGresult *);
typedef char *(*PQ_PQcmdTuples)(PGresult *);
typedef char * (*PQ_PQfname)(const PGresult *, int);
typedef int (*PQ_PQfnumber)(const PGresult *, const char *);
typedef Oid (*PQ_PQftype)(const PGresult *, int);
typedef int (*PQ_PQnfields)(const PGresult *);
typedef int (*PQ_PQgetisnull)(const PGresult *, int, int);
typedef char * (*PQ_PQgetvalue)(const PGresult *, int, int);
typedef int (*PQ_PQgetlength)(const PGresult *, int, int);
typedef char * (*PQ_PQescapeLiteral)(PGconn *, const char *, size_t);
typedef char * (*PQ_PQescapeIdentifier)(PGconn *, const char *, size_t);

typedef struct _POSTGRES_INTERFACE
{
    PQ_PQconnectdb PQconnectdb;
    PQ_PQstatus PQstatus;
    PQ_PQerrorMessage PQerrorMessage;
    PQ_PQfinish PQfinish;
    PQ_PQclear PQclear;
    PQ_PQresultAlloc PQresultAlloc;
    PQ_PQfreemem PQfreemem;
    PQ_PQresultStatus PQresultStatus;
    PQ_PQntuples PQntuples;
    PQ_PQcmdStatus PQcmdStatus;
    PQ_PQcmdTuples PQcmdTuples;
    PQ_PQfname PQfname;
    PQ_PQfnumber PQfnumber;
    PQ_PQftype PQftype;
    PQ_PQnfields PQnfields;
    PQ_PQgetisnull PQgetisnull;
    PQ_PQgetvalue PQgetvalue;
    PQ_PQgetlength PQgetlength;
    PQ_PQescapeLiteral PQescapeLiteral;
    PQ_PQescapeIdentifier PQescapeIdentifier;
} POSTGRES_INTERFACE;

#endif

// ================================================= SQLITE ==========================================================
#if HAVE_SQLITE

typedef int (*SL_sqlite3_open_v2)(const char *, sqlite3 **, int, const char *);
typedef int (*SL_sqlite3_close)(sqlite3 *);
typedef const char *(*SL_sqlite3_errmsg)(sqlite3*);
typedef int (*SL_sqlite3_db_status)(sqlite3*, int, int *, int *, int);
typedef int (*SL_sqlite3_exec)( sqlite3*, const char *sql, int (*callback)(void*,int,char**,char**), void *, char **);
typedef void (*SL_sqlite3_free)(void*);
typedef int (*SL_sqlite3_get_table)( sqlite3 *, const char *, char ***, int *, int *, char **);
typedef const char *(*SL_sqlite3_db_filename)(sqlite3 *, const char *);
typedef void (*SL_sqlite3_free_table)(char **);

typedef struct _SQLITE_INTERFACE
{
    SL_sqlite3_open_v2 sqlite3_open_v2;
    SL_sqlite3_close sqlite3_close;
    SL_sqlite3_errmsg sqlite3_errmsg;
    SL_sqlite3_db_status sqlite3_db_status;
    SL_sqlite3_exec sqlite3_exec;
    SL_sqlite3_free sqlite3_free;
    SL_sqlite3_get_table sqlite3_get_table;
    SL_sqlite3_db_filename sqlite3_db_filename;
    SL_sqlite3_free_table sqlite3_free_table;
} SQLITE_INTERFACE;

#endif
// ===================================================================================================================


} // namespace vtapi

#endif