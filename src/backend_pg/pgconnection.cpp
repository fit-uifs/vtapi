
#include <common/vtapi_global.h>
#include <backends/vtapi_connection.h>

#if HAVE_POSTGRESQL

using std::string;

using namespace vtapi;

PGConnection *PGConnection::glob = NULL;


PGConnection::PGConnection(const PGBackendBase &base, const std::string& connectionInfo) :
    Connection (connectionInfo),
    PGBackendBase(base)
{
    thisClass   = "PGConnection";
    conn        = NULL;
    glob        = this;
}

PGConnection::~PGConnection() {
    disconnect();
    pqt.PQclearTypes(conn);
    if (glob == this) glob == NULL;
}

bool PGConnection::connect (const string& connectionInfo) {
    bool retval = VT_OK;

    do {
        if (!connectionInfo.empty()) {
            this->connInfo = connectionInfo;
        }
        
        conn = pg.PQconnectdb(this->connInfo.c_str());
        retval = isConnected();
        if (!retval) {
            const char *errmsgc = pg.PQerrorMessage(conn);
            string errmsg = errmsgc ? errmsgc : "The connection couldn't have been established.";
            logger->error(122, errmsg, thisClass+"::connect()");
            break;
        }
        
        retval = pqt.PQinitTypes(conn);
        if (!retval) {
            logger->error(123, "failed to init libpqtypes", thisClass + "::connect()");
            break;
        }
        
        retval = loadDBTypes();
        if (!retval) {
            logger->error(124, "failed to load database types", thisClass + "::connect()");
            break;
        }
    } while(0);

    return retval;
};

bool PGConnection::reconnect (const string& connectionInfo) {
    if (!connectionInfo.empty()) connInfo = connectionInfo;
    disconnect();
    return connect(connInfo);
}

void PGConnection::disconnect () {
    if (isConnected()) {
        pg.PQfinish(conn);
        conn = NULL;
    }
}

bool PGConnection::isConnected () {
    return (conn && pg.PQstatus(conn) == CONNECTION_OK);
}

bool PGConnection::execute(const string& query, void *param) {
    PGresult    *pgres  = NULL;
    bool        retval  = VT_OK;

    errorMessage.clear();

    if (param) {
        pgres = pqt.PQparamExec(conn, (PGparam *)param, query.c_str(), PG_FORMAT);
    }
    else {
        pgres = pqt.PQexecf(conn, query.c_str(), PG_FORMAT);
    }

    if (!pgres) {
        errorMessage = string(pg.PQerrorMessage(conn));
        retval = VT_FAIL;
    }
    else {
        int result = pg.PQresultStatus(pgres);
        if (result != PGRES_TUPLES_OK && result != PGRES_COMMAND_OK) {
            logger->warning(2012, "Apocalypse warning", thisClass+"::execute()");
            errorMessage = string(pg.PQerrorMessage(conn));
            retval = VT_FAIL;
        }
        pg.PQclear(pgres);
    }

    return retval;
}

int PGConnection::fetch(const string& query, void *param, ResultSet *resultSet) {
    int         retval  = ER_FAIL;
    PGresult    *pgres  = NULL;
    
    errorMessage.clear();
    
    if (param && (PGparam *)param) {
        pgres = pqt.PQparamExec(conn, (PGparam *)param, query.c_str(), PG_FORMAT);
    }
    else {
        pgres = pqt.PQexecf(conn, query.c_str(), PG_FORMAT);
    }

    resultSet->newResult((void *) pgres);

    if (!pgres) {
        errorMessage = string(pg.PQerrorMessage(conn));
        retval = ER_WRONG_QUERY;
    }
    else {
        if (pg.PQresultStatus(pgres) == PGRES_TUPLES_OK) {
            retval = pg.PQntuples(pgres);
        }
        else if (pg.PQresultStatus(pgres) == PGRES_COMMAND_OK) {
            retval = 0;
        }
        else {
            logger->warning(2012, "Apocalypse warning", thisClass+"::fetch()");
            errorMessage = string(pg.PQerrorMessage(conn));
            retval = ER_WRONG_QUERY;
        }
    }

    return retval;
}

void* PGConnection::getConnectionObject() {
    return (void *) this->conn;
}

