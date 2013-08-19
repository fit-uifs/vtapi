/**
 * @file    backend_pg.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of all PostgreSQL-specific polymorphic classes:
 *  - PGConnection, PGTypeManager, PGResultSet, PGQueryBuilder and PGLibLoader
 *
 */

#include <iomanip>

#include "vtapi_backends.h"
#include "common/vtapi_misc.h"

using namespace vtapi;


PGConnection::PGConnection(fmap_t *fmap, const string& connectionInfo, Logger* logger)
: Connection (fmap, connectionInfo, logger){
    thisClass   = "PGConnection";
    conn        = NULL;
    if (!this->connect(connectionInfo)) {
        logger->error(121, "The connection couldn't have been established.", thisClass+"::PGConnection()");
    }
}

PGConnection::~PGConnection() {
    disconnect();
    CALL_PQT(fmap, PQclearTypes, conn);
}

bool PGConnection::connect (const string& connectionInfo) {
    connInfo    = connectionInfo;
    conn        = CALL_PQ(fmap, PQconnectdb, connInfo.c_str());
    if (CALL_PQ(fmap, PQstatus, conn) != CONNECTION_OK) {
        logger->warning(122, CALL_PQ(fmap, PQerrorMessage, conn), thisClass+"::connect()");
        return false;
    }
    else {
        CALL_PQT(fmap, PQinitTypes, conn);
        return true;
    }
};

bool PGConnection::reconnect (const string& connectionInfo) {
    if (!connectionInfo.empty()) connInfo = connectionInfo;
    disconnect();
    return this->connect(connInfo);
}

void PGConnection::disconnect () {
    if (isConnected()) {
        CALL_PQ(fmap, PQfinish, conn);
    }
}

bool PGConnection::isConnected () {
    if (CALL_PQ(fmap, PQstatus, conn) != CONNECTION_OK) {
        logger->warning(125, CALL_PQ(fmap, PQerrorMessage, conn), thisClass+"::isConnected()");
        return false;
    }
    else {
        return true;
    }
}

int PGConnection::execute(const string& query, void *param) {
    PGresult    *pgres  = NULL;

    errorMessage.clear();

    if (param && ((pg_param_t *)param)->args) {
        pgres = CALL_PQT(fmap, PQparamExec, conn, ((pg_param_t *)param)->args, query.c_str(), PG_FORMAT);
    }
    else {
        pgres = CALL_PQT(fmap, PQexecf, conn, query.c_str(), PG_FORMAT);
    }

    if (!pgres) {
        errorMessage = string(CALL_PQ(fmap, PQerrorMessage, conn));
        return -1;
    }
    else {
        int retval = 0;
        if (CALL_PQ(fmap, PQresultStatus, pgres) == PGRES_TUPLES_OK) {
            retval = CALL_PQ(fmap, PQntuples, pgres);
        }
        else if (CALL_PQ(fmap, PQresultStatus, pgres) == PGRES_COMMAND_OK) {
            retval = atoi(CALL_PQ(fmap, PQcmdTuples, pgres));
        }
        else {
            logger->warning(2012, "Apocalypse warning", thisClass+"::execute()");
            errorMessage = string(CALL_PQ(fmap, PQerrorMessage, conn));
            retval = -1;
        }
        CALL_PQ(fmap, PQclear, pgres);
        return retval;
    }
}

int PGConnection::fetch(const string& query, void *param, ResultSet *resultSet) {
    PGresult *pgres = NULL;
    
    errorMessage.clear();
    
    if (param && ((pg_param_t *)param)->args) {
        pgres = CALL_PQT(fmap, PQparamExec, conn, ((pg_param_t *)param)->args, query.c_str(), PG_FORMAT);
    }
    else {
        pgres = CALL_PQT(fmap, PQexecf, conn, query.c_str(), PG_FORMAT);
    }

    resultSet->newResult((void *) pgres);

    if (!pgres) {
        errorMessage = string(CALL_PQ(fmap, PQerrorMessage, conn));
        return -1;
    }
    else {
        if (CALL_PQ(fmap, PQresultStatus, pgres) == PGRES_TUPLES_OK) {
            return CALL_PQ(fmap, PQntuples, pgres);
        }
        else if (CALL_PQ(fmap, PQresultStatus, pgres) == PGRES_COMMAND_OK) {
            return 0;
        }
        else {
            logger->warning(2012, "Apocalypse warning", thisClass+"::fetch()");
            errorMessage = string(CALL_PQ(fmap, PQerrorMessage, conn));
            return -1;
        }
    }
}

void* PGConnection::getConnectionObject() {
    return (void *) this->conn;
}


PGTypeManager::PGTypeManager(fmap_t *fmap, Connection *connection, Logger *logger)
: TypeManager(fmap, connection, logger) {
    thisClass = "PGTypeManager";
    this->loadTypes();
    this->registerTypes();
}

PGTypeManager::~PGTypeManager() {
}

int PGTypeManager::registerTypes () {

    // general types registered at all times
    PGregisterType types_userdef[] =
    {
        {"seqtype", pg_enum_put, pg_enum_get},
        {"inouttype", pg_enum_put, pg_enum_get}//,
        //{"permissions", pg_enum_put, pg_enum_get}, // change 2 to 3 in next command
    };


    if (!CALL_PQT(fmap, PQregisterTypes, (PGconn *)connection->getConnectionObject(), PQT_USERDEFINED, types_userdef, 2, 0))
        logger->warning(666, CALL_PQT(fmap, PQgeterror, ), thisClass+"::registerTypes()");

    // PostGIS special types
#ifdef POSTGIS
    PGregisterType typespg_userdef[] = {
        {"cube", cube_put, cube_get},
        {"geometry", geometry_put, geometry_get}
    };
    if (!CALL_PQT(fmap, PQregisterTypes, (PGconn *)connection->getConnectionObject(), PQT_USERDEFINED, typespg_userdef, 2, 0))
        logger->warning(666, CALL_PQT(fmap, PQgeterror, ), thisClass+"::registerTypes()");
#endif

    // OpenCV special types
#ifdef __OPENCV_CORE_HPP__
    PGregisterType typescv_comp[] = {
        {"cvmat", NULL, NULL}
    };
    if (!CALL_PQT(fmap, PQregisterTypes, (PGconn *)connection->getConnectionObject(), PQT_COMPOSITE, typescv_comp, 1, 0))
        logger->warning(666, CALL_PQT(fmap, PQgeterror, ), thisClass+"::registerTypes()");
#endif
}

