
#include <common/vtapi_global.h>
#include <backends/vtapi_querybuilder.h>


#if HAVE_POSTGRESQL

// postgres data transfer format: 0=text, 1=binary
#define PG_FORMAT           1

using std::string;

using namespace vtapi;

PGQueryBuilder::PGQueryBuilder(const PGBackendBase &base, void *connection, const string& initString) :
    QueryBuilder (connection, initString),
    PGBackendBase(base)
{
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

    // construct main part of the query
    if (!this->keys_main.empty()) {
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
                columnsStr  += " AS " + this->escapeAlias(tmpColumn)  + ", ";
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
    }
    // keys are empty, use direct query
    else {
        queryString = this->initString;
    }
    
    // construct WHERE and the rest of it all
    if (!this->keys_where.empty()) {
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
    }
    queryString += ";";

    //printf("%s\n", queryString.c_str());
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
        dstTable = escapeColumn(dstTable, "");
    }
    
    // construct query
    queryString = "INSERT INTO " + dstTable + " (" + intoStr + ")\n VALUES (" + valuesStr + ");";
    
    //printf("%s\n", queryString.c_str());
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
    if (column.empty()) return VT_FAIL;
    else {
        TKey k("", column, 1, table);
        keys_main.push_back(k);
        return VT_OK;
    }
}

bool PGQueryBuilder::keyString(const string& key, const string& value, const string& from) {
    if (key.empty() || value.empty()) return VT_FAIL;
    else {
        TKey k("varchar", key, 1, from);
        keys_main.push_back(k);
        keys_main_order.push_back(keysCnt++);
        if (!param) createParam();
        pqt.PQputf((PGparam *)param, "%varchar", value.c_str());
        return VT_OK;
    }
}

bool PGQueryBuilder::keyStringA(const string& key, string* values, const int size, const string& from) {
    if (key.empty() || !values || size <= 0) return VT_FAIL;
    else {
        TKey k("varchar[]", key, size, from);
        keys_main.push_back(k);
        keys_main_order.push_back(keysCnt++);

        if (!param) createParam();

        PGarray arr = {0};
        arr.param = pqt.PQparamCreate((PGconn *)connection);

        // put the array elements
        for(int i = 0; i < size; ++i) {
            pqt.PQputf(arr.param, "%varchar", values[i].c_str());
        }
        pqt.PQputf((PGparam *)param, "%varchar[]", &arr);
        pqt.PQparamClear(arr.param);
        return VT_OK;
    }
}

bool PGQueryBuilder::keyInt(const string& key, int value, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKey k("int4", key, 1, from);
        keys_main.push_back(k);
        keys_main_order.push_back(keysCnt++);
        if (!param) createParam();
        pqt.PQputf((PGparam *)param, "%int4", value);
        return VT_OK;
    }
}

bool PGQueryBuilder::keyIntA(const string& key, int* values, const int size, const string& from) {
    if (key.empty() || !values || size <= 0) return VT_FAIL;
    else {
        TKey k("int4[]", key, size, from);
        keys_main.push_back(k);
        keys_main_order.push_back(keysCnt++);

        if (!param) createParam();

        PGarray arr = {0};
        arr.param = pqt.PQparamCreate((PGconn *)connection);

        // put the array elements
        for(int i = 0; i < size; ++i) {
            pqt.PQputf(arr.param, "%int4", values[i]);
        }
        pqt.PQputf((PGparam *)param, "%int4[]", &arr);
        pqt.PQparamClear(arr.param);
        return VT_OK;
    }
}

bool PGQueryBuilder::keyFloat(const string& key, float value, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKey k("float4", key, 1, from);
        keys_main.push_back(k);
        keys_main_order.push_back(keysCnt++);
        if (!param) createParam();
        pqt.PQputf((PGparam *)param, "%float4", value);
        return VT_OK;
    }
}

bool PGQueryBuilder::keyFloatA(const string& key, float* values, const int size, const string& from) {
    if (key.empty() || !values || size <= 0) return VT_FAIL;
    else {
        TKey k("float4[]", key, size, from);
        keys_main.push_back(k);
        keys_main_order.push_back(keysCnt++);

        if (!param) createParam();

        PGarray arr = {0};
        arr.param = pqt.PQparamCreate((PGconn *)connection);

        // put the array elements
        for(int i = 0; i < size; ++i) {
            pqt.PQputf(arr.param, "%float4", values[i]);
        }
        pqt.PQputf((PGparam *)param, "%float4[]", &arr);
        pqt.PQparamClear(arr.param);
        return VT_OK;
    }
}

