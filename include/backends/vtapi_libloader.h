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

#define CALL_PQT(PQT_FUNCTION) ((PQT_ ## PQT_FUNCTION) (*FUNC_MAP)[string("PQT_" #PQT_FUNCTION)])

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

#define CALL_PQ(PQ_FUNCTION) ((PQ_ ## PQ_FUNCTION) (*FUNC_MAP)[string("PQ_" #PQ_FUNCTION)])

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

#define CALL_SL(SL_FUNCTION) ((SL_ ## SL_FUNCTION) (*FUNC_MAP)[string("SL_" #SL_FUNCTION)])

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