int PGTypeManager::loadTypes() {
    PGresult * pgres;

    pgres = CALL_PQT(fmap, PQexecf, (PGconn *)connection->getConnectionObject(),
            "SELECT oid, typname, typcategory, typlen, typelem from pg_catalog.pg_type", PG_FORMAT);
    if (!pgres) {
        return -1;
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
        return 0;
    }
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


PGQueryBuilder::PGQueryBuilder(fmap_t *fmap, Connection *connection, Logger *logger, const string& initString)
: QueryBuilder (fmap, connection, logger, initString) {
    thisClass   = "PGQueryBuilder";
    keysCnt     = 1;
}

PGQueryBuilder::~PGQueryBuilder() {
    destroyParam();    
}

string PGQueryBuilder::getGenericQuery() {
    return initString;
}

string PGQueryBuilder::getSelectQuery(const string& groupby, const string& orderby, const int limit, const int offset) {
    string queryString;
    string columnsStr;
    string tablesStr;
    string whereStr;

    if (this->keys_main.empty()) return initString; // in case of a direct query

    // go through keys
    for (int i = 0; i < keys_main.size(); i++) {
        string tmpTable     = !keys_main[i].from.empty() ? keys_main[i].from : this->table;
        string tmpColumn    = keys_main[i].key;
        size_t dotPos       = tmpTable.find(".");
        bool addTable       = true;

        // get escaped table
        if (dotPos == string::npos) {
            tmpTable = this->escapeIdent(this->dataset) + "." + this->escapeIdent(tmpTable);
        }
        else {
            tmpTable = this->escapeIdent(tmpTable.substr(0,dotPos)) + "." + this->escapeIdent(tmpTable.substr(dotPos+1,string::npos));
        }
        // get and add escaped column
        if (tmpColumn.empty() || tmpColumn.compare("*") == 0) {
            columnsStr  += tmpTable + "." + "*" +", ";
        }
        else {
            columnsStr  += tmpTable + "." + this->escapeColumn(tmpColumn, "");
            columnsStr  += " AS " + this->escapeLiteral(tmpColumn)  + ", ";
        }
        // check if table already exists
        for (int j = 0; j < i; j++) {
            if (keys_main[i].from.compare(keys_main[j].from) == 0 ||
               (keys_main[i].from.empty() && keys_main[j].from.empty())) {
                addTable = false;
                break;
            }
        }
        // add table
        if (addTable) {
            tablesStr   += tmpTable + ", ";
        }
    }
    // erase commas
    if (!columnsStr.empty())    columnsStr.erase(columnsStr.length()-2);
    if (!tablesStr.empty())     tablesStr.erase(tablesStr.length()-2);

    // construct main part of the query
    queryString = "SELECT " + columnsStr + "\n FROM " + tablesStr;
    if (tablesStr.empty() || columnsStr.empty()) {
        logger->error(201, queryString, thisClass+"::getSelectQuery()");
    }
    // construct WHERE and the rest of it all
    for (int i = 0; i < keys_where.size(); i++) {
        if (!whereStr.empty()) whereStr += " AND ";
        whereStr += this->escapeColumn(keys_where[i].key, keys_where[i].from);
        whereStr += opers[i];
        whereStr += "$" + toString(keys_where_order[i]);
    }
    if (!whereStr.empty()) {
        queryString += "\n WHERE " + whereStr;
    }
    if (!groupby.empty()) {
        queryString += "\n GROUP BY " + groupby;
    }
    if (!orderby.empty()) {
        queryString += "\n ORDER BY " + orderby;
    }
    if (limit > 0) {
        queryString += "\n LIMIT " + toString(limit);
    }
    if (offset > 0) {
        queryString += "\n OFFSET " + toString(offset);
    }
    queryString += ";";
    return (queryString);
}

string PGQueryBuilder::getInsertQuery() {
    string queryString;
    string dstTable;
    string intoStr;
    string valuesStr;

    if (this->keys_main.empty()) return initString; // in case of a direct query

    // in case we're lazy, we have the table specified in initString or selection
    dstTable = (!initString.empty()) ? initString : this->table;

    // go through keys
    for (int i = 0; i < this->keys_main.size(); ++i) {
        if (dstTable.empty()) dstTable = this->keys_main[i].from;
        intoStr     += escapeIdent(this->keys_main[i].key) + ", ";
        valuesStr   += "$" + toString(keys_main_order[i]) + ", ";
    }
    // this is to remove ending separators
    intoStr.erase(intoStr.length()-2);
    valuesStr.erase(valuesStr.length()-2);

    // add the dataset selected and escape table
    if (dstTable.find(".") == string::npos) {
        dstTable = escapeColumn(dstTable, this->dataset);
    }
    else {
        dstTable = escapeIdent(dstTable);
    }
    // construct query
    queryString = "INSERT INTO " + dstTable + " (" + intoStr + ")\n VALUES (" + valuesStr + ");";
    return queryString;
}

string PGQueryBuilder::getUpdateQuery() {
    string queryString;
    string dstTable;
    string setStr;
    string whereStr;    

    if (this->keys_main.empty()) return initString; // in case of a direct query

    // in case we're lazy, we have the table specified in initString or selection
    dstTable = (!initString.empty()) ? initString : this->table;

    // go through keys
    for (int i = 0; i < this->keys_main.size(); ++i) {
        if (dstTable.empty()) dstTable = this->keys_main[i].from;
        setStr  += escapeIdent(this->keys_main[i].key);
        setStr  += "=$" + toString(keys_main_order[i]) + ", ";
    }
    // this is to remove ending separators
    setStr.erase(setStr.length()-2);

    // add the dataset selected and escape table
    if (dstTable.find(".") == string::npos) {
        dstTable = escapeColumn(dstTable, this->dataset);
    }
    else {
        dstTable = escapeIdent(dstTable);
    }
    //construct main part of the query
    queryString = "UPDATE " + dstTable + "\n SET " + setStr;
    // construct WHERE clause
    for (int i = 0; i < keys_where.size(); i++) {
        if (!whereStr.empty()) whereStr += " AND ";
        whereStr += this->escapeColumn(keys_where[i].key, keys_where[i].from);
        whereStr += opers[i];
        whereStr += "$" + toString(keys_where_order[i]);
    }
    if (!whereStr.empty()) {
        queryString += "\n WHERE " + whereStr;
    }
    queryString += ";";
    return queryString;
}

bool PGQueryBuilder::keyFrom(const string& table, const string& column) {
    TKey k("", column, 1, table);
    keys_main.push_back(k);
    return true;
}

bool PGQueryBuilder::keyString(const string& key, const string& value, const string& from) {
    TKey k("varchar", key, 1, from);
    if (value.empty()) return false;
    keys_main.push_back(k);
    keys_main_order.push_back(keysCnt++);
    if (!param) createParam();
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%varchar", value.c_str());
    return true;
}

bool PGQueryBuilder::keyStringA(const string& key, string* values, const int size, const string& from) {

    return false;
}

bool PGQueryBuilder::keyInt(const string& key, int value, const string& from) {
    TKey k("int4", key, 1, from);
    keys_main.push_back(k);
    keys_main_order.push_back(keysCnt++);
    if (!param) createParam();
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%int4", value);
    return true;
}

bool PGQueryBuilder::keyIntA(const string& key, int* values, const int size, const string& from) {
    TKey k("int4[]", key, size, from);
    keys_main.push_back(k);
    keys_main_order.push_back(keysCnt++);

    if (!param) createParam();

    PGarray arr;
    arr.ndims = 0; // one dimensional arrays do not require setting dimension info
    // FIXME: this is a potential bug
    // arr.lbound[0] = 1;
    arr.param = CALL_PQT(fmap, PQparamCreate, (PGconn *)connection->getConnectionObject());

    // put the array elements
    for(int i = 0; i < size; ++i) {
        CALL_PQT(fmap, PQputf, arr.param, "%int4", values[i]);
    }
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%int4[]", &arr);
    CALL_PQT(fmap, PQparamClear, arr.param);
    return true;
}

bool PGQueryBuilder::keyFloat(const string& key, float value, const string& from) {
    TKey k("float4", key, 1, from);
    keys_main.push_back(k);
    keys_main_order.push_back(keysCnt++);
    if (!param) createParam();
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%float4", value);
    return true;
}

bool PGQueryBuilder::keyFloatA(const string& key, float* values, const int size, const string& from) {
    TKey k("float4[]", key, size, from);
    keys_main.push_back(k);
    keys_main_order.push_back(keysCnt++);

    if (!param) createParam();

    PGarray arr;
    arr.ndims = 0; // one dimensional arrays do not require setting dimension info
    // FIXME: this is a potential bug
    // arr.lbound[0] = 1;
    arr.param = CALL_PQT(fmap, PQparamCreate, (PGconn *)connection->getConnectionObject());

    // put the array elements
    for(int i = 0; i < size; ++i) {
        CALL_PQT(fmap, PQputf, arr.param, "%float4", values[i]);
    }
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%float4[]", &arr);
    CALL_PQT(fmap, PQparamClear, arr.param);
    return true;
}

bool PGQueryBuilder::keySeqtype(const string& key, const string& value, const string& from) {
    TKey k("seqtype", key, 1, from);
    if (value.empty()) return false;
    keys_main.push_back(k);
    keys_main_order.push_back(keysCnt++);
    if (!param) createParam();
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%seqtype", value.c_str());
    return true;
}

bool PGQueryBuilder::keyInouttype(const string& key, const string& value, const string& from) {
    TKey k("inouttype", key, 1, from);
    if (value.empty()) return false;
    keys_main.push_back(k);
    keys_main_order.push_back(keysCnt++);
    if (!param) createParam();
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%inouttype", value.c_str());

    return true;
}

//bool PGQueryBuilder::keyPermissions(const string& key, const string& value, const string& from) {
//    TKey k("permissions", key, 1, from);
//    keys_main.push_back(k);
//    keys_main_order.push_back(keysCnt++);
//   if (!param) createParam();
//    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%permissions", value.c_str());
//
//    return true;
//}

bool PGQueryBuilder::keyTimestamp(const string& key, const time_t& value, const string& from) {
    PGtimestamp timestamp = {0};
    struct tm* ts;

    TKey k("timestamp", key, 1, from);
    keys_main.push_back(k);
    keys_main_order.push_back(keysCnt++);
    if (!param) createParam();

    ts = gmtime(&value);
    timestamp.date.isbc = 0;
    timestamp.date.year = ts->tm_year + 1900;
    timestamp.date.mon = ts->tm_mon;
    timestamp.date.mday = ts->tm_mday;
    timestamp.time.hour = ts->tm_hour;
    timestamp.time.min = ts->tm_min;
    timestamp.time.sec = ts->tm_sec;
    timestamp.time.usec = 0;
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%timestamp", &timestamp);

    return true;
}

bool PGQueryBuilder::whereString(const string& key, const string& value, const string& oper, const string& from) {
    TKey k("varchar", key, 1, from);
    string value_put;

    if (value.empty()) return false;    

    keys_where.push_back(k);
    keys_where_order.push_back(keysCnt++);
    if (value.compare("NULL") == 0) {
        value_put = "IS NULL";
        opers.push_back("");
    }
    else if (value.compare("NOT NULL") == 0) {
        value_put = "IS NOT NULL";
        opers.push_back("");
    }
    else {
        value_put = value;
        opers.push_back(oper);
    }
    if (!param) createParam();
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%varchar", value_put.c_str());

    return true;
}

bool PGQueryBuilder::whereInt(const string& key, const int value, const string& oper, const string& from) {
    TKey k("int4", key, 1, from);
    keys_where.push_back(k);
    keys_where_order.push_back(keysCnt++);
    opers.push_back(oper);
    if (!param) createParam();
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%int4", value);
    return true;
}

bool PGQueryBuilder::whereFloat(const string& key, const float value, const string& oper, const string& from) {
    TKey k("float4", key, 1, from);
    keys_where.push_back(k);
    keys_where_order.push_back(keysCnt++);
    opers.push_back(oper);
    if (!param) createParam();
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%float4", value);
    return true;
}

bool PGQueryBuilder::whereSeqtype(const string& key, const string& value, const string& oper, const string& from) {
    TKey k("seqtype", key, 1, from);
    if (value.empty()) return false;
    keys_where.push_back(k);
    keys_where_order.push_back(keysCnt++);
    opers.push_back(oper);
    if (!param) createParam();
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%seqtype", value.c_str());
    return true;
}

bool PGQueryBuilder::whereInouttype(const string& key, const string& value, const string& oper, const string& from) {
    TKey k("inouttype", key, 1, from);
    if (value.empty()) return false;
    keys_where.push_back(k);
    keys_where_order.push_back(keysCnt++);
    opers.push_back(oper);
    if (!param) createParam();
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%inouttype", value.c_str());
    return true;
}

bool PGQueryBuilder::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from) {
    PGtimestamp timestamp = {0};
    struct tm* ts;

    TKey k("timestamp", key, 1, from);
    keys_where.push_back(k);
    keys_where_order.push_back(keysCnt++);
    opers.push_back(oper);
    if (!param) createParam();

    ts = gmtime(&value);
    timestamp.date.isbc = 0;
    timestamp.date.year = ts->tm_year + 1900;
    timestamp.date.mon = ts->tm_mon;
    timestamp.date.mday = ts->tm_mday;
    timestamp.time.hour = ts->tm_hour;
    timestamp.time.min = ts->tm_min;
    timestamp.time.sec = ts->tm_sec;
    timestamp.time.usec = 0;
    CALL_PQT(fmap, PQputf, ((pg_param_t *)param)->args, "%timestamp", &timestamp);

    return true;
}




