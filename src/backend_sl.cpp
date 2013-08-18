/**
 * @file    backend_sl.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of all SQLite-specific polymorphic classes:
 *  - SLConnection, SLTypeManager, SLResultSet, SLQueryBuilder and SLLibLoader
 *
 */

#include "vtapi_backends.h"

using namespace vtapi;


SLConnection::SLConnection(func_map_t *FUNC_MAP, const string& connectionInfo, Logger* logger)
: Connection (FUNC_MAP, connectionInfo, logger){
    thisClass   = "SLConnection";
    conn        = NULL;
    if (!this->connect(connectionInfo)) {
        logger->error(121, "The connection couldn't have been established.", thisClass+"::SLConnection()");
    }
    this->isConnected();
}

SLConnection::~SLConnection() {
    disconnect();
}

bool SLConnection::connect (const string& connectionInfo) {
    string dbname;
    connInfo    = connectionInfo;
    fixSlashes(connInfo);
    dbname      = connInfo + "/" + SL_DB_PREFIX + SL_DB_PUBLIC + SL_DB_SUFFIX;

    if (CALL_SL(sqlite3_open_v2)(dbname.c_str(), &conn, SQLITE_OPEN_READWRITE, NULL) != SQLITE_OK) {
        logger->warning(122, CALL_SL(sqlite3_errmsg)(conn), thisClass+"::connect(");
        return false;
    }
    else {
        return true;
    }
}

bool SLConnection::reconnect (const string& connectionInfo) {
    if (!connectionInfo.empty()) {
        connInfo = connectionInfo;
        fixSlashes(connInfo);
    }
    disconnect();
    return this->connect(connInfo);
}

void SLConnection::disconnect () {
    if (isConnected()) {
        CALL_SL(sqlite3_close)(conn);
    }
}

bool SLConnection::isConnected () {
    int cur, high;
    if (CALL_SL(sqlite3_db_status)(conn, SQLITE_DBSTATUS_SCHEMA_USED, &cur, &high, false) != SQLITE_OK) {
        logger->warning(125, CALL_SL(sqlite3_errmsg)(conn), thisClass+"::isConnected()");
        return false;
    }
    else {
        return true;
    }
}

int SLConnection::execute(const string& query, void *param) {
    sl_param_t      *sl_param   = (sl_param_t *) param;
    char            *errmsg     = NULL;
    int             ret_query   = 0;

    errorMessage.clear();

    ret_query = attachDatabase (sl_param->database);
    if (ret_query != SQLITE_OK) {
        errorMessage = "Database " + sl_param->database + " couldn't have been attached.";
        return -1;
    }
    ret_query = CALL_SL(sqlite3_exec)(conn, query.c_str(), NULL, NULL, &errmsg);
    if (ret_query == SQLITE_OK) {
        return 1;
    }
    else {
        if (errmsg) {
            errorMessage = string(errmsg);
            CALL_SL(sqlite3_free)(errmsg);
        }
        return -1;
    }
}

int SLConnection::fetch(const string& query, void *param, ResultSet *resultSet) {
    sl_param_t      *sl_param   = (sl_param_t *) param;
    sl_res_t        *sl_res     = new sl_res_t();
    char            *errmsg     = NULL;
    int             ret_query   = 0;

    errorMessage.clear();

    ret_query = attachDatabase (sl_param->database);
    if (ret_query != SQLITE_OK) {
        errorMessage = "Database " + sl_param->database + " couldn't have been attached.";
        return -1;
    }

    ret_query = CALL_SL(sqlite3_get_table)(conn, query.c_str(), &(sl_res->res), &(sl_res->rows), &(sl_res->cols), &errmsg);
    resultSet->newResult((void *) sl_res);
    if (ret_query == SQLITE_OK) {
        return sl_res->rows;
    }
    else {
        if (errmsg) {
            errorMessage = string(errmsg);
            CALL_SL(sqlite3_free)(errmsg);
        }
        return -1;
    }
}

void* SLConnection::getConnectionObject() {
    return (void *) this->conn;
}

