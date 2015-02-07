/**
 * @file    backend_sl.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @brief Methods of all SQLite-specific polymorphic classes: SLConnection, SLTypeManager, SLResultSet, SLQueryBuilder and SLLibLoader
 *
 */

#include <common/vtapi_global.h>
#include <common/vtapi_serialize.h>
#include <backends/vtapi_connection.h>
#include <backends/vtapi_libloader.h>
#include <backends/vtapi_querybuilder.h>
#include <backends/vtapi_resultset.h>
#include <backends/vtapi_typemanager.h>

#if HAVE_SQLITE

using std::string;
using std::stringstream;
using std::vector;
using std::pair;

using namespace vtapi;

// sqlite database files
#define SL_DB_PREFIX        "vtapi_"
#define SL_DB_SUFFIX        ".db"
#define SL_DB_PUBLIC        "public"


SLConnection::SLConnection(fmap_t *fmap, const string& connectionInfo, Logger* logger)
: Connection (fmap, connectionInfo, logger){
    thisClass   = "SLConnection";
    conn        = NULL;
    connect(connectionInfo);
}

SLConnection::~SLConnection() {
    disconnect();
}

bool SLConnection::connect (const string& connectionInfo) {
    bool retval     = VT_OK;
    string dbname   = "";

    connInfo    = connectionInfo;
    fixSlashes(connInfo);
    dbname      = connInfo + "/" + SL_DB_PREFIX + SL_DB_PUBLIC + SL_DB_SUFFIX;
    CALL_SL(fmap, sqlite3_open_v2, dbname.c_str(), &conn, SQLITE_OPEN_READWRITE, NULL);

    retval = isConnected();
    if (!retval) {
        string errmsg = conn ? string(CALL_SL(fmap, sqlite3_errmsg, conn)) : "The connection couldn't have been established.";
        logger->error(122, errmsg, thisClass+"::connect(");
    }

    return retval;
}

bool SLConnection::reconnect (const string& connectionInfo) {
    if (!connectionInfo.empty()) {
        connInfo = connectionInfo;
        fixSlashes(connInfo);
    }
    disconnect();
    return connect(connInfo);
}

void SLConnection::disconnect () {
    if (conn) {
        CALL_SL(fmap, sqlite3_close, conn);
    }
}

bool SLConnection::isConnected () {
    int cur, high;
    if (conn) {
        return (CALL_SL(fmap, sqlite3_db_status, conn, SQLITE_DBSTATUS_SCHEMA_USED, &cur, &high, false) == SQLITE_OK);
    }
    else {
        return VT_FAIL;
    }
}

bool SLConnection::execute(const string& query, void *param) {
    sl_param_t  *sl_param   = (sl_param_t *) param;
    char        *errmsg     = NULL;
    bool        retval      = VT_OK;

    errorMessage.clear();

    retval = attachDatabase (sl_param->database);
    if (!retval) {
        errorMessage = "Database " + sl_param->database + " couldn't have been attached.";
    }
    else {
        retval = CALL_SL(fmap, sqlite3_exec, conn, query.c_str(), NULL, NULL, &errmsg) == SQLITE_OK;
        if (!retval && errmsg) {
            errorMessage = string(errmsg);
            CALL_SL(fmap, sqlite3_free, errmsg);
        }
    }

    return retval;
}

int SLConnection::fetch(const string& query, void *param, ResultSet *resultSet) {
    sl_param_t  *sl_param   = (sl_param_t *) param;
    sl_res_t    *sl_res     = new sl_res_t();
    char        *errmsg     = NULL;
    int         retval      = ER_FAIL;
    int         retquery    = SQLITE_ERROR;

    errorMessage.clear();

    if (!attachDatabase (sl_param->database)) {
        errorMessage = "Database " + sl_param->database + " couldn't have been attached.";
    }
    else {
        retquery = CALL_SL(fmap, sqlite3_get_table, conn, query.c_str(), &(sl_res->res), &(sl_res->rows), &(sl_res->cols), &errmsg);
        resultSet->newResult((void *) sl_res);
        if (retquery == SQLITE_OK) {
            retval = sl_res->rows;
        }
        else if (errmsg) {
            errorMessage = string(errmsg);
            CALL_SL(fmap, sqlite3_free, errmsg);
        }
    }

    return retval;
}

