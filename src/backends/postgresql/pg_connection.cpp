
#include <cstdarg>
#include <vtapi/common/global.h>
#include "pg_connection.h"

#define PG_FORMAT 1   // postgres data transfer format: 0=text, 1=binary

#define PGCONN ((PGconn *)_conn)

using namespace std;

namespace vtapi {


bool PGConnection::connect()
{
    bool retval = true;

    do {
        VTLOG_DEBUG("Connecting to DB... " + _connection_info);
        
        _conn = PQconnectdb(_connection_info.c_str());
        if (!(retval = isConnected())) {
            const char *errmsgc = PQerrorMessage(PGCONN);
            if (errmsgc)
                VTLOG_ERROR(errmsgc);
            else
                VTLOG_ERROR("Failed to connect to database (" + _connection_info + ")");
            break;
        }

        // set NOTICE displaying function
        PQsetNoticeProcessor(PGCONN, PGConnection::noticeProcessor, NULL);

        if (!(retval = PQinitTypes(PGCONN))) {
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

        PQclearTypes(PGCONN);
        PQfinish(PGCONN);
        _conn = NULL;
    }
}

bool PGConnection::isConnected () const
{
    return (PGCONN && PQstatus(PGCONN) == CONNECTION_OK);
}

bool PGConnection::execute(const string& query, void *param)
{
    PGresult    *pgres  = NULL;
    bool        retval  = true;

    VTLOG_DEBUG(query);

    _error_message.clear();

    if (param)
        pgres = PQparamExec(PGCONN, (PGparam *) param, query.c_str(), PG_FORMAT);
    else
        pgres = PQexecf(PGCONN, query.c_str(), PG_FORMAT);

    if (!pgres) {
        _error_message = PQgeterror();
        VTLOG_ERROR(_error_message);
        retval = false;
    }
    else {
        int result = PQresultStatus(pgres);
        if (result == PGRES_NONFATAL_ERROR) {
            _error_message = PQerrorMessage(PGCONN);
            VTLOG_DEBUG(_error_message);
        }
        else if (result != PGRES_TUPLES_OK && result != PGRES_COMMAND_OK) {
            _error_message = PQerrorMessage(PGCONN);
            VTLOG_ERROR(_error_message);
            retval = false;
        }
        PQclear(pgres);
    }

    return retval;
}

int PGConnection::fetch(const string& query, void *param, ResultSet &resultSet)
{
    int retval  = -1;
    PGresult *pgres  = NULL;

    VTLOG_DEBUG(query);

    _error_message.clear();

    if (param)
        pgres = PQparamExec(PGCONN, (PGparam *) param, query.c_str(), PG_FORMAT);
    else
        pgres = PQexecf(PGCONN, query.c_str(), PG_FORMAT);

    resultSet.newResult((void *) pgres);

    if (!pgres) {
        _error_message = PQgeterror();
        VTLOG_ERROR(_error_message);
        retval = -1;
    }
    else {
        if (PQresultStatus(pgres) == PGRES_TUPLES_OK) {
            retval = PQntuples(pgres);
        }
        else if (PQresultStatus(pgres) == PGRES_COMMAND_OK) {
            retval = 0;
        }
        else {
            _error_message = PQerrorMessage(PGCONN);
            VTLOG_ERROR(_error_message);
            retval = -1;
        }
    }

    return retval;
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
            {"public.seqtype", enum_put, enum_get },
            {"public.inouttype", enum_put, enum_get },
            {"public.pstatus", enum_put, enum_get }
            //{"public.paramtype", enum_put, enum_get}
        };
        retval = PQregisterTypes(PGCONN,
                                 PQT_USERDEFINED,
                                 types_userdef,
                                 sizeof (types_userdef) / sizeof (PGregisterType),
                                 0);
        if (!retval) {
            VTLOG_ERROR(PQgeterror());
            break;
        }

        // register composites
        PGregisterType types_comp[] = {
            {"public.cvmat", NULL, NULL },
            {"public.vtevent", NULL, NULL },
            {"public.pstate", NULL, NULL }
        };
        retval = PQregisterTypes(PGCONN,
                                 PQT_COMPOSITE,
                                 types_comp,
                                 sizeof (types_comp) / sizeof (PGregisterType),
                                 0);
        if (!retval) {
            VTLOG_ERROR(PQgeterror());
            break;
        }

        // now load types definitions

        // select type info from catalog
        pgres = PQexecf(PGCONN,
                        "SELECT oid, typname, typcategory, typlen, typelem from pg_catalog.pg_type",
                        PG_FORMAT);
        if (!pgres) {
            VTLOG_ERROR(PQgeterror());
            retval = false;
            break;
        }

        // go through all types and fill dbtypes map
        map<int, int> oid_map;    // oid of array -> oid of elem
        int ntuples = PQntuples(pgres);
        for (int i = 0; i < ntuples; i++) {
            DatabaseTypes::TypeDefinition def;
            PGint4 oid;
            PGtext name;
            PGchar cat;
            PGint2 length;
            PGint4 oid_elem;

            PQgetf(pgres, i, "%oid %name %char %int2 %oid",
                       0, &oid, 1, &name, 2, &cat, 3, &length, 4, &oid_elem);

            def._name = name;

            // get type category and flags
            getTypeCategoryFlags(cat, def._name, def._category, def._flags);
            if (def._flags & DatabaseTypes::FLAG_ARRAY)
                oid_map.insert(pair<int, int>(oid, oid_elem));
            else
                def._length = length;
            _dbtypes.insert(oid, std::move(def));
        }

        // postprocess array types - set category/length of elements
        for (const auto & oid_item : oid_map) {
            DatabaseTypes::TypeDefinition &def_arr = _dbtypes.type(oid_item.first);
            DatabaseTypes::TypeDefinition &def_elem = _dbtypes.type(oid_item.second);
            def_arr._name = def_elem._name;
            def_arr._flags |= def_elem._flags;
            def_arr._length = def_elem._length;
        }

    } while (0);