bool PGConnection::loadDBTypes()
{
    bool retval = VT_OK;
    PGresult *pgres = NULL;

    do {
        // general types registered at all times
        PGregisterType types_userdef[] = {
#if HAVE_POSTGIS
            {"geometry", geometry_put, geometry_get},
#endif
            {"public.seqtype", enum_put, enum_get},
            {"public.inouttype", enum_put, enum_get}//,
            //{"public.permissions", enum_put, enum_get}
        };
        retval = pqt.PQregisterTypes(conn, PQT_USERDEFINED, types_userdef, sizeof (types_userdef) / sizeof (PGregisterType), 0);
        if (!retval) {
            logger->warning(666, pqt.PQgeterror(), thisClass + "::loadTypes()");
            retval = VT_FAIL;
            break;
        }

        // register composites
        PGregisterType types_comp[] = {
#if HAVE_OPENCV
            {"public.cvmat", NULL, NULL},
#endif
            {"public.vtevent", NULL, NULL}
        };
        retval = pqt.PQregisterTypes(conn, PQT_COMPOSITE, types_comp, sizeof (types_comp) / sizeof (PGregisterType), 0);
        if (!retval) {
            logger->warning(666, pqt.PQgeterror(), thisClass + "::loadTypes()");
            retval = VT_FAIL;
            break;
        }
        
        // now load types info

        pgres = pqt.PQexecf(conn, "SELECT oid, typname, typcategory, typlen, typelem from pg_catalog.pg_type", PG_FORMAT);
        if (!pgres) {
            retval = VT_FAIL;
            break;
        }
        
        std::map<int,int> oid_array;    // oid of array -> oid of elem
        int ntuples = pg.PQntuples(pgres);
        for (int i = 0; i < ntuples; i++) {
            VTAPI_DBTYPE_DEFINITION_T def;
            PGint4 oid;
            PGtext name;
            PGchar cat;
            PGint2 length;
            PGint4 oid_elem;
            
            pqt.PQgetf(pgres, i, "%oid %name %char %int2 %oid",
                0, &oid, 1, &name, 2, &cat, 3, &length, 4, &oid_elem);

            def.name = name;
            def.type = 0;
            
            short catFlags = typeCategoryCharToType(cat);
            if (VTAPI_DBTYPE_HASFLAG(catFlags, TYPE_FLAG_ARRAY)) {
                VTAPI_DBTYPE_SETFLAG(def.type, TYPE_FLAG_ARRAY);
                oid_array.insert(std::pair<int,int>(oid, oid_elem));
            }
            else {
                VTAPI_DBTYPE_SETCATEGORYFLAGS(def.type, catFlags);
                VTAPI_DBTYPE_SETLENGTH(def.type, length);
            }
            dbtypes.insert(VTAPI_DBTYPES_PAIR(oid, def));
        }

        for (std::map<int, int>::iterator it = oid_array.begin(); it != oid_array.end(); it++) {
            VTAPI_DBTYPE_DEFINITION_T &def_arr = dbtypes[(*it).first];
            VTAPI_DBTYPE_DEFINITION_T &def_elem = dbtypes[(*it).second];
            VTAPI_DBTYPE_SETCATEGORYFLAGS(def_arr.type, VTAPI_DBTYPE_GETCATEGORYFLAGS(def_elem.type));
            VTAPI_DBTYPE_SETLENGTH(def_arr.type, VTAPI_DBTYPE_GETLENGTH(def_elem.type));
        }
        // TODO: load reference types?
    } while (0);

    if (pgres) pg.PQclear(pgres);
    
    return retval;
}

short PGConnection::typeCategoryCharToType(char c)
{
    short ret = 0;
            
    switch (c) {
        case 'A': VTAPI_DBTYPE_SETFLAG(ret, TYPE_FLAG_ARRAY); break;
        case 'B': VTAPI_DBTYPE_SETCATEGORY(ret,TYPE_BOOLEAN); break;
        case 'C': VTAPI_DBTYPE_SETCATEGORY(ret,TYPE_COMPOSITE); break;
        case 'D': VTAPI_DBTYPE_SETCATEGORY(ret,TYPE_DATE); break;
        case 'E': VTAPI_DBTYPE_SETCATEGORY(ret,TYPE_ENUM); break;
        case 'G': VTAPI_DBTYPE_SETCATEGORY(ret,TYPE_GEOMETRIC); break;
        case 'N': VTAPI_DBTYPE_SETCATEGORY(ret,TYPE_NUMERIC); break;
        case 'S': VTAPI_DBTYPE_SETCATEGORY(ret,TYPE_STRING); break;
        case 'U': {
            VTAPI_DBTYPE_SETFLAG(ret, TYPE_FLAG_USERDEFINED);
            break;
        }
        default: VTAPI_DBTYPE_SETCATEGORY(ret,TYPE_UNDEFINED); break;
    }  
    
    return ret;
}

int PGConnection::enum_get(PGtypeArgs *args)
{
    return glob ? glob->enum_get_helper(args) : -1;
}

int PGConnection::enum_put(PGtypeArgs *args)
{
    return glob ? glob->enum_put_helper(args) : -1;
}

int PGConnection::enum_get_helper(PGtypeArgs *args)
{
    char *val   = pg.PQgetvalue(args->get.result, args->get.tup_num, args->get.field_num);
    int len     = pg.PQgetlength(args->get.result, args->get.tup_num, args->get.field_num);

    if (val) {
        char **result = va_arg(args->ap, char **);
        *result = (char *) pg.PQresultAlloc((PGresult *) args->get.result, (len + 1) * sizeof (char));
        if (*result) {
            std::copy(val, val + len, *result);
            (*result)[len] = '\0';
        }
        else {
            len = -1;
        }
    }
    
    return len;
}

int PGConnection::enum_put_helper(PGtypeArgs *args)
{
    char *val   = va_arg(args->ap, char *);
    int len     = 0;
    
    if (val) {
        len = strlen(val);
        if (args->put.expandBuffer(args, len) >= 0 -1) {
            std::copy(val, val + len, args->put.out);
        }
        else {
            len = -1;
        }
    }
    
    return len;
}

#if HAVE_POSTGIS
int PGConnection::geometry_get(PGtypeArgs *args) {
    return glob ? glob.geometry_get_helper(args) : -1;
}

int PGConnection::geometry_put(PGtypeArgs *args) {
    return glob ? glob.geometry_put_helper(args) : -1;
}

int PGConnection::geometry_get_helper(PGtypeArgs *args) {
    //TODO: unimplemented
    return -1;
}

int PGConnection::geometry_put_helper(PGtypeArgs *args) {
    //TODO: unimplemented
    return -1;
}
#endif

#endif
