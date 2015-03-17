
#include <common/vtapi_global.h>
#include <common/vtapi_misc.h>
#include <common/vtapi_serialize.h>
#include <backends/vtapi_typemanager.h>


#if HAVE_POSTGRESQL

// postgres data transfer format: 0=text, 1=binary
#define PG_FORMAT           1

using std::string;
using std::stringstream;
using std::vector;
using std::pair;

using namespace vtapi;

static PGTypeManager *g_typeManager = NULL;

static int pg_enum_put (PGtypeArgs *args) {
    return g_typeManager ? g_typeManager->enum_put(args) : 0;
}

static int pg_enum_get (PGtypeArgs *args) {
    return g_typeManager ? g_typeManager->enum_get(args) : 0;
}

PGTypeManager::PGTypeManager(fmap_t *fmap, Connection *connection, Logger *logger, string& schema)
: TypeManager(fmap, connection, logger, schema) {
    thisClass = "PGTypeManager";
    this->loadTypes();
    this->registerTypes();
    if (!g_typeManager) g_typeManager = this;
}

PGTypeManager::~PGTypeManager() {
    if (g_typeManager == this) g_typeManager = NULL;
}

bool PGTypeManager::registerTypes () {
    bool retreg = VT_OK, retval = VT_OK;

    // general types registered at all times
    PGregisterType types_userdef[] =
    {
        {"public.seqtype", pg_enum_put, pg_enum_get},
        {"public.inouttype", pg_enum_put, pg_enum_get}//,
        //{"public.permissions", pg_enum_put, pg_enum_get}, // change 2 to 3 in next command
    };
    retreg = CALL_PQT(fmap, PQregisterTypes, (PGconn *)connection->getConnectionObject(), PQT_USERDEFINED, types_userdef, 2, 0);
    if (!retreg) {
        logger->warning(666, CALL_PQT(fmap, PQgeterror), thisClass+"::registerTypes()");
        retval = VT_FAIL;
    }

    // PostGIS special types
#if HAVE_POSTGIS
    PGregisterType typespg_userdef[] = {
        {"geometry", geometry_put, geometry_get}
    };
    retreg = CALL_PQT(fmap, PQregisterTypes, (PGconn *)connection->getConnectionObject(), PQT_USERDEFINED, typespg_userdef, 1, 0);
    if (!retreg) {
        logger->warning(666, CALL_PQT(fmap, PQgeterror), thisClass+"::registerTypes()");
        retval = VT_FAIL;
    }
#endif

    // OpenCV special types
#if HAVE_OPENCV
    PGregisterType typescv_comp[] = {
        {"public.cvmat", NULL, NULL},
        {"public.vtevent", NULL, NULL},
    };
    retreg = CALL_PQT(fmap, PQregisterTypes, (PGconn *)connection->getConnectionObject(), PQT_COMPOSITE, typescv_comp, 2, 0);
    if (!retreg) {
        logger->warning(666, CALL_PQT(fmap, PQgeterror), thisClass+"::registerTypes()");
        retval = VT_FAIL;
    }
#endif

    return retval;
}

bool PGTypeManager::loadTypes() {
    bool retval = VT_OK;
    PGresult * pgres;

    pgres = CALL_PQT(fmap, PQexecf, (PGconn *)connection->getConnectionObject(),
            "SELECT oid, typname, typcategory, typlen, typelem from pg_catalog.pg_type", PG_FORMAT);
    if (!pgres) {
        retval = VT_FAIL;
    }
    else {
        vector<int> oid_array;
        int oid_ix = 0;
        type_metadata_t type_metadata;
        PGint4 oid;
        PGtext name;
        PGchar category_char;
        PGint2 length;
        PGint4 oid_elem;

        for (int i = 0; i < CALL_PQ(fmap, PQntuples, pgres); i++) {
            CALL_PQT(fmap, PQgetf, pgres, i, "%oid %name %char %int2 %oid",
                0, &oid, 1, &name, 2, &category_char, 3, &length, 4, &oid_elem);
            type_metadata.category      = mapCategory(category_char);
            type_metadata.length        = length;
            type_metadata.category_elem = TYPE_UNDEFINED;
            type_metadata.length_elem   = -1;
            types_map.insert(std::make_pair(string(name), type_metadata));
            oid_map.insert(std::make_pair(oid, name));
            if (type_metadata.category == TYPE_ARRAY) {
                oid_array.push_back(oid_elem);
            }
        }

        for (types_map_t::iterator it = types_map.begin(); it != types_map.end(); it++) {
            if ((*it).second.category == TYPE_ARRAY) {                
                (*it).second.category_elem  = types_map[toTypname(oid_array[oid_ix])].category;
                (*it).second.length_elem    = types_map[toTypname(oid_array[oid_ix])].length;
            }
            oid_ix++;
        }
        // TODO? load reference types
        CALL_PQ(fmap, PQclear, pgres);
    }

    return retval;
}

type_category_t PGTypeManager::mapCategory(char category_char) {
    switch (category_char) {
        case 'A': return TYPE_ARRAY;
        case 'B': return TYPE_BOOLEAN;
        case 'C': return TYPE_COMPOSITE;
        case 'D': return TYPE_DATE;
        case 'E': return TYPE_ENUM;
        case 'G': return TYPE_GEOMETRIC;
        case 'N': return TYPE_NUMERIC;
        case 'S': return TYPE_STRING;
        case 'U': return TYPE_USERDEFINED;
        case 'X': return TYPE_UNKNOWN;
        default : return TYPE_UNKNOWN;
    }    
}

//void PGTypeManager::loadRefTypes() {
//    reftypes.insert("regproc");
//    reftypes.insert("regprocedure");
//    reftypes.insert("regoper");
//    reftypes.insert("regoperator");
//    reftypes.insert("regtype");
//    reftypes.insert("regclass");
//}


int PGTypeManager::enum_put (PGtypeArgs *args) {
    char *val = va_arg(args->ap, char *);
    char *out = NULL;
    int vallen = 0;

    if (!args || !val) return 0;

    /* expand buffer enough */
    vallen = string(val).length();
    if (args->put.expandBuffer(args, vallen) == -1) return -1;

    out = args->put.out;
    std::copy(val, val+vallen, out);

    return vallen;
}

int PGTypeManager::enum_get (PGtypeArgs *args) {
    char *val = CALL_PQ(fmap, PQgetvalue, args->get.result, args->get.tup_num, args->get.field_num);
    int vallen = CALL_PQ(fmap, PQgetlength, args->get.result, args->get.tup_num, args->get.field_num);

    char **result = va_arg(args->ap, char **);
    *result = (char *) CALL_PQ(fmap, PQresultAlloc, (PGresult *) args->get.result, (vallen+1) * sizeof(char));

    std::copy(val, val+vallen, *result);
    result[vallen] = '\0';

    return vallen;
}


#endif