
#include <common/vtapi_global.h>
#include <backends/vtapi_connection.h>

#if VTAPI_HAVE_POSTGRESQL

using namespace std;

namespace vtapi {


POSTGRES_INTERFACE *g_pg;

PGConnection::PGConnection(const PGBackendBase &base, const string& connectionInfo)
    : Connection (connectionInfo), PGBackendBase(base)
{
    _conn = NULL;
    g_pg = &_pg;
}

PGConnection::~PGConnection()
{
    disconnect();
    _pqt.PQclearTypes(_conn);
    if (g_pg == &_pg) g_pg == NULL;
}

bool PGConnection::connect()
{
    bool retval = true;

    do {
        VTLOG_DEBUG("Connecting to DB... " + _connInfo);
        
        _conn = _pg.PQconnectdb(_connInfo.c_str());
        if (!(retval = isConnected())) {
            const char *errmsgc = _pg.PQerrorMessage(_conn);
            if (errmsgc)
                VTLOG_ERROR(errmsgc);
            else
                VTLOG_ERROR("Failed to connect to database (" + _connInfo + ")");
            break;
        }

        if (!(retval = _pqt.PQinitTypes(_conn))) {
            VTLOG_ERROR("Failed to init libpqtypes");
            break;
        }

        if (!(retval = loadDBTypes())) {
            VTLOG_ERROR("Failed to load database types");
            break;
        }
    } while (0);

    return retval;
};

void PGConnection::disconnect ()
{
    if (isConnected()) {
        VTLOG_DEBUG("Disconnecting DB...");

        _pg.PQfinish(_conn);
        _conn = NULL;
    }
}

bool PGConnection::isConnected ()
{
    return (_conn && _pg.PQstatus(_conn) == CONNECTION_OK);
}

bool PGConnection::execute(const string& query, void *param)
{
    PGresult    *pgres  = NULL;
    bool        retval  = true;

    VTLOG_DEBUG(query);

    _errorMessage.clear();

    if (param)
        pgres = _pqt.PQparamExec(_conn, (PGparam *) param, query.c_str(), PG_FORMAT);
    else
        pgres = _pqt.PQexecf(_conn, query.c_str(), PG_FORMAT);

    if (!pgres) {
        _errorMessage = _pqt.PQgeterror();
        VTLOG_ERROR(_errorMessage);
        retval = false;
    }
    else {
        int result = _pg.PQresultStatus(pgres);
        if (result != PGRES_TUPLES_OK && result != PGRES_COMMAND_OK) {
            _errorMessage = _pg.PQerrorMessage(_conn);
            VTLOG_ERROR(_errorMessage);
            retval = false;
        }
        _pg.PQclear(pgres);
    }

    return retval;
}

int PGConnection::fetch(const string& query, void *param, ResultSet *resultSet)
{
    int retval  = -1;
    PGresult *pgres  = NULL;

    VTLOG_DEBUG(query);

    _errorMessage.clear();

    if (param)
        pgres = _pqt.PQparamExec(_conn, (PGparam *) param, query.c_str(), PG_FORMAT);
    else
        pgres = _pqt.PQexecf(_conn, query.c_str(), PG_FORMAT);

    resultSet->newResult((void *) pgres);

    if (!pgres) {
        _errorMessage = _pqt.PQgeterror();
        VTLOG_ERROR(_errorMessage);
        retval = -1;
    }
    else {
        if (_pg.PQresultStatus(pgres) == PGRES_TUPLES_OK) {
            retval = _pg.PQntuples(pgres);
        }
        else if (_pg.PQresultStatus(pgres) == PGRES_COMMAND_OK) {
            retval = 0;
        }
        else {
            _errorMessage = _pg.PQerrorMessage(_conn);
            VTLOG_ERROR(_errorMessage);
            retval = -1;
        }
    }

    return retval;
}

void* PGConnection::getConnectionObject()
{
    return (void *)_conn;
}

bool PGConnection::loadDBTypes()
{
    bool retval = true;
    PGresult *pgres = NULL;

    do {
        VTLOG_DEBUG("Loading DB types...");

        // register types for use by libpqtypes

        // general types registered at all times
        PGregisterType types_userdef[] = {
#if VTAPI_HAVE_POSTGIS
            {"geometry", geometry_put, geometry_get },
#endif
            {"public.seqtype", enum_put, enum_get },
            {"public.inouttype", enum_put, enum_get },
            {"public.pstatus", enum_put, enum_get }
            //{"public.paramtype", enum_put, enum_get}
        };
        retval = _pqt.PQregisterTypes(_conn,
                                     PQT_USERDEFINED,
                                     types_userdef,
                                     sizeof (types_userdef) / sizeof (PGregisterType),
                                     0);
        if (!retval) {
            VTLOG_ERROR(_pqt.PQgeterror());
            break;
        }

        // register composites
        PGregisterType types_comp[] = {
#if VTAPI_HAVE_OPENCV
            {"public.cvmat", NULL, NULL },
#endif
            {"public.vtevent", NULL, NULL },
            {"public.pstate", NULL, NULL }
        };
        retval = _pqt.PQregisterTypes(_conn,
                                     PQT_COMPOSITE,
                                     types_comp,
                                     sizeof (types_comp) / sizeof (PGregisterType),
                                     0);
        if (!retval) {
            VTLOG_ERROR(_pqt.PQgeterror());
            break;
        }

        // now load types definitions

        // select type info from catalog
        pgres = _pqt.PQexecf(_conn,
                            "SELECT oid, typname, typcategory, typlen, typelem from pg_catalog.pg_type",
                             PG_FORMAT);
        if (!pgres) {
            VTLOG_ERROR(_pqt.PQgeterror());
            retval = false;
            break;
        }

        // go through all types and fill dbtypes map
        map<int, int> oid_array;    // oid of array -> oid of elem
        int ntuples = _pg.PQntuples(pgres);
        for (int i = 0; i < ntuples; i++) {
            DBTYPE_DEFINITION_T def;
            PGint4 oid;
            PGtext name;
            PGchar cat;
            PGint2 length;
            PGint4 oid_elem;

            _pqt.PQgetf(pgres, i, "%oid %name %char %int2 %oid",
                       0, &oid, 1, &name, 2, &cat, 3, &length, 4, &oid_elem);

            def.name = name;
            def.type = 0;

            // get type category and flags
            short catFlags = getTypeCategoryFlags(cat, def.name);
            if (catFlags) {
                DBTYPE_SETFLAGS(def.type, catFlags);
                if (DBTYPE_HASFLAG(catFlags, DBTYPE_FLAG_ARRAY)) {
                    oid_array.insert(pair<int, int>(oid, oid_elem));
                }
                else {
                    DBTYPE_SETCATEGORY(def.type, catFlags);
                    DBTYPE_SETLENGTH(def.type, length);
                }
                _dbtypes.insert(DBTYPES_PAIR(oid, def));
            }
        }

        // postprocess array types - set category/length of elements
        for (map<int, int>::iterator it = oid_array.begin(); it != oid_array.end(); it++) {
            DBTYPES_MAP_IT itArr = _dbtypes.find((*it).first);
            DBTYPES_MAP_IT itElem = _dbtypes.find((*it).second);
            if (itArr != _dbtypes.end() && itElem != _dbtypes.end()) {
                DBTYPE_SETCATEGORY((*itArr).second.type, (*itElem).second.type);
                DBTYPE_SETFLAGS((*itArr).second.type, (*itElem).second.type);
                DBTYPE_SETLENGTH((*itArr).second.type, DBTYPE_GETLENGTH((*itElem).second.type));
            }
        }

    } while (0);

    if (pgres) _pg.PQclear(pgres);

    return retval;
}

short PGConnection::getTypeCategoryFlags(char c, const string &name)
{
    short ret = 0;

    switch (c)
    {
    case 'A':   // array
    {
        DBTYPE_SETFLAGS(ret, DBTYPE_FLAG_ARRAY);
        break;
    }
    case 'B':   // boolean
    {
        if (name.compare("bool") == 0) {
            DBTYPE_SETCATEGORY(ret, DBTYPE_BOOLEAN);
        }
        break;
    }
    case 'C':   // composite
    {
        if (name.compare("cvmat") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_UD_CVMAT | DBTYPE_FLAG_USERDEFINED);
        }
        else if (name.compare("vtevent") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_UD_EVENT | DBTYPE_FLAG_USERDEFINED);
        }
        else if (name.compare("pstate") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_UD_PSTATE | DBTYPE_FLAG_USERDEFINED);
        }
        break;
    }
    case 'D':   // date/time
    {
        if (name.compare("timestamp") == 0) {
            DBTYPE_SETCATEGORY(ret, DBTYPE_TIMESTAMP);
        }
        break;
    }
    case 'E':   // enum
    {
        if (name.compare("seqtype") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_UD_SEQTYPE | DBTYPE_FLAG_USERDEFINED);
        }
        else if (name.compare("inouttype") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_UD_INOUTTYPE | DBTYPE_FLAG_USERDEFINED);
        }
        else if (name.compare("pstatus") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_UD_PSTATUS | DBTYPE_FLAG_USERDEFINED);
        }
        break;
    }
    case 'G':   // geometric
    {
        if (name.compare("point") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_GEO_POINT | DBTYPE_FLAG_GEOMETRIC);
        }
        else if (name.compare("lseg") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_GEO_LSEG | DBTYPE_FLAG_GEOMETRIC);
        }
        else if (name.compare("path") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_GEO_PATH | DBTYPE_FLAG_GEOMETRIC);
        }
        else if (name.compare("box") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_GEO_BOX | DBTYPE_FLAG_GEOMETRIC);
        }
        else if (name.compare("polygon") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_GEO_POLYGON | DBTYPE_FLAG_GEOMETRIC);
        }
        else if (name.compare("line") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_GEO_LINE | DBTYPE_FLAG_GEOMETRIC);
        }
        else if (name.compare("circle") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_GEO_CIRCLE | DBTYPE_FLAG_GEOMETRIC);
        }
        break;
    }
    case 'N':   // numeric
    {
        if (strncmp(name.c_str(), "int", 3) == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_INT | DBTYPE_FLAG_NUMERIC);
        }
        else if (strncmp(name.c_str(), "float", 5) == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_FLOAT | DBTYPE_FLAG_NUMERIC);
        }
        else if (name.compare("numeric") == 0) {
            DBTYPE_SETFLAGS(ret, DBTYPE_FLAG_NUMERIC);
        }
        else if (name.compare("regtype") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_REF_TYPE | DBTYPE_FLAG_REFTYPE);
        }
        else if (name.compare("regclass") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_REF_CLASS | DBTYPE_FLAG_REFTYPE);
        }
        break;
    }
    case 'S':   // string
    {
        if (name.compare("char") == 0 ||
        name.compare("varchar") == 0 ||
        name.compare("name") == 0 ||
        name.compare("text") == 0) {
            DBTYPE_SETCATEGORY(ret, DBTYPE_STRING);
        }
        break;
    }
    case 'U':   // types with user-defined input/output
    {
        if (name.compare("bytea") == 0) {
            DBTYPE_SETCATEGORY(ret, DBTYPE_BLOB);
        }
        else if (name.compare("geometry") == 0) {
            DBTYPE_SETCATEGORYFLAGS(ret, DBTYPE_GEO_GEOMETRY | DBTYPE_FLAG_GEOMETRIC);
        }
        break;
    }
    default:    // unknown
    {
        break;
    }
    }

    return ret;
}

int PGConnection::enum_get(PGtypeArgs *args)
{
    char *val   = g_pg->PQgetvalue(args->get.result, args->get.tup_num, args->get.field_num);
    int len     = g_pg->PQgetlength(args->get.result, args->get.tup_num, args->get.field_num);

    if (val) {
        char **result = va_arg(args->ap, char **);
        *result = (char *) g_pg->PQresultAlloc((PGresult *) args->get.result, (len + 1) * sizeof (char));
        if (*result) {
            copy(val, val + len, *result);
            (*result)[len] = '\0';
        }
        else {
            len = -1;
        }
    }

    return len;
}

int PGConnection::enum_put(PGtypeArgs *args)
{
    char *val   = va_arg(args->ap, char *);
    int len     = 0;

    if (val) {
        len = strlen(val);
        if (args->put.expandBuffer(args, len) >= 0 - 1) {
            copy(val, val + len, args->put.out);
        }
        else {
            len = -1;
        }
    }

    return len;
}


#endif

}