bool PGQueryBuilder::keySeqtype(const string& key, const string& value, const string& from) {
    if (key.empty() || value.empty() || !this->checkSeqtype(value)) return VT_FAIL;
    else {
        TKey k("seqtype", key, 1, from);
        keys_main.push_back(k);
        keys_main_order.push_back(keysCnt++);
        if (!param) createParam();
        pqt.PQputf((PGparam *)param, "%public.seqtype", value.c_str());
        return VT_OK;
    }
}

bool PGQueryBuilder::keyInouttype(const string& key, const string& value, const string& from) {
    if (key.empty() || value.empty() || !this->checkInouttype(value)) return VT_FAIL;
    else {
        TKey k("inouttype", key, 1, from);
        keys_main.push_back(k);
        keys_main_order.push_back(keysCnt++);
        if (!param) createParam();
        pqt.PQputf((PGparam *)param, "%public.inouttype", value.c_str());
        return VT_OK;
    }
}

//bool PGQueryBuilder::keyPermissions(const string& key, const string& value, const string& from) {
//    if (key.empty() || value.empty()) return VT_FAIL;
//    else {
//        TKey k("permissions", key, 1, from);
//        keys_main.push_back(k);
//        keys_main_order.push_back(keysCnt++);
//        if (!param) createParam();
//        pqt.PQputf((PGparam *)param, "%public.permissions", value.c_str());
//        return VT_OK;
//    }
//}

bool PGQueryBuilder::keyTimestamp(const string& key, const time_t& value, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
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
        pqt.PQputf((PGparam *)param, "%timestamp", &timestamp);

        return VT_OK;
    }
}

#if HAVE_OPENCV
bool PGQueryBuilder::keyCvMat(const string& key, const cv::Mat& value, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKey k("cvmat", key, 1, from);
        keys_main.push_back(k);
        keys_main_order.push_back(keysCnt++);
        if (!param) createParam();
        
        // cvmat arguments
        int mat_type        = value.type();
        PGarray mat_dims    = {0};
        PGbytea mat_data    = {(int)(value.dataend-value.datastart), (char*)value.data};

        // create dimensions array
        mat_dims.param = pqt.PQparamCreate((PGconn *)connection);
        for (int i = 0; i < value.dims; i++) {
            pqt.PQputf(mat_dims.param, "%int4", value.size[i]);
        }
        
        // create cvmat composite
        PGparam *cvmat = pqt.PQparamCreate((PGconn *)connection);
        pqt.PQputf(cvmat, "%int4 %int4[] %bytea*",
            (PGint4)mat_type, &mat_dims, &mat_data);
        pqt.PQparamClear(mat_dims.param);
        
        // put cvmat composite
        pqt.PQputf((PGparam *)param, "%public.cvmat", cvmat);
        pqt.PQparamClear(cvmat);
        
        return VT_OK;
    }
}
#endif

bool PGQueryBuilder::keyIntervalEvent(const string& key, const IntervalEvent& value, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKey k("vtevent", key, 1, from);
        keys_main.push_back(k);
        keys_main_order.push_back(keysCnt++);
        if (!param) createParam();
        
        PGbytea data = {value.user_data_size, (char*)value.user_data};
        
        // create interval event composite
        PGparam *event = pqt.PQparamCreate((PGconn *)connection);
        pqt.PQputf(event, "%int4 %int4 %bool %box %float8 %bytea*",
            (PGint4)value.group_id, (PGint4)value.class_id, (PGbool)value.is_root, (PGbox *)&value.region, (PGfloat8)value.score, &data);
        
        // put interval event composite
        pqt.PQputf((PGparam *)param, "%public.vtevent", event);
        pqt.PQparamClear(event);
        
        return VT_OK;
    }
}


bool PGQueryBuilder::whereString(const string& key, const string& value, const string& oper, const string& from) {
    if (key.empty() || value.empty()) return VT_FAIL;
    else {
        TKey k("varchar", key, 1, from);
        string value_put;

        if (value.empty()) return VT_FAIL;

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
        pqt.PQputf((PGparam *)param, "%varchar", value_put.c_str());

        return VT_OK;
    }
}