bool PGQueryBuilder::reset() {
    keys_main.clear();
    keys_where.clear();
    keys_main_order.clear();
    keys_where_order.clear();
    keysCnt = 1;
    opers.clear();
    destroyParam();
}

void PGQueryBuilder::createParam() {
    pg_param_t *param_new = NULL;
    destroyParam();
    param_new       = new pg_param_t();
    param_new->args = CALL_PQT(fmap, PQparamCreate, (PGconn *)connection->getConnectionObject());
    param           = (void *) param_new;
}

void PGQueryBuilder::destroyParam() {
    pg_param_t *param_kill = (pg_param_t *) param;
    if (param_kill) {
        if (param_kill->args) {
            CALL_PQT(fmap, PQparamClear, param_kill->args);
        }
        destruct(param_kill);
    }
}

string PGQueryBuilder::escapeColumn(const string& key, const string& table) {

    int keyLength = key.length();
    string rest = "";

    // FIXME: in a case, there may be a buffer overflow - do not expose column names!
    // FIXME: in a case, this may reorder somethings' meaning!
    if (key.find(':') != string::npos) {
        keyLength = key.find(':');
        rest += key.substr(key.find(':'));
    } else if (key.find('[') != string::npos) {
        keyLength = key.find('[');
        rest += key.substr(key.find('['));
    } else if (key.find('(') != string::npos) {
        keyLength = key.find('(');
        rest += key.substr(key.find('('));
    } // else { // nothing to do


//    string ret = "";
//    char* c = CALL_PQ(fmap, PQescapeIdentifier, (PGconn *)connection->getConnectionObject(), key.c_str(), keyLength);
//
//    if (!table.empty()) {
//        char* t = CALL_PQ(fmap, PQescapeIdentifier, (PGconn *)connection->getConnectionObject(), table.c_str(), table.length());
//        ret += string(t) + ".";
//        CALL_PQ(fmap, PQfreemem, t);
//    }
//
//    ret += string(c) + rest;
//    CALL_PQ(fmap, PQfreemem, c);
//
//    return ret;
    return (!table.empty() ? (escapeIdent(table) + ".") : "") + escapeIdent(key.substr(0, keyLength)) + rest;

}