    if (pgres) PQclear(pgres);

    return retval;
}

void PGConnection::getTypeCategoryFlags(char c, const std::string &name,
                                        short int & category, char & flags) const
{
    switch (c)
    {
    case 'A':   // array
    {
        flags |= DatabaseTypes::FLAG_ARRAY;
        break;
    }
    case 'B':   // boolean
    {
        if (name == "bool")
            category = DatabaseTypes::CATEGORY_BOOLEAN;
        break;
    }
    case 'C':   // composite
    {
        if (name == "cvmat") {
            category = DatabaseTypes::CATEGORY_UD_CVMAT;
            flags |= DatabaseTypes::FLAG_USERDEFINED;
        }
        else if (name == "vtevent") {
            category = DatabaseTypes::CATEGORY_UD_EVENT;
            flags |= DatabaseTypes::FLAG_USERDEFINED;
        }
        else if (name == "pstate") {
            category = DatabaseTypes::CATEGORY_UD_PSTATE;
            flags |= DatabaseTypes::FLAG_USERDEFINED;
        }
        break;
    }
    case 'D':   // date/time
    {
        if (name == "timestamp") {
            category = DatabaseTypes::CATEGORY_TIMESTAMP;
        }
        break;
    }
    case 'E':   // enum
    {
        if (name == "seqtype") {
            category = DatabaseTypes::CATEGORY_UD_SEQTYPE;
            flags |= DatabaseTypes::FLAG_USERDEFINED;
        }
        else if (name == "inouttype") {
            category = DatabaseTypes::CATEGORY_UD_INOUTTYPE;
            flags |= DatabaseTypes::FLAG_USERDEFINED;
        }
        else if (name == "pstatus") {
            category = DatabaseTypes::CATEGORY_UD_PSTATUS;
            flags |= DatabaseTypes::FLAG_USERDEFINED;
        }
        break;
    }
    case 'G':   // geometric
    {
        if (name == "point") {
            category = DatabaseTypes::CATEGORY_GEO_POINT;
            flags |= DatabaseTypes::FLAG_GEOMETRIC;
        }
        else if (name == "lseg") {
            category = DatabaseTypes::CATEGORY_GEO_LSEG;
            flags |= DatabaseTypes::FLAG_GEOMETRIC;
        }
        else if (name == "path") {
            category = DatabaseTypes::CATEGORY_GEO_PATH;
            flags |= DatabaseTypes::FLAG_GEOMETRIC;
        }
        else if (name == "box") {
            category = DatabaseTypes::CATEGORY_GEO_BOX;
            flags |= DatabaseTypes::FLAG_GEOMETRIC;
        }
        else if (name == "polygon") {
            category = DatabaseTypes::CATEGORY_GEO_POLYGON;
            flags |= DatabaseTypes::FLAG_GEOMETRIC;
        }
        else if (name == "line") {
            category = DatabaseTypes::CATEGORY_GEO_LINE;
            flags |= DatabaseTypes::FLAG_GEOMETRIC;
        }
        else if (name == "circle") {
            category = DatabaseTypes::CATEGORY_GEO_CIRCLE;
            flags |= DatabaseTypes::FLAG_GEOMETRIC;
        }
        break;
    }
    case 'N':   // numeric
    {
        if (strncmp(name.c_str(), "int", 3) == 0) {
            category = DatabaseTypes::CATEGORY_INT;
            flags |= DatabaseTypes::FLAG_NUMERIC;
        }
        else if (strncmp(name.c_str(), "float", 5) == 0) {
            category = DatabaseTypes::CATEGORY_FLOAT;
            flags |= DatabaseTypes::FLAG_NUMERIC;
        }
        else if (name == "numeric") {
            flags |= DatabaseTypes::FLAG_NUMERIC;
        }
        else if (name == "regtype") {
            category = DatabaseTypes::CATEGORY_REF_TYPE;
            flags |= DatabaseTypes::FLAG_REFTYPE;
        }
        else if (name == "regclass") {
            category = DatabaseTypes::CATEGORY_REF_CLASS;
            flags |= DatabaseTypes::FLAG_REFTYPE;
        }
        break;
    }
    case 'S':   // string
    {
        if (name == "char"||
            name == "varchar" ||
            name == "name" ||
            name == "text")
        {
            category = DatabaseTypes::CATEGORY_STRING;
        }
        break;
    }
    case 'U':   // types with user-defined input/output
    {
        if (name == "bytea") {
            category = DatabaseTypes::CATEGORY_BLOB;
        }
        else if (name == "geometry") {
            category = DatabaseTypes::CATEGORY_GEO_GEOMETRY;
            flags |= DatabaseTypes::FLAG_GEOMETRIC;
        }
        break;
    }
    default:    // unknown
    {
        break;
    }
    }
}

void PGConnection::noticeProcessor(void *arg, const char *message)
{
    VTLOG_WARNING(message);
}

int PGConnection::enum_get(PGtypeArgs *args)
{
    char *val   = PQgetvalue(args->get.result, args->get.tup_num, args->get.field_num);
    int len     = PQgetlength(args->get.result, args->get.tup_num, args->get.field_num);

    if (val) {
        char **result = va_arg(args->ap, char **);
        *result = (char *) PQresultAlloc((PGresult *) args->get.result,
                                         (len + 1) * sizeof (char));
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


}