void* SLConnection::getConnectionObject() {
    return (void *) this->conn;
}

bool SLConnection::fixSlashes(string& path) {
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
            return VT_FAIL;
        }
    } while (path[nsPos] == '/');
    if (nsPos < len - 1) {
        path = path.substr(0, nsPos + 1);
    }

    return VT_OK;
}

bool SLConnection::attachDatabase(string& dbfile) {
    if (CALL_SL(fmap, sqlite3_db_filename, conn, dbfile.c_str()) == NULL) {
        string query = "ATTACH DATABASE \'" + connInfo + "/" + SL_DB_PREFIX + dbfile + SL_DB_SUFFIX + "\' AS \'" + dbfile + "\';";
        return CALL_SL(fmap, sqlite3_exec, conn, query.c_str(), NULL, NULL, NULL) == SQLITE_OK;
    }
    else {
        return VT_OK;
    }
}



SLTypeManager::SLTypeManager(fmap_t *fmap, Connection *connection, Logger *logger, string& schema)
: TypeManager(fmap, connection, logger, schema) {
    thisClass = "SLTypeManager";
}

SLTypeManager::~SLTypeManager() {
}

bool SLTypeManager::loadTypes() {
    return VT_OK;
}



SLQueryBuilder::SLQueryBuilder(fmap_t *fmap, Connection *connection, TypeManager *typeManager, Logger *logger, const string& initString)
: QueryBuilder (fmap, connection, typeManager, logger, initString) {
    thisClass   = "SLQueryBuilder";
    param       = (void *) new sl_param_t();
}

 SLQueryBuilder::~SLQueryBuilder() {
     sl_param_t *sl_param = (sl_param_t *)param;
     vt_destruct(sl_param);
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
    if (column.empty()) return VT_FAIL;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("", column, "", table);
        key_values_main.push_back(tk);
        return VT_OK;
    }
}

bool SLQueryBuilder::keyString(const string& key, const string& value, const string& from) {
    if (key.empty() || value.empty()) return VT_FAIL;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("text", key, value, from);
        key_values_main.push_back(tk);
        return VT_OK;
    }    
}

bool SLQueryBuilder::keyStringA(const string& key, string* values, const int size, const string& from) {    
    if (key.empty() || !values || size <= 0) return VT_FAIL;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("textA", key, values, size, from);
        key_values_main.push_back(tk);
        return VT_OK;
    }
}

bool SLQueryBuilder::keyInt(const string& key, int value, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKeyValue<int> *tk = new TKeyValue<int>("integer", key, value, from);
        key_values_main.push_back(tk);
        return VT_OK;
    }
}

bool SLQueryBuilder::keyIntA(const string& key, int* values, const int size, const string& from) {
    if (key.empty() || !values || size <= 0) return VT_FAIL;
    else {
        TKeyValue<int> *tk = new TKeyValue<int>("integerA", key, values, size, from);
        key_values_main.push_back(tk);
        return VT_OK;
    }
}

bool SLQueryBuilder::keyFloat(const string& key, float value, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKeyValue<float> *tk = new TKeyValue<float>("float", key, value, from);
        key_values_main.push_back(tk);
        return VT_OK;
    }
}

bool SLQueryBuilder::keyFloatA(const string& key, float* values, const int size, const string& from) {
    if (key.empty() || !values || size <= 0) return VT_FAIL;
    else {
        TKeyValue<float> *tk = new TKeyValue<float>("floatA", key, values, size, from);
        key_values_main.push_back(tk);
        return VT_OK;
    }
}

bool SLQueryBuilder::keySeqtype(const string& key, const string& value, const string& from) {
    if (key.empty() || value.empty() || !this->checkSeqtype(value)) return VT_FAIL;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("seqtype", key, value, from);
        key_values_main.push_back(tk);
        return VT_OK;
    }
}

bool SLQueryBuilder::keyInouttype(const string& key, const string& value, const string& from) {
    if (key.empty() || value.empty() || !this->checkInouttype(value)) return VT_FAIL;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("inouttype", key, value, from);
        key_values_main.push_back(tk);
        return VT_OK;
    }
}