string PGQueryBuilder::escapeIdent(const string& ident) {
    char    *ident_c = CALL_PQ(fmap, PQescapeIdentifier, (PGconn *)connection->getConnectionObject(), ident.c_str(), ident.length());
    string  escaped  = string(ident_c);
    CALL_PQ(fmap, PQfreemem, ident_c);
    return string(escaped);
}

string PGQueryBuilder::escapeLiteral(const string& literal) {
    return "";
}

/***********************************************************************************************************************************/

PGResultSet::PGResultSet(fmap_t *fmap, TypeManager *typeManager, Logger *logger)
 : ResultSet(fmap, typeManager, logger) {
    thisClass = "PGResultSet";
}

PGResultSet::~PGResultSet() {
    clear();
}

void PGResultSet::newResult(void *res) {
    clear();
    this->res = res;
}

int PGResultSet::countRows() {
    return CALL_PQ(fmap, PQntuples , (PGresult *) this->res);
}
int PGResultSet::countCols() {
    return CALL_PQ(fmap, PQnfields , (PGresult *) this->res);
}
bool PGResultSet::isOk() {
    return (this->res) ? true : false;
}
void PGResultSet::clear() {
    if (this->res) {
        CALL_PQ(fmap, PQclear, (PGresult *) this->res);
        this->res = NULL;
    }
}


TKey PGResultSet::getKey(int col) {
    PGresult *pgres = (PGresult *) this->res;
    string type, name;

    name    = CALL_PQ(fmap, PQfname, pgres, col);
    type    = typeManager->toTypname(CALL_PQ(fmap, PQftype, pgres, col));
    return TKey(type, name, 0);
}

TKeys* PGResultSet::getKeys() {
    PGresult *pgres = (PGresult *) this->res;
    TKeys* keys     = new TKeys;
    int cols = CALL_PQ(fmap, PQnfields, pgres);

    for (int col = 0; col < cols; col++) {
        keys->push_back(getKey(col));
    }
    return keys;
}

string PGResultSet::getKeyType(const int col) {
    return typeManager->toTypname(CALL_PQ(fmap, PQftype, (PGresult *) this->res, col));
}

int PGResultSet::getKeyIndex(const string& key) {
    return CALL_PQ(fmap, PQfnumber, (PGresult *) this->res, key.c_str());
}

// =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===================
char PGResultSet::getChar(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    PGchar value    = '\0';
    int ret         = CALL_PQT(fmap, PQgetf, pgres, this->pos, "%char", col, &value);
    if (ret == 0) {
        logger->warning(304, "Value is not an char", thisClass+"::getChar()");
        return '\0';
    }
    else return value;
}

