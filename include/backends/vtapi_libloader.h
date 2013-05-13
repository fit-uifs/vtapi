/* 
 * File:   vtapi_libloader.h
 * Author: vojca
 *
 * Created on May 4, 2013, 10:34 PM
 */

#ifndef LIBLOADER_H
#define	LIBLOADER_H

namespace vtapi {
    class LibLoader;
    class PGLibLoader;
    class SLLibLoader;
}

#include "../common/vtapi_global.h"

namespace vtapi {


#define FUNC_MAP                        func_map
#define FUNC_MAP_ENTRY(func,funcptr)    std::make_pair<string,void *>(func,funcptr)

typedef map<string,void *>              func_map_t;


// ========================================= LIBPQTYPES ====================================================

typedef char * (*PQT_PQgeterror)(void);
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

#define CALL_PQT_PQgeterror ((PQT_PQgeterror) (*FUNC_MAP)[string("PQT_PQgeterror")])
#define CALL_PQT_PQseterror ((PQT_PQseterror) (*FUNC_MAP)[string("PQT_PQseterror")])
#define CALL_PQT_PQgetErrorField ((PQT_PQgetErrorField) (*FUNC_MAP)[string("PQT_PQgetErrorField")])
#define CALL_PQT_PQspecPrepare ((PQT_PQspecPrepare) (*FUNC_MAP)[string("PQT_PQspecPrepare")])
#define CALL_PQT_PQclearSpecs ((PQT_PQclearSpecs) (*FUNC_MAP)[string("PQT_PQclearSpecs")])
#define CALL_PQT_PQinitTypes ((PQT_PQinitTypes) (*FUNC_MAP)[string("PQT_PQinitTypes")])
#define CALL_PQT_PQregisterSubClasses ((PQT_PQregisterSubClasses) (*FUNC_MAP)[string("PQT_PQregisterSubClasses")])
#define CALL_PQT_PQregisterComposites ((PQT_PQregisterComposites) (*FUNC_MAP)[string("PQT_PQregisterComposites")])
#define CALL_PQT_PQregisterUserDefinedTypes ((PQT_PQregisterUserDefinedTypes) (*FUNC_MAP)[string("PQT_PQregisterUserDefinedTypes")])
#define CALL_PQT_PQregisterTypes ((PQT_PQregisterTypes) (*FUNC_MAP)[string("PQT_PQregisterTypes")])
#define CALL_PQT_PQregisterResult ((PQT_PQregisterResult) (*FUNC_MAP)[string("PQT_PQregisterResult")])
#define CALL_PQT_PQclearTypes ((PQT_PQclearTypes) (*FUNC_MAP)[string("PQT_PQclearTypes")])
#define CALL_PQT_PQparamCreate ((PQT_PQparamCreate) (*FUNC_MAP)[string("PQT_PQparamCreate")])
#define CALL_PQT_PQparamDup ((PQT_PQparamDup) (*FUNC_MAP)[string("PQT_PQparamDup")])
#define CALL_PQT_PQparamCount ((PQT_PQparamCount) (*FUNC_MAP)[string("PQT_PQparamCount")])
#define CALL_PQT_PQparamReset ((PQT_PQparamReset) (*FUNC_MAP)[string("PQT_PQparamReset")])
#define CALL_PQT_PQparamClear ((PQT_PQparamClear) (*FUNC_MAP)[string("PQT_PQparamClear")])
#define CALL_PQT_PQputf ((PQT_PQputf) (*FUNC_MAP)[string("PQT_PQputf")])
#define CALL_PQT_PQputvf ((PQT_PQputvf) (*FUNC_MAP)[string("PQT_PQputvf")])
#define CALL_PQT_PQgetf ((PQT_PQgetf) (*FUNC_MAP)[string("PQT_PQgetf")])
#define CALL_PQT_PQgetvf ((PQT_PQgetvf) (*FUNC_MAP)[string("PQT_PQgetvf")])
#define CALL_PQT_PQexecf ((PQT_PQexecf) (*FUNC_MAP)[string("PQT_PQexecf")])
#define CALL_PQT_PQexecvf ((PQT_PQexecvf) (*FUNC_MAP)[string("PQT_PQexecvf")])
#define CALL_PQT_PQsendf ((PQT_PQsendf) (*FUNC_MAP)[string("PQT_PQsendf")])
#define CALL_PQT_PQsendvf ((PQT_PQsendvf) (*FUNC_MAP)[string("PQT_PQsendvf")])
#define CALL_PQT_PQparamExec ((PQT_PQparamExec) (*FUNC_MAP)[string("PQT_PQparamExec")])
#define CALL_PQT_PQparamSendQuery ((PQT_PQparamSendQuery) (*FUNC_MAP)[string("PQT_PQparamSendQuery")])
#define CALL_PQT_PQparamExecPrepared ((PQT_PQparamExecPrepared) (*FUNC_MAP)[string("PQT_PQparamExecPrepared")])
#define CALL_PQT_PQparamSendQueryPrepared ((PQT_PQparamSendQueryPrepared) (*FUNC_MAP)[string("PQT_PQparamSendQueryPrepared")])
#define CALL_PQT_PQlocalTZInfo ((PQT_PQlocalTZInfo) (*FUNC_MAP)[string("PQT_PQlocalTZInfo")])

// ================================================= LIBPQ ==========================================================

typedef PGconn * (*PQ_PQconnectdb)(const char *);
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

#define CALL_PQ_PQconnectdb ((PQ_PQconnectdb) (*FUNC_MAP)[string("PQ_PQconnectdb")])
#define CALL_PQ_PQstatus ((PQ_PQstatus) (*FUNC_MAP)[string("PQ_PQstatus")])
#define CALL_PQ_PQerrorMessage ((PQ_PQerrorMessage) (*FUNC_MAP)[string("PQ_PQerrorMessage")])
#define CALL_PQ_PQfinish ((PQ_PQfinish) (*FUNC_MAP)[string("PQ_PQfinish")])
#define CALL_PQ_PQclear ((PQ_PQclear) (*FUNC_MAP)[string("PQ_PQclear")])
#define CALL_PQ_PQresultAlloc ((PQ_PQresultAlloc) (*FUNC_MAP)[string("PQ_PQresultAlloc")])
#define CALL_PQ_PQfreemem ((PQ_PQfreemem) (*FUNC_MAP)[string("PQ_PQfreemem")])
#define CALL_PQ_PQresultStatus ((PQ_PQresultStatus) (*FUNC_MAP)[string("PQ_PQresultStatus")])
#define CALL_PQ_PQntuples ((PQ_PQntuples) (*FUNC_MAP)[string("PQ_PQntuples")])
#define CALL_PQ_PQcmdStatus ((PQ_PQcmdStatus) (*FUNC_MAP)[string("PQ_PQcmdStatus")])
#define CALL_PQ_PQcmdTuples ((PQ_PQcmdTuples) (*FUNC_MAP)[string("PQ_PQcmdTuples")])
#define CALL_PQ_PQfname ((PQ_PQfname) (*FUNC_MAP)[string("PQ_PQfname")])
#define CALL_PQ_PQfnumber ((PQ_PQfnumber) (*FUNC_MAP)[string("PQ_PQfnumber")])
#define CALL_PQ_PQftype ((PQ_PQftype) (*FUNC_MAP)[string("PQ_PQftype")])
#define CALL_PQ_PQnfields ((PQ_PQnfields) (*FUNC_MAP)[string("PQ_PQnfields")])
#define CALL_PQ_PQgetisnull ((PQ_PQgetisnull) (*FUNC_MAP)[string("PQ_PQgetisnull")])
#define CALL_PQ_PQgetvalue ((PQ_PQgetvalue) (*FUNC_MAP)[string("PQ_PQgetvalue")])
#define CALL_PQ_PQgetlength ((PQ_PQgetlength) (*FUNC_MAP)[string("PQ_PQgetlength")])
#define CALL_PQ_PQescapeLiteral ((PQ_PQescapeLiteral) (*FUNC_MAP)[string("PQ_PQescapeLiteral")])
#define CALL_PQ_PQescapeIdentifier ((PQ_PQescapeIdentifier) (*FUNC_MAP)[string("PQ_PQescapeIdentifier")])

// ================================================= SQLITE ==========================================================

typedef int (*SL_sqlite3_open_v2)(const char *, sqlite3 **, int, const char *);
typedef int (*SL_sqlite3_close)(sqlite3 *);
typedef const char *(*SL_sqlite3_errmsg)(sqlite3*);
typedef int (*SL_sqlite3_db_status)(sqlite3*, int, int *, int *, int);
typedef int (*SL_sqlite3_exec)( sqlite3*, const char *sql, int (*callback)(void*,int,char**,char**), void *, char **);
typedef void (*SL_sqlite3_free)(void*);
typedef int (*SL_sqlite3_get_table)( sqlite3 *, const char *, char ***, int *, int *, char **);
typedef const char *(*SL_sqlite3_db_filename)(sqlite3 *, const char *);
typedef void (*SL_sqlite3_free_table)(char **);

#define CALL_SL_sqlite3_open_v2 ((SL_sqlite3_open_v2) (*FUNC_MAP)[string("SL_sqlite3_open_v2")])
#define CALL_SL_sqlite3_close ((SL_sqlite3_close) (*FUNC_MAP)[string("SL_sqlite3_close")])
#define CALL_SL_sqlite3_errmsg ((SL_sqlite3_errmsg) (*FUNC_MAP)[string("SL_sqlite3_errmsg")])
#define CALL_SL_sqlite3_db_status ((SL_sqlite3_db_status) (*FUNC_MAP)[string("SL_sqlite3_db_status")])
#define CALL_SL_sqlite3_exec ((SL_sqlite3_exec) (*FUNC_MAP)[string("SL_sqlite3_exec")])
#define CALL_SL_sqlite3_free ((SL_sqlite3_free) (*FUNC_MAP)[string("SL_sqlite3_free")])
#define CALL_SL_sqlite3_get_table ((SL_sqlite3_get_table) (*FUNC_MAP)[string("SL_sqlite3_get_table")])
#define CALL_SL_sqlite3_db_filename ((SL_sqlite3_db_filename) (*FUNC_MAP)[string("SL_sqlite3_db_filename")])
#define CALL_SL_sqlite3_free_table ((SL_sqlite3_free_table) (*FUNC_MAP)[string("SL_sqlite3_free_table")])

// ===================================================================================================================

//TODO: comment
class LibLoader {
public:

    LibLoader() {};
    virtual ~LibLoader() {};

    virtual func_map_t *load() = 0;
    virtual int unload() = 0;

};


class PGLibLoader : public LibLoader {
private:

    lt_dlhandle h_libpqtypes;
    lt_dlhandle h_libpq;
    
public:

    PGLibLoader();
    ~PGLibLoader();

    func_map_t *load();
    int unload();

private:

    int load_libpqtypes(func_map_t *);
    int load_libpq(func_map_t *);
    int unload_libpqtypes();
    int unload_libpq();

};


class SLLibLoader : public LibLoader {
private:

    lt_dlhandle h_libsqlite;

public:

    SLLibLoader();
    ~SLLibLoader();

    func_map_t *load();
    int unload();
    
private:

    int load_libsqlite(func_map_t *);
    int unload_libsqlite();
};

} // namespace vtapi

#endif	