bool PGQueryBuilder::whereInt(const string& key, const int value, const string& oper, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKey k("int4", key, 1, from);
        keys_where.push_back(k);
        keys_where_order.push_back(keysCnt++);
        opers.push_back(oper);
        if (!param) createParam();
        pqt.PQputf((PGparam *)param, "%int4", value);
        return VT_OK;
    }
}

bool PGQueryBuilder::whereFloat(const string& key, const float value, const string& oper, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKey k("float4", key, 1, from);
        keys_where.push_back(k);
        keys_where_order.push_back(keysCnt++);
        opers.push_back(oper);
        if (!param) createParam();
        pqt.PQputf((PGparam *)param, "%float4", value);
        return VT_OK;
    }
}

bool PGQueryBuilder::whereSeqtype(const string& key, const string& value, const string& oper, const string& from) {
    if (key.empty() || value.empty() || !this->checkSeqtype(value)) return VT_FAIL;
    else {
        TKey k("seqtype", key, 1, from);
        if (value.empty()) return VT_FAIL;
        keys_where.push_back(k);
        keys_where_order.push_back(keysCnt++);
        opers.push_back(oper);
        if (!param) createParam();
        pqt.PQputf((PGparam *)param, "%public.seqtype", value.c_str());
        return VT_OK;
    }
}

bool PGQueryBuilder::whereInouttype(const string& key, const string& value, const string& oper, const string& from) {
    if (key.empty() || value.empty() || !this->checkInouttype(value)) return VT_FAIL;
    else {
        TKey k("inouttype", key, 1, from);
        if (value.empty()) return VT_FAIL;
        keys_where.push_back(k);
        keys_where_order.push_back(keysCnt++);
        opers.push_back(oper);
        if (!param) createParam();
        pqt.PQputf((PGparam *)param, "%public.inouttype", value.c_str());
        return VT_OK;
    }
}

bool PGQueryBuilder::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
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
        pqt.PQputf((PGparam *)param, "%timestamp", &timestamp);

        return VT_OK;
    }
}


void PGQueryBuilder::reset() {
    keys_main.clear();
    keys_where.clear();
    keys_main_order.clear();
    keys_where_order.clear();
    keysCnt = 1;
    opers.clear();
    destroyParam();
}

void PGQueryBuilder::createParam() {
    destroyParam();
    param = (void*)pqt.PQparamCreate((PGconn *)connection);
}

void PGQueryBuilder::destroyParam() {
    if (param) {
        pqt.PQparamClear((PGparam *)param);
        param = NULL;
    }
}

string PGQueryBuilder::escapeColumn(const string& key, const string& table)
{
    string ret;
    
    size_t charPos = key.find_first_of(":[(");
    size_t dotPos = key.find('.');
    
    // escape everything
    if (dotPos != string::npos) {
        ret = escapeIdent(key.substr(0, dotPos)) + '.';
        if (charPos != string::npos) {
            ret += escapeIdent(key.substr(dotPos+1, charPos-dotPos-1)) + key.substr(charPos);
        }
        else {
            ret += escapeIdent(key.substr(dotPos+1));
        }
    }
    else {
        if (!table.empty()) {
            ret = escapeIdent(table) + '.';
        }
        if (charPos != string::npos) {
            ret += escapeIdent(key.substr(0, charPos)) + key.substr(charPos);
        }
        else {
            ret += escapeIdent(key);
        }
    }
    
    return ret;
}

string PGQueryBuilder::escapeAlias(const string& key) {
    return key.substr(0, key.find(':'));
}

string PGQueryBuilder::escapeIdent(const string& ident) {
    char    *ident_c = pg.PQescapeIdentifier((PGconn *)connection, ident.c_str(), ident.length());
    string  escaped  = string(ident_c);
    pg.PQfreemem(ident_c);
    return escaped;
}

string PGQueryBuilder::escapeLiteral(const string& literal) {
    char *literal_c = pg.PQescapeLiteral((PGconn *)connection, literal.c_str(), literal.length());
    string  escaped  = string(literal_c);
    pg.PQfreemem(literal_c);
    return escaped;
}

#endif