char *PGResultSet::getCharA(const int col, int& size) {
    PGresult *pgres = (PGresult *) this->res;
    PGarray tmp;
    if (! CALL_PQT(fmap, PQgetf, pgres, this->pos, "%char[]", col, &tmp)) {
        logger->warning(304, "Value is not an array of chars", thisClass+"::getCharA()");
        size = -1;
        return NULL;
    }

    size = CALL_PQ(fmap, PQntuples, tmp.res);
    char* values = new char [size];
    for (int i = 0; i < size; i++) {
        if (! CALL_PQT(fmap, PQgetf, tmp.res, i, "%char", 0, &values[i])) {
            logger->warning(304, "Unexpected value in char array", thisClass+"::getCharA()");
            size = -1;
            CALL_PQ(fmap, PQclear, tmp.res);
            destruct (values);
            return NULL;
        }
    }
    CALL_PQ(fmap, PQclear, tmp.res);

    return values;
}

string PGResultSet::getString(const int col) {
    PGresult *pgres             = (PGresult *) this->res;
    PGtext value                = (PGtext) "";
    string keytype              = getKeyType(col);
    type_category_t category    = typeManager->getTypeMetadata(keytype).category;
    if (!pgres) return "";    

    // reference types (regtype, regclass..)
    if (category == TYPE_STRING || category == TYPE_ENUM) {
        value = CALL_PQ(fmap, PQgetvalue, pgres, this->pos, col);
    }
    // reference types (regtype, regclass..)
    // TODO: dodelat v typemanager
    else if(category == TYPE_REFTYPE) {
        
    }
    return (value) ? string(value) : string("");
}

// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================

int PGResultSet::getInt(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    int value = 0;
    short length = typeManager->getTypeMetadata(getKeyType(col)).length;

    // this circumvents libpqtypes and handles all int types (int, oid...)
    if (length < 0) { // conversion if length == -1
        stringstream iss (CALL_PQ(fmap, PQgetvalue, pgres, this->pos, col));
        iss >> value;
    }
     // short 2-byte int
    else if (length == 2) {
//        CALL_PQT(fmap, PQgetf, pgres, this->pos, "%int2", col, &value);
        endian_swap2(&value, CALL_PQ(fmap, PQgetvalue, pgres, this->pos, col));
    }
    // 4-byte int
    else if (length == 4) {
//        CALL_PQT(fmap, PQgetf, pgres, this->pos, "%int4", col, &value);
        endian_swap4(&value, CALL_PQ(fmap, PQgetvalue, pgres, this->pos, col));
    }
    else {
        if (length == 8) {
            logger->warning(306, "Use getInt8(...) to retrieve int8 values", thisClass+"::getInt()");
        }
        else {
            logger->warning(306, "Integer value of length "+toString(length)+" is not supported", thisClass+"::getInt()");
        } 
    }
    return value;
}

long PGResultSet::getInt8(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    long value = 0;
    short length = typeManager->getTypeMetadata(getKeyType(col)).length;

    // extract long or call getInt if value is integer
    if (length == 8) {
//        CALL_PQT(fmap, PQgetf, pgres, this->pos, "%int8", col, &value);
        endian_swap8(&value, CALL_PQ(fmap, PQgetvalue, pgres, this->pos, col));
    }
    else if (length < 8) {
        value = (long) getInt(col);
    }
    else {
        logger->warning(306, "Integer value of length "+toString(length)+" is not supported", thisClass+"::getInt8()");
    }
    return value;
}

int* PGResultSet::getIntA(const int col, int& size) {
    PGresult *pgres = (PGresult *) this->res;
    PGarray tmp;
    if (! CALL_PQT(fmap, PQgetf, pgres, this->pos, "%int4[]", col, &tmp)) {
        logger->warning(307, "Value is not an array of integer", thisClass+"::getIntA()");
        size = -1;
        return NULL;
    }

    size = CALL_PQ(fmap, PQntuples, tmp.res);
    int* values = new int [size];
    for (int i = 0; i < size; i++) {
        if (! CALL_PQT(fmap, PQgetf, tmp.res, i, "%int4", 0, &values[i])) {
            logger->warning(308, "Unexpected value in integer array", thisClass+"::getIntA()");
            size = -1;
            CALL_PQ(fmap, PQclear, tmp.res);
            destruct (values);
            return NULL;
        }
    }
    CALL_PQ(fmap, PQclear, tmp.res);

    return values;
}

vector<int>* PGResultSet::getIntV(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    PGarray tmp;
    if (! CALL_PQT(fmap, PQgetf, pgres, this->pos, "%int4[]", col, &tmp)) {
        logger->warning(307, "Value is not an array of integer", thisClass+"::getIntV()");
        return NULL;
    }

    PGint4 value;
    vector<int>* values = new vector<int>;

    for (int i = 0; i < CALL_PQ(fmap, PQntuples, tmp.res); i++) {
        if (! CALL_PQT(fmap, PQgetf, tmp.res, i, "%int4", 0, &value)) {
            logger->warning(308, "Unexpected value in integer array", thisClass+"::getIntV()");
            CALL_PQ(fmap, PQclear, tmp.res);
            destruct (values);
            return NULL;
        }
        values->push_back(value);
    }
    CALL_PQ(fmap, PQclear, tmp.res);

    return values;
}

vector< vector<int>* >* PGResultSet::getIntVV(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    PGarray tmp;
    if (! CALL_PQT(fmap, PQgetf, pgres, this->pos, "%int4[]", col, &tmp)) {
        logger->warning(307, "Value is not an array of integer arrays", thisClass+"::getIntVV()");
        return NULL;
    }
    if (tmp.ndims != 2) {
        logger->warning(308, "Array must have exactly 2 dimensions", thisClass+"::getIntVV()");
        return NULL;
    }

    PGint4 value;
    vector< vector<int>* >* arrays = new vector< vector<int>* >;
    vector<int>* arr;

    // array rows
    for (int i = 0; i < tmp.dims[0]; i++) {
        arr = new vector<int>;
        // array columns
        for (int j = 0; j < tmp.dims[1]; j++) {
            if (! CALL_PQT(fmap, PQgetf, tmp.res, i*tmp.dims[1]+j, "%int4", 0, &value)) {
                logger->warning(308, "Unexpected value in integer array", thisClass+"::getIntVV()");
                CALL_PQ(fmap, PQclear, tmp.res);
                for (int x = 0; x < (*arrays).size(); x++) destruct ((*arrays)[x]);
                destruct (arrays);
                return NULL;
            }
            arr->push_back(value);
        }
        arrays->push_back(arr);
    }
    return arrays;
}

// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================
float PGResultSet::getFloat(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    float value = 0;
    short length = typeManager->getTypeMetadata(getKeyType(col)).length;
    if (length < 0) { // conversion if length == -1
        stringstream iss (CALL_PQ(fmap, PQgetvalue, pgres, this->pos, col));
        iss >> value;
    }
    else if (length == 4) {
//        CALL_PQT(fmap, PQgetf, pgres, this->pos, "%float4", col, &value);
        endian_swap4(&value, CALL_PQ(fmap, PQgetvalue, pgres, this->pos, col));
    }
    else {
        logger->warning(306, "Float value of length "+toString(length)+" is not supported", thisClass+"::getFloat()");
    }
    return value;
}

double PGResultSet::getFloat8(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    double value = 0;
    short length = typeManager->getTypeMetadata(getKeyType(col)).length;

    if (length < 8) {
        value = (double) getFloat(col);
    }
    else  if (length == 8) {
        CALL_PQT(fmap, PQgetf, pgres, this->pos, "%float8", col, &value);
    }
    else {
        logger->warning(306, "Float value of length "+toString(length)+" is not supported", thisClass+"::getFloat8()");
    }
    return value;
}

float* PGResultSet::getFloatA(const int col, int& size) {
    PGresult *pgres = (PGresult *) this->res;
    PGarray tmp;
    if (! CALL_PQT(fmap, PQgetf, pgres, this->pos, "%float4[]", col, &tmp)) {
        logger->warning(311, "Value is not an array of float", thisClass+"::getFloatA()");
        size = -1;
        return NULL;
    }

    size = CALL_PQ(fmap, PQntuples, tmp.res);
    float* values = new float [size];
    for (int i = 0; i < size; i++) {
        if (! CALL_PQT(fmap, PQgetf, tmp.res, i, "%float4", 0, &values[i])) {
            logger->warning(312, "Unexpected value in float array", thisClass+"::getFloatA()");
            size = -1;
            CALL_PQ(fmap, PQclear, tmp.res);
            destruct (values);
            return NULL;
        }
    }
    CALL_PQ(fmap, PQclear, tmp.res);

    return values;
}

vector<float>* PGResultSet::getFloatV(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    PGarray tmp;
    if (! CALL_PQT(fmap, PQgetf, pgres, this->pos, "%float4[]", col, &tmp)) {
        logger->warning(311, "Value is not an array of float", thisClass+"::getFloatV()");
        return NULL;
    }

    PGfloat4 value;
    vector<float>* values = new vector<float>;
    for (int i = 0; i < CALL_PQ(fmap, PQntuples, tmp.res); i++) {
        if (! CALL_PQT(fmap, PQgetf, tmp.res, i, "%float4", 0, &value)) {
            logger->warning(312, "Unexpected value in float array", thisClass+"::getFloatV()");
            CALL_PQ(fmap, PQclear, tmp.res);
            destruct (values);
            return NULL;
        }
        values->push_back(value);
    }
    CALL_PQ(fmap, PQclear, tmp.res);

    return values;
}

// =============== GETTERS - TIMESTAMP =========================================

time_t PGResultSet::getTimestamp(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    struct tm ts = {0};
    string dtype = getKey(col).type;

    if (!dtype.compare("time")) {
        PGtime timestamp;
        CALL_PQT(fmap, PQgetf, pgres, this->pos, "%time", col, &timestamp);
        ts.tm_hour  = timestamp.hour;
        ts.tm_min   = timestamp.min;
        ts.tm_sec   = timestamp.sec;
    }
    else if (!dtype.compare("timestamp")) {
        PGtimestamp timestamp;
        CALL_PQT(fmap, PQgetf, pgres, this->pos, "%timestamp", col, &timestamp);
        ts.tm_year  = timestamp.date.year;
        ts.tm_mon   = timestamp.date.mon;
        ts.tm_mday  = timestamp.date.mday;
        ts.tm_hour  = timestamp.time.hour;
        ts.tm_min   = timestamp.time.min;
        ts.tm_sec   = timestamp.time.sec;
    }
    else {
        logger->warning(313, "Data type "+dtype+" not yet supported", thisClass+"::getTimestamp()");
    }
    return mktime(&ts);
}

int PGResultSet::getIntOid(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    PGint4 value;

    CALL_PQT(fmap, PQgetf, pgres, this->pos, "%oid", col, &value);
    return (int) value;
}