int SLConnection::fixSlashes(string& path) {
    size_t len = path.length();
    size_t slPos = 0;
    size_t nsPos = len;

    do {
        slPos = path.find('\\', slPos);
        if (slPos != string::npos) {
            path[slPos] = '/';
        }
        else {
            break;
        }
    } while (1);
    do {
        nsPos--;
        if (nsPos < 0) {
            path.clear();
            return -1;
        }
    } while (path[nsPos] == '/' || path[nsPos] == '\\');
    if (nsPos < len - 1) {
        path = path.substr(0, nsPos + 1);
    }
    return 0;
}

int SLConnection::attachDatabase(string& db) {
    if (!CALL_SL(sqlite3_db_filename)(conn, db.c_str())) {
        string query = "ATTACH DATABASE \'" + connInfo + "/" + SL_DB_PREFIX + db + SL_DB_SUFFIX + "\' AS \'" + db + "\';";
        return CALL_SL(sqlite3_exec)(conn, query.c_str(), NULL, NULL, NULL);
    }
    else {
        return SQLITE_OK;
    }
}



SLTypeManager::SLTypeManager(func_map_t *FUNC_MAP, Connection *connection, Logger *logger)
: TypeManager(FUNC_MAP, connection, logger) {
    thisClass = "SLTypeManager";
}

SLTypeManager::~SLTypeManager() {
}

int SLTypeManager::loadTypes() {
    return 0;
}



SLQueryBuilder::SLQueryBuilder(func_map_t *FUNC_MAP, Connection *connection, Logger *logger, const string& initString)
: QueryBuilder (FUNC_MAP, connection, logger, initString) {
    thisClass   = "SLQueryBuilder";
    param       = (void *) new sl_param_t();
}

 SLQueryBuilder::~SLQueryBuilder() {
     sl_param_t *sl_param = (sl_param_t *)param;
     destruct(sl_param);
     destroyKeys();
}