//bool SLQueryBuilder::keyPermissions(const string& key, const string& value, const string& from) {
//    if (key.empty() || value.empty()) return VT_FAIL;
//    else {
//        TKeyValue<string> *tk = new TKeyValue<string>("permissions", key, value, from);
//        key_values_main.push_back(tk);
//        return VT_OK;
//    }
//}

bool SLQueryBuilder::keyTimestamp(const string& key, const time_t& value, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKeyValue<time_t> *tk = new TKeyValue<time_t>("timestamp", key, value, from);
        key_values_main.push_back(tk);
        return VT_OK;
    }
}

#if HAVE_OPENCV
bool SLQueryBuilder::keyCvMat(const std::string& key, const cv::Mat& value, const std::string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKeyValue<cv::Mat> *tk = new TKeyValue<cv::Mat>("cvmat", key, value, from);
        key_values_main.push_back(tk);
        return VT_OK;
    }
}
#endif 

bool SLQueryBuilder::whereString(const string& key, const string& value, const string& oper, const string& from) {
    if (key.empty() || value.empty()) return VT_FAIL;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("text", key, value, from);
        key_values_where.push_back(tk);
        if (value.compare("NULL") == 0 || value.compare("NOT NULL") == 0) opers.push_back(" IS ");
        else opers.push_back(oper);
        return VT_OK;
    }
}

bool SLQueryBuilder::whereInt(const string& key, const int value, const string& oper, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKeyValue<int> *tk = new TKeyValue<int>("integer", key, value, from);
        key_values_where.push_back(tk);
        opers.push_back(oper);
        return VT_OK;
    }
}

bool SLQueryBuilder::whereFloat(const string& key, const float value, const string& oper, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKeyValue<float> *tk = new TKeyValue<float>("float", key, value, from);
        key_values_where.push_back(tk);
        opers.push_back(oper);
        return VT_OK;
    }
}

bool SLQueryBuilder::whereSeqtype(const string& key, const string& value, const string& oper, const string& from) {
    if (key.empty() || value.empty() || !this->checkSeqtype(value)) return VT_FAIL;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("seqtype", key, value, from);
        key_values_where.push_back(tk);
        opers.push_back(oper);
        return VT_OK;
    }
}

bool SLQueryBuilder::whereInouttype(const string& key, const string& value, const string& oper, const string& from) {
    if (key.empty() || value.empty() || !this->checkInouttype(value)) return VT_FAIL;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("inouttype", key, value, from);
        key_values_where.push_back(tk);
        opers.push_back(oper);
        return VT_OK;
    }
}

bool SLQueryBuilder::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKeyValue<time_t> *tk = new TKeyValue<time_t>("timestamp", key, value, from);
        key_values_where.push_back(tk);
        opers.push_back(oper);
        return VT_OK;
    }
}

void SLQueryBuilder::reset() {
    destroyKeys();
    opers.clear();
}

void SLQueryBuilder::createParam() {
}

void SLQueryBuilder::destroyParam() {
}