string PGResultSet::getValue(const int col, const int arrayLimit) {
    stringstream valss;                         //return stringstream
    string keytype      = getKeyType(col);      // get column type
    // get type metadata (type category, byte length and array type
    type_metadata_t keytype_metadata   = typeManager->getTypeMetadata(keytype);
   
    //int precision = 4; // float precision

    // check for NULL resultset and field
    if (!this->res) return "";
    if (CALL_PQ(fmap, PQgetisnull, (PGresult *)this->res, this->pos, col)) return "";

    // Call different getters for different categories of types
    switch (keytype_metadata.category) {

        case TYPE_ARRAY: {
            //TODO: arrays of other types
            if (keytype.find("int") != string::npos) {
                // array of 4-byte ints
                if (keytype_metadata.length_elem == 4) {
                    vector<int>* arr = this->getIntV(col);
                    if (arr) for (int i = 0; i < (*arr).size(); i++) {
                        valss << (*arr)[i];
                        if (arrayLimit && i == arrayLimit) {
                            valss << "...";
                            break;
                        }
                        if (i < (*arr).size()-1) valss << ",";
                    }
                    destruct (arr);
                }
            }
            
            else if (keytype.find("float") != string::npos) {
                // array of 4-byte floats
                if (keytype_metadata.length_elem == 4) {
                    vector<float>* arr = this->getFloatV(col);
                    if (arr) for (int i = 0; i < (*arr).size(); i++) {
                        valss << (*arr)[i];
                        if (arrayLimit && i == arrayLimit) {
                            valss << "...";
                            break;
                        }
                        if (i < (*arr).size()-1) valss << ",";
                    }
                    destruct (arr);
                }
            }

            else if (keytype.find("char") != string::npos) {
                // array of 1-byte chars
                if (keytype_metadata.length_elem == 1) {
                    int arr_size;
                    char *arr = this->getCharA(col, arr_size);
                    for (int i = 0; i < arr_size; i++) valss << arr[i];
                    destruct (arr);
                }
            }

            // array of PGpoint
            else if (keytype.find("point") != string::npos) {
#ifdef POSTGIS

                vector<PGpoint>* arr = this->getPointV(col);
                if (arr) for (int i = 0; i < (*arr).size(); i++) {
                    valss << "(" << (*arr)[i].x << "," << (*arr)[i].y << ")";
                    if (arrayLimit && i == arrayLimit) {
                        valss << "...";
                        break;
                    }
                    if (i < (*arr).size()-1) valss << ",";
                }
                destruct (arr);
#endif
            }

            } break;

        case TYPE_BOOLEAN: { // NOT IMPLEMENTED
            } break;

        case TYPE_COMPOSITE: {
#ifdef __OPENCV_CORE_C_H__XXX
            // OpenCV cvMat type
            if (!keytype.compare("cvmat")) {
                CvMat *mat = getCvMat(col);
                valss << cvGetElemType(mat);
                cvReleaseMat(&mat);
            }
#endif
            } break;

        case TYPE_DATE: {
            time_t ts = getTimestamp(col);
            valss << toString(ts);
//            valss << std::right << std::setfill('0');
//            if (ts.tm_year > 0)
//                valss << std::setw(4) << ts.tm_year << '-' << std::setw(2) << ts.tm_mon <<
//                  '-' << std::setw(2) << ts.tm_mday << ' ' << std::setw(2) << ts.tm_hour <<
//                  ':' << std::setw(2) << ts.tm_min  << ':' << std::setw(2) << ts.tm_sec;
            } break;

        case TYPE_ENUM: {
            valss << getString(col);
            } break;

        case TYPE_GEOMETRIC: {
#ifdef POSTGIS
            // PostGIS point type
            if (!keytype.compare("point")) {
                PGpoint point = getPoint(col);
                valss << point.x << " , " << point.y;
            }
            // PostGIS box type
            else if (!keytype.compare("box")) {
                PGbox box = getBox(col);
                valss << '(' << box.low.x << " , " << box.low.y << ") , ";
                valss << '(' << box.high.x << " , " << box.high.y << ')';
            }
            // PostGIS line-segment type
            else if (!keytype.compare("lseg")) {
                PGlseg lseg = getLineSegment(col);
                valss << '(' << lseg.pts[0].x << " , " << lseg.pts[0].y << ") ";
                valss << '(' << lseg.pts[1].x << " , " << lseg.pts[1].y << ')';
            }
            // PostGIS circle type
            else if (!keytype.compare("circle")) {
                PGcircle circle = getCircle(col);
                valss << '(' << circle.center.x << " , " << circle.center.y;
                valss << ") , " << circle.radius;
            }
            // PostGIS path type
            else if (!keytype.compare("path")) {
                PGpath path = getPath(col);
                for (int i = 0; i < path.npts; i++) {
                    valss << '(' << path.pts[i].x << " , " << path.pts[i].y << ')';
                    if (arrayLimit && i == arrayLimit) {
                        valss << "...";
                        break;
                    }
                    if (i < path.npts-1) valss << " , ";
                }
            }
            // PostGIS polygon type
            else if (!keytype.compare("polygon")) {
                PGpolygon polygon = getPolygon(col);
                for (int i = 0; i < polygon.npts; i++) {
                    valss << '(' << polygon.pts[i].x << " , " << polygon.pts[i].y << ')';
                    if (arrayLimit && i == arrayLimit) {
                        valss << "...";
                        break;
                    }
                    if (i < polygon.npts-1) valss << " , ";
                }
            }
#endif
            } break;

        case TYPE_NUMERIC: {
            // recognize 4 or 8-bytes integers and floats
            if (!keytype.substr(0,5).compare("float"))
                keytype_metadata.length < 8 ? valss << getFloat(col) : valss << getFloat8(col);
            else
                keytype_metadata.length < 8 ? valss << getInt(col) : valss << getInt8(col);
        } break;

        case TYPE_STRING: {
            if (keytype_metadata.length == 1) valss << getChar(col); // char has length 1
            else return getString(col);
        } break;


        case TYPE_USERDEFINED: { // user-defined (cube + postGIS types!!)
#ifdef POSTGIS
            // PostGIS generic geometry type
            if (!keytype.compare("geometry")) {
                GEOSGeometry *geo;
                GEOSWKTWriter *geo_writer;
                char * geo_string;

                if (!(geo = getGeometry(col))) break;
                if (! (geo_writer  =  GEOSWKTWriter_create())) {
                    GEOSGeom_destroy(geo);
                    break;
                }

                //TODO: GEOS 2.2 conflict, if resolved uncomment int precision
                //GEOSWKTWriter_setRoundingPrecision(geo_writer, precision);
                geo_string = GEOSWKTWriter_write(geo_writer, geo);
                valss << geo_string;

                GEOSFree(geo_string);
                GEOSGeom_destroy(geo);
            }
            // PostGIS cube type
            else if (!keytype.compare("cube")) {
                PGcube cube = getCube(col);
                int lim = cube.dim;
                if (arrayLimit && (2 * lim > arrayLimit)) lim = arrayLimit/2;
                if (cube.x && cube.dim > 0) {
                    valss << '(';
                    for (int i = 0; i < lim; i++) {
                        valss << cube.x[i];
                        if (i < lim-1) valss << ',';
                    }
                    valss << ')';
                    if (cube.x[lim]) {
                        valss << ",(";
                        for (int i = lim; i < 2 * lim; i++) {
                            valss << cube.x[i];
                            if (i < lim-1) valss << ',';
                        }
                    }
                    valss << ')';
                }
            }
#endif
        } break;

        case TYPE_REFTYPE:{
            // TODO: Petr if (keytype.equals("regclass") == 0) // pg_namespace.oid > 99 (pg_catalog, pg_toast)
        } break;

        case TYPE_UNKNOWN:
        default: {
            valss << "???";
        } break;
    }
    return valss.str();
}