string SLQueryBuilder::getSelectQuery(const string& groupby, const string& orderby, const int limit, const int offset) {
    string queryString;
    string columnsStr;
    string tablesStr;
    string whereStr;

    ((sl_param_t *)param)->database = this->dataset;
    if (this->key_values_main.empty()) return initString; // in case of a direct query

    // go through keys
    for (int i = 0; i < key_values_main.size(); i++) {
        string tmpTable  = !key_values_main[i]->from.empty() ? key_values_main[i]->from : this->table;
        string tmpColumn = key_values_main[i]->key;
        size_t dotPos    = tmpTable.find(".");
        bool addTable    = true;

        // get escaped table
        if (dotPos != string::npos && dotPos != tmpTable.length()-1) {
            tmpTable = tmpTable.substr(dotPos+1, string::npos);
        }
        tmpTable = this->escapeIdent(tmpTable);
        // get and add escaped column
        if (tmpColumn.empty() || tmpColumn.compare("*") == 0) {
            columnsStr += tmpTable + ".*, ";
        }
        else {
            columnsStr += tmpTable + "." + this->escapeIdent(tmpColumn);
            columnsStr += " AS " + this->escapeLiteral(tmpColumn) + ", ";
        }
        // check if table already exists
        for (int j = 0; j < i; j++) {
            if (key_values_main[i]->from.compare(key_values_main[j]->from) == 0 ||
               (key_values_main[i]->from.empty() && key_values_main[j]->from.empty())) {
                addTable = false;
                break;
            }
        }
        // add table
        if (addTable) {
            tablesStr += tmpTable+ ", ";
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
    for (int i = 0; i < key_values_where.size(); i++) {
        if (!whereStr.empty()) whereStr += " AND ";
        whereStr += this->escapeColumn(key_values_where[i]->key, key_values_where[i]->from);
        whereStr += opers[i];
        whereStr += escapeLiteral(key_values_where[i]->getValue());
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

string SLQueryBuilder::getInsertQuery() {
    string queryString;
    string dstTable;
    string intoStr;
    string valuesStr;
    size_t dotPos;

    ((sl_param_t *)param)->database = this->dataset;
    if (this->key_values_main.empty()) return initString; // in case of a direct query

    // in case we're lazy, we have the table specified in initString or selection
    dstTable = (!initString.empty()) ? initString : this->table;

    // go through keys
    for (int i = 0; i < this->key_values_main.size(); ++i) {
        if (dstTable.empty()) dstTable = this->key_values_main[i]->from;
        intoStr     += escapeIdent(this->key_values_main[i]->key) + ", ";
        valuesStr   += escapeLiteral(key_values_main[i]->getValues()) + ", ";
    }
    // this is to remove ending separators
    intoStr.erase(intoStr.length()-2);
    valuesStr.erase(valuesStr.length()-2);

    dotPos = dstTable.find(".");
    // add the dataset selected and escape table
    if (dotPos == string::npos) {
        dstTable = escapeColumn(dstTable, this->dataset);
    }
    else {
        dstTable = escapeColumn(dstTable.substr(dotPos+1, string::npos), this->dataset);
    }
    queryString = "INSERT INTO " + dstTable + " (" + intoStr + ")\n VALUES (" + valuesStr + ");";
    return queryString;
    return "";
}

string SLQueryBuilder::getUpdateQuery() {
    string queryString;
    string dstTable;
    string setStr;
    string whereStr;
    size_t dotPos;

    ((sl_param_t *)param)->database = this->dataset;
    if (this->key_values_main.empty()) return initString; // in case of a direct query

    // in case we're lazy, we have the table specified in initString or selection
    dstTable = (!initString.empty()) ? initString : this->table;

    // go through keys
    for (int i = 0; i < this->key_values_main.size(); ++i) {
        if (dstTable.empty()) dstTable = this->key_values_main[i]->from;
        setStr  += escapeIdent(this->key_values_main[i]->key);
        setStr  += "=" + escapeLiteral(key_values_main[i]->getValues()) + ", ";
    }
    // this is to remove ending separators
    setStr.erase(setStr.length()-2);

    dotPos = dstTable.find(".");
    // add the dataset selected and escape table
    if (dotPos == string::npos) {
        dstTable = escapeColumn(dstTable, this->dataset);
    }
    else {
        dstTable = escapeColumn(dstTable.substr(dotPos+1, string::npos), this->dataset);
    }
    //construct main part of the query
    queryString = "UPDATE " + dstTable + "\n SET " + setStr;
    // construct WHERE clause
    for (int i = 0; i < key_values_where.size(); i++) {
        if (!whereStr.empty()) whereStr += " AND ";
        whereStr += this->escapeColumn(key_values_where[i]->key, key_values_where[i]->from);
        whereStr += opers[i];
        whereStr += escapeLiteral(key_values_where[i]->getValue());
    }
    if (!whereStr.empty()) {
        queryString += "\n WHERE " + whereStr;
    }
    queryString += ";";
    return queryString;
}

string SLQueryBuilder::getGenericQuery() {
    ((sl_param_t *)param)->database = this->dataset;
    return initString;
}

bool SLQueryBuilder::keyFrom(const string& table, const string& column) {
    if (column.empty()) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("", column, "", table);
        key_values_main.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyString(const string& key, const string& value, const string& from) {
    if (key.empty() || value.empty()) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("text", key, value, from);
        key_values_main.push_back(tk);
        return true;
    }    
}

bool SLQueryBuilder::keyStringA(const string& key, string* values, const int size, const string& from) {    
    if (key.empty() || !values || size <= 0) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("textA", key, values, size, from);
        key_values_main.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyInt(const string& key, int value, const string& from) {
    if (key.empty()) return false;
    else {
        TKeyValue<int> *tk = new TKeyValue<int>("integer", key, value, from);
        key_values_main.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyIntA(const string& key, int* values, const int size, const string& from) {
    if (key.empty() || !values || size <= 0) return false;
    else {
        TKeyValue<int> *tk = new TKeyValue<int>("integerA", key, values, size, from);
        key_values_main.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyFloat(const string& key, float value, const string& from) {
    if (key.empty()) return false;
    else {
        TKeyValue<float> *tk = new TKeyValue<float>("float", key, value, from);
        key_values_main.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyFloatA(const string& key, float* values, const int size, const string& from) {
    if (key.empty() || !values || size <= 0) return false;
    else {
        TKeyValue<float> *tk = new TKeyValue<float>("floatA", key, values, size, from);
        key_values_main.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keySeqtype(const string& key, const string& value, const string& from) {
    if (key.empty() || value.empty() || !this->checkSeqtype(value)) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("seqtype", key, value, from);
        key_values_main.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyInouttype(const string& key, const string& value, const string& from) {
    if (key.empty() || value.empty() || !this->checkInouttype(value)) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("inouttype", key, value, from);
        key_values_main.push_back(tk);
        return true;
    }
}

//bool SLQueryBuilder::keyPermissions(const string& key, const string& value, const string& from) {
//    if (key.empty() || value.empty()) return false;
//    else {
//        TKeyValue<string> *tk = new TKeyValue<string>("permissions", key, value, from);
//        key_values_main.push_back(tk);
//        return true;
//    }
//}

bool SLQueryBuilder::keyTimestamp(const string& key, const time_t& value, const string& from) {
    if (key.empty()) return false;
    else {
        TKeyValue<time_t> *tk = new TKeyValue<time_t>("timestamp", key, value, from);
        key_values_main.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::whereString(const string& key, const string& value, const string& oper, const string& from) {
    if (key.empty() || value.empty()) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("text", key, value, from);
        key_values_where.push_back(tk);
        if (value.compare("NULL") == 0 || value.compare("NOT NULL") == 0) opers.push_back(" IS ");
        else opers.push_back(oper);
        return true;
    }
}

bool SLQueryBuilder::whereInt(const string& key, const int value, const string& oper, const string& from) {
    if (key.empty()) return false;
    else {
        TKeyValue<int> *tk = new TKeyValue<int>("integer", key, value, from);
        key_values_where.push_back(tk);
        opers.push_back(oper);
        return true;
    }
}

bool SLQueryBuilder::whereFloat(const string& key, const float value, const string& oper, const string& from) {
    if (key.empty()) return false;
    else {
        TKeyValue<float> *tk = new TKeyValue<float>("float", key, value, from);
        key_values_where.push_back(tk);
        opers.push_back(oper);
        return true;
    }
}

bool SLQueryBuilder::whereSeqtype(const string& key, const string& value, const string& oper, const string& from) {
    if (key.empty() || value.empty() || !this->checkSeqtype(value)) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("seqtype", key, value, from);
        key_values_where.push_back(tk);
        opers.push_back(oper);
        return true;
    }
}

bool SLQueryBuilder::whereInouttype(const string& key, const string& value, const string& oper, const string& from) {
    if (key.empty() || value.empty() || !this->checkInouttype(value)) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("inouttype", key, value, from);
        key_values_where.push_back(tk);
        opers.push_back(oper);
        return true;
    }
}

bool SLQueryBuilder::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from) {
    if (key.empty()) return false;
    else {
        TKeyValue<time_t> *tk = new TKeyValue<time_t>("timestamp", key, value, from);
        key_values_where.push_back(tk);
        opers.push_back(oper);
        return true;
    }
}

bool SLQueryBuilder::reset() {
    destroyKeys();
    opers.clear();
}

void SLQueryBuilder::createParam() {
}

void SLQueryBuilder::destroyParam() {
}

bool SLQueryBuilder::checkSeqtype(const string& value) {
    return (value.compare("images") == 0) || (value.compare("video") == 0) || (value.compare("data") == 0);
}

bool SLQueryBuilder::checkInouttype(const string& value) {
    return (value.compare("in") == 0) || (value.compare("inout") == 0) || (value.compare("out") == 0);
}

void SLQueryBuilder::destroyKeys() {
    for (TKeyValues::iterator it = key_values_main.begin(); it != key_values_main.end(); ++it) {
        destruct(*it);
    }
    for (TKeyValues::iterator it = key_values_where.begin(); it != key_values_where.end(); ++it) {
        destruct(*it);
    }
}

string SLQueryBuilder::escapeColumn(const string& key, const string& table) {
    if (table.empty()) return escapeIdent(key);
    else return escapeIdent(table) + "." + escapeIdent(key);
}

string SLQueryBuilder::escapeIdent(const string& ident) {
    return "[" + ident + "]"; 
}

string SLQueryBuilder::escapeLiteral(const string& ident) {
    string escaped  = ident;
    for (int i = 0; i < escaped.length(); ++i) {
        if (escaped.at(i) == '\'') {
            escaped.insert(i, "\'");
            i++;
        }
    }
    return "\'" + escaped + "\'";
}

///**********************************************************************/

SLResultSet::SLResultSet(func_map_t *FUNC_MAP, TypeManager *typeManager, Logger *logger)
 : ResultSet(FUNC_MAP, typeManager, logger) {
    thisClass = "SLResultSet";
}

SLResultSet::~SLResultSet() {
    clear();
}

void SLResultSet::newResult(void *res) {
    clear();
    this->res = res;
}

int SLResultSet::countRows() {
    return ((sl_res_t *)this->res)->rows;
}
int SLResultSet::countCols() {
    return ((sl_res_t *)this->res)->cols;
}
bool SLResultSet::isOk() {
    return (this->res) ? true : false;
}
void SLResultSet::clear() {
    if (this->res) {
        sl_res_t *sl_res = (sl_res_t *) this->res;
        CALL_SL(sqlite3_free_table)(sl_res->res);
        destruct(sl_res)
    }
}

TKey SLResultSet::getKey(int col) {
    sl_res_t *sl_res = (sl_res_t *) this->res;
    return TKey("", sl_res->res[(col*this->pos)+col], 0);
}

TKeys* SLResultSet::getKeys() {
    sl_res_t *sl_res = (sl_res_t *) this->res;
    TKeys* keys = new TKeys;

    for (int col = 0; col < sl_res->cols; col++) {
        keys->push_back(getKey(col));
    }
    return keys;
}

string SLResultSet::getKeyType(const int col) {
    sl_res_t    *sl_res = (sl_res_t *) this->res;
    char        *val_c  = sl_res->res[col];
    return (val_c) ? string(val_c) : "";
}

int SLResultSet::getKeyIndex(const string& key) {
    sl_res_t    *sl_res = (sl_res_t *) this->res;
    for (int col = 0; col < sl_res->cols; col++) {
        if (string(sl_res->res[col]).compare(key) == 0) {
            return col;
        }
    }
    return -1;
}

//// =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===================
char SLResultSet::getChar(const int col) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return '\0';
    else return *(sl_res->res[sl_res_i]);
}

char *SLResultSet::getCharA(const int col, int& size) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return NULL;
    else return sl_res->res[sl_res_i];
}

string SLResultSet::getString(const int col) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return "";
    else return sl_res->res[sl_res_i];
}

//// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================

int SLResultSet::getInt(const int col) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    int         ret         = 0;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return 0;
    else {
        stringstream(sl_res->res[sl_res_i]) >> ret;
        return ret;
    }
}

long SLResultSet::getInt8(const int col) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    long        ret         = 0;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return 0;
    else {
        stringstream(sl_res->res[sl_res_i]) >> ret;
        return ret;
    }
}

int* SLResultSet::getIntA(const int col, int& size) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return NULL;
    else {
        return deserializeA<int>(sl_res->res[sl_res_i], size);
    }
}

vector<int>* SLResultSet::getIntV(const int col) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return NULL;
    else {
        return deserializeV<int>(sl_res->res[sl_res_i]);
    }
}

vector< vector<int>* >* SLResultSet::getIntVV(const int col) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return NULL;
    else {
        vector< vector<int>* >* arrays = new vector< vector<int>* >;
        arrays->push_back(deserializeV<int>(sl_res->res[sl_res_i]));
    }


}

//// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================

float SLResultSet::getFloat(const int col) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    char        *val_c      = sl_res->res[sl_res_i];
    float       ret         = 0;
    if (val_c && col >= 0)  stringstream (val_c) >> ret;
    return ret;
}

double SLResultSet::getFloat8(const int col) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    char        *val_c      = sl_res->res[sl_res_i];
    double      ret         = 0;
    if (val_c && col >= 0)  stringstream (val_c) >> ret;
    return ret;
}

float* SLResultSet::getFloatA(const int col, int& size) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return NULL;
    else {
        return deserializeA<float>(sl_res->res[sl_res_i], size);
    }
}