void SLQueryBuilder::destroyKeys() {
    for (TKeyValues::iterator it = key_values_main.begin(); it != key_values_main.end(); ++it) {
        vt_destruct(*it);
    }
    for (TKeyValues::iterator it = key_values_where.begin(); it != key_values_where.end(); ++it) {
        vt_destruct(*it);
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

SLResultSet::SLResultSet(fmap_t *fmap, TypeManager *typeManager, Logger *logger)
 : ResultSet(fmap, typeManager, logger) {
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
        CALL_SL(fmap, sqlite3_free_table, sl_res->res);
        vt_destruct(sl_res)
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

#if HAVE_OPENCV

cv::Mat *SLResultSet::getCvMat(const int col) {
    cv::Mat *mat = NULL;
//    PGresult *mres;
//    PGarray step_arr;
//    int type, rows, cols, dims, step_size, data_len;
//    char *data_loc;
//    int *step;
//    void *data;
//
//    // get CvMat header structure
//    if (! PQgetf(select->res, this->pos, "%cvmat", col, &mres)) {
//        warning(324, "Value is not a correct cvmat type");
//        return NULL;
//    }
//    // parse CvMat header fields
//    if (! PQgetf(mres, 0, "%int4 %int4 %int4[] %int4 %int4 %name",
//        0, &type, 1, &dims, 2, &step_arr, 3, &rows, 4, &cols, 5, &data_loc)) {
//        warning(325, "Incorrect cvmat type");
//        PQclear(mres);
//        return NULL;
//    }
//    // sometimes data type returns with apostrophes ('type')
//    if (data_loc && data_loc[0] == '\'') {
//        int len = strlen(data_loc);
//        if (data_loc[len-1] == '\'') data_loc[len-1] = '\0';
//        data_loc++;
//    }
//    // construct step[] array
//    step_size = PQntuples(step_arr.res);
//    step = new int [step_size];
//    for (int i = 0; i < step_size; i++) {
//        if (! PQgetf(step_arr.res, i, "%int4", 0, &step[i])) {
//            warning(310, "Unexpected value in int array");
//            vt_destruct(step);
//            PQclear(step_arr.res);
//            PQclear(mres);
//            return NULL;
//        }
//    }
//    PQclear(step_arr.res);
//
//    // get matrix data from specified column
//    int dataloc_col = PQfnumber(select->res, data_loc);
//    int data_oid;
//    if (dataloc_col < 0) {
//        warning(325, "Invalid column for CvMat user data");
//        data = NULL;
//    }
//    else data_oid = typeManager->getElemOID(PQftype(select->res, dataloc_col));
//
//    // could be char, short, int, float, double
//    if (data_oid == typeManager->toOid("char")) {
//        //TODO: maybe fix alignment (every row to 4B) ?
//        data = getCharA(dataloc_col, data_len);
//    }
//    else if (data_oid == typeManager->toOid("float4") ||
//            data_oid == typeManager->toOid("real")) {
//        data = getFloatA(dataloc_col, data_len);
//    }
//    else {
//        warning(326, "Unexpected type of CvMat data");
//        data = NULL;
//    }
//    // create CvMat header and set user data
//    if (dims > 0 && data && step) {
//        mat = cvCreateMatHeader(rows, cols, type);
//        cvSetData(mat, data, step[dims-1]);
//    }
//    vt_destruct(step);
//    PQclear(mres);

    return mat;
}
#endif

    // =============== GETTERS - GEOMETRIC TYPES ===============================
#if HAVE_POSTGRESQL
PGpoint SLResultSet::getPoint(const int col) {
    PGpoint point = { 0.0, 0.0 };
//    memset(&point, 0, sizeof(PGpoint));
//    if (! PQgetf(select->res, this->pos, "%point", col, &point)) {
//        logger->warning(314, "Value is not a point");
//    }
    return point;
}
vector<PGpoint>*  SLResultSet::getPointV(const int col) {
    return NULL;
}
#endif

#if HAVE_POSTGIS
GEOSGeometry* SLResultSet::getGeometry(const int col) {
    return NULL;
}
GEOSGeometry* SLResultSet::getLineString(const int col) {
    return NULL;
}
#endif

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
        vt_destruct(widths);
        vt_destruct(keys);
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
        vt_destruct(widths);
        vt_destruct(keys);
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

fmap_t *SLLibLoader::loadLibs() {
    fmap_t *fmap = new fmap_t();

    if (load_libsqlite(fmap)) {
        return fmap;
    }
    else {
        vt_destruct(fmap);
        return NULL;
    }
};

void SLLibLoader::unloadLibs() {
    unload_libsqlite();
};

bool SLLibLoader:: isLoaded() {
    return h_libsqlite ? VT_OK : VT_FAIL;
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

bool SLLibLoader::load_libsqlite (fmap_t *fmap) {
    bool retval     = VT_OK;
    void *funcPtr   = NULL;
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
                fmap->insert(FMAP_ENTRY("SL_"+funcStrings[i], funcPtr));
            }
            else {
                logger->warning(555, "Function " + funcStrings[i] + " not loaded.", thisClass+"::load_libsqlite()");
                retval = VT_FAIL;
                break;
            }
        }
    }
    else {
        logger->warning(556, "Libsqlite library not found.", thisClass+"::load_libsl()");
        retval = VT_FAIL;
    }

    return retval;
}

void SLLibLoader::unload_libsqlite () {
    if (h_libsqlite) {
        lt_dlclose(h_libsqlite);
        h_libsqlite = NULL;
    }
}

#endif