pair< TKeys*, vector<int>* > PGResultSet::getKeysWidths(const int row, bool get_widths, const int arrayLimit) {
    PGresult *pgres     = (PGresult *) this->res;
    vector<int> *widths = get_widths ? new vector<int>() : NULL;
    TKeys *keys         = getKeys();
    int rows            = CALL_PQ(fmap, PQntuples, pgres);
    int cols            = CALL_PQ(fmap, PQnfields, pgres);
    int plen, flen, tlen, width;

    if (!get_widths && keys) return std::make_pair(keys, widths);
    else if (!widths || !keys || cols != keys->size() || cols == 0 || rows == 0) {
        destruct(widths);
        destruct(keys);
        return std::make_pair((TKeys*)NULL, (vector<int>*)NULL);
    }

    // header and first row
    for (int c = 0; c < cols; c++) {
        // don't forget to save original value of pos beforehand!
        this->pos = row < 0 ? 0 : row;
        flen = (*keys)[c].key.length();  // field name length
        tlen = (*keys)[c].type.length(); // data type string length
        plen = getValue(c).length();     // field value string length
        if (plen >= flen && plen >= tlen) width = plen;
        else if (flen >= plen && flen >= tlen) width = flen;
        else width = tlen;
        widths->push_back(width);
    }

    // rest of the rows
    if (row < 0) {
        for (int r = 1; r < rows; r++) {
            // very ugly manipulation with pos, it needs to be reset after print
            this->pos = r;
            for (int c = 0; c < cols; c++) {
                plen = getValue(c).length();
                if (plen > (*widths)[c]) (*widths)[c] = plen;
            }
        }
    }
    if (widths->size() != keys->size()) {
        destruct(widths);
        destruct(keys);
        return std::make_pair((TKeys*)NULL, (vector<int>*)NULL);
    }
    else return std::make_pair(keys, widths);
}



PGLibLoader::PGLibLoader(Logger *logger)
 : LibLoader (logger) {
    thisClass = "PGLibLoader";
    h_libpqtypes = NULL;
    h_libpq = NULL;
    lt_dlinit();
}

PGLibLoader::~PGLibLoader() {
    unloadLibs();
    lt_dlexit();
};

fmap_t *PGLibLoader::loadLibs() {
    int ret = 0;
    fmap_t *fmap = new fmap_t();

    ret += load_libpq(fmap);
    ret += load_libpqtypes(fmap);

    if (ret == 0) {
        return fmap;
    }
    else {
        destruct(fmap);
        return NULL;
    }
};

int PGLibLoader::unloadLibs() {
    int ret = 0;

    ret += unload_libpqtypes();
    ret += unload_libpq();
    return ret;
};

bool PGLibLoader:: isLoaded() {
    return h_libpq && h_libpqtypes;
}

char *PGLibLoader::getLibName() {
    char *name = NULL;
    const lt_dlinfo *libpq_info;

    if (h_libpq) {
        libpq_info = lt_dlgetinfo(h_libpq);
        name = libpq_info->name;
    }
    return name;
}

int PGLibLoader::load_libpqtypes (fmap_t *fmap) {
    int retval = 0;
    void *funcPtr = NULL;
    const string funcStrings[] =
    {"PQgeterror","PQseterror","PQgetErrorField","PQspecPrepare","PQclearSpecs","PQinitTypes",
     "PQregisterSubClasses","PQregisterComposites","PQregisterUserDefinedTypes","PQregisterTypes",
     "PQregisterResult","PQclearTypes","PQparamCreate","PQparamDup","PQparamCount","PQparamReset",
     "PQparamClear","PQputf","PQputvf","PQgetf","PQgetvf","PQexecf","PQexecvf","PQsendf","PQsendvf",
     "PQparamExec","PQparamSendQuery","PQparamExecPrepared","PQparamSendQueryPrepared","PQlocalTZInfo",
     ""
    };

    h_libpqtypes = lt_dlopenext("libpqtypes");

    if (h_libpqtypes) {
        for(int i = 0; !funcStrings[i].empty(); i++) {
            funcPtr = lt_dlsym(h_libpqtypes, funcStrings[i].c_str());
            if (funcPtr) {
                fmap->insert(FMAP_ENTRY("PQT_"+funcStrings[i], funcPtr));
            }
            else {
                logger->warning(555, "Function " + funcStrings[i] + " not loaded.", thisClass+"::load_libpqtypes()");
                retval = -1;
                break;
            }
        }
    }
    else {
        logger->warning(556, "Libpqtypes library not found.", thisClass+"::load_libpqtypes()");
        retval = -1;
    }
    return retval;
}

int PGLibLoader::load_libpq (fmap_t *fmap) {
    int retval = 0;
    lt_dladvise libpq_advise = NULL;
    void *funcPtr = NULL;
    const string funcStrings[] =
    {"PQconnectdb","PQstatus","PQerrorMessage","PQfinish","PQclear","PQresultAlloc","PQfreemem","PQresultStatus","PQntuples", "PQcmdStatus",
     "PQcmdTuples","PQfname","PQfnumber","PQftype","PQnfields","PQgetvalue","PQgetisnull","PQgetlength","PQescapeLiteral","PQescapeIdentifier",
    ""
    };

    lt_dladvise_init (&libpq_advise);
    lt_dladvise_ext(&libpq_advise);
    lt_dladvise_global(&libpq_advise);
    h_libpq = lt_dlopenadvise("libpq", libpq_advise);
    lt_dladvise_destroy(&libpq_advise);

    if (h_libpq) {
        for(int i = 0; !funcStrings[i].empty(); i++) {
            funcPtr = lt_dlsym(h_libpq, funcStrings[i].c_str());
            if (funcPtr) {
                fmap->insert(FMAP_ENTRY("PQ_"+funcStrings[i], funcPtr));
            }
            else {
                logger->warning(555, "Function " + funcStrings[i] + " not loaded.", thisClass+"::load_libpq()");
                retval = -1;
                break;
            }
        }
    }
    else {
        logger->warning(556, "Libpq library not found.", thisClass+"::load_libpq()");
        retval = -1;
    }
    return retval;
}

int PGLibLoader::unload_libpqtypes () {
    if (h_libpqtypes) {
        lt_dlclose(h_libpqtypes);
        h_libpqtypes = NULL;
        return 0;
    }
    else {
        return -1;
    }
}

int PGLibLoader::unload_libpq () {
    if (h_libpq) {
        lt_dlclose(h_libpq);
        h_libpq = NULL;
        return 0;
    }
    else {
        return -1;
    }
}