vector<float>* SLResultSet::getFloatV(const int col) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return NULL;
    else {
        return deserializeV<float>(sl_res->res[sl_res_i]);
    }
}

//// =============== GETTERS - TIMESTAMP =========================================

time_t SLResultSet::getTimestamp(const int col) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    time_t      ret         = 0;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return ret;
    else return toTimestamp(sl_res->res[sl_res_i]);
}

int SLResultSet::getIntOid(const int col) {
    sl_res_t    *sl_res     = (sl_res_t *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    int         ret         = 0;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return 0;
    else {
        stringstream(sl_res->res[sl_res_i]) >> ret;
        return ret;
    }
}

string SLResultSet::getValue(const int col, const int arrayLimit) {
    sl_res_t    *sl_res = (sl_res_t *) this->res;
    char        *val_c  = sl_res->res[(sl_res->cols)*(this->pos+1)+col];
    string      value   = (val_c && col >= 0) ? val_c : "";
    size_t      comPos  = 0;

    if (arrayLimit > 0) {
        for (int lim = 0; lim < arrayLimit; lim++) {
            comPos = value.find(',', comPos);
            if (comPos == string::npos) break;
        }
        value = value.substr(0, comPos);
    }
    return value;
}



pair< TKeys*, vector<int>* > SLResultSet::getKeysWidths(const int row, bool get_widths, const int arrayLimit) {
    sl_res_t *sl_res    = (sl_res_t *) this->res;
    vector<int> *widths = get_widths ? new vector<int>() : NULL;
    TKeys *keys         = getKeys();

    if (!get_widths && keys) return std::make_pair(keys, widths);
    else if (!widths || !keys || sl_res->cols != keys->size() || sl_res->cols == 0 || sl_res->rows == 0) {
        destruct(widths);
        destruct(keys);
        return std::make_pair((TKeys*)NULL, (vector<int>*)NULL);
    }

    for (int col = 0; col < sl_res->cols; col++) {
        int max_width = 0;
        int col_width = 0;
        size_t comPos = 0;
        for (int row = 0; row < sl_res->rows+1; row++) {
            char *val_c     = sl_res->res[(sl_res->cols)*row+col];
            string value    = val_c ? val_c : "";
            if (arrayLimit > 0) {
                for (int lim = 0; lim < arrayLimit; lim++) {
                    comPos = value.find(',', comPos);
                    if (comPos == string::npos) break;
                    comPos++;
                }
                col_width = value.substr(0, comPos).length();
            }
            else {
                col_width = value.length();
            }
            comPos      = 0;
            max_width   = max_width > col_width ? max_width : col_width;
        }
        widths->push_back(max_width);
    }

    if (widths->size() != keys->size()) {
        destruct(widths);
        destruct(keys);
        return std::make_pair((TKeys*)NULL, (vector<int>*)NULL);
    }
    else return std::make_pair(keys, widths);
}



SLLibLoader::SLLibLoader(Logger *logger)
 : LibLoader (logger) {
    thisClass = "SLLibLoader";
    h_libsqlite = NULL;
    lt_dlinit();
}

SLLibLoader::~SLLibLoader() {
    unloadLibs();
    lt_dlexit();
};

func_map_t *SLLibLoader::loadLibs() {
    func_map_t *func_map = new func_map_t();

    if (load_libsqlite(func_map) == 0) {
        return func_map;
    }
    else {
        destruct(func_map);
        return NULL;
    }
};

int SLLibLoader::unloadLibs() {
    return unload_libsqlite();;
};

bool SLLibLoader:: isLoaded() {
    return h_libsqlite;
}

char *SLLibLoader::getLibName() {
    char *name = NULL;
    const lt_dlinfo *libsl_info;

    if (h_libsqlite) {
        libsl_info = lt_dlgetinfo(h_libsqlite);
        name = libsl_info->name;
    }
    return name;
}

int SLLibLoader::load_libsqlite (func_map_t *func_map) {
    int retval = 0;
    void *funcPtr = NULL;
    const string funcStrings[] =
    {"sqlite3_open_v2","sqlite3_close","sqlite3_errmsg","sqlite3_db_status","sqlite3_get_table",
     "sqlite3_db_filename","sqlite3_exec","sqlite3_free","sqlite3_free_table",
     ""
    };

    h_libsqlite = lt_dlopenext("libsqlite3");

    if (h_libsqlite) {
        for(int i = 0; !funcStrings[i].empty(); i++) {
            funcPtr = lt_dlsym(h_libsqlite, funcStrings[i].c_str());
            if (funcPtr) {
                func_map->insert(FUNC_MAP_ENTRY("SL_"+funcStrings[i], funcPtr));
            }
            else {
                logger->warning(555, "Function " + funcStrings[i] + " not loaded.", thisClass+"::load_libsqlite()");
                retval = -1;
                break;
            }
        }
    }
    else {
        logger->warning(556, "Libsqlite library not found.", thisClass+"::load_libsl()");
        retval = -1;
    }
    return retval;
}

int SLLibLoader::unload_libsqlite () {
    if (h_libsqlite) {
        lt_dlclose(h_libsqlite);
        h_libsqlite = NULL;
        return 0;
    }
    else {
        return -1;
    }
}