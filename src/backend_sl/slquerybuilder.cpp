
#include <common/vtapi_global.h>
#include <backends/vtapi_querybuilder.h>


#if HAVE_SQLITE

using std::string;
using std::stringstream;

using namespace vtapi;


SLQueryBuilder::SLQueryBuilder(const SLBackendBase &base, void *connection, const string& initString) :
    QueryBuilder (connection, initString),
    SLBackendBase(base)
{
    thisClass   = "SLQueryBuilder";
    param       = (void *) new SLparam();
}

 SLQueryBuilder::~SLQueryBuilder() {
     if (param) delete ((SLparam *) param);
     destroyKeys();
}

string SLQueryBuilder::getSelectQuery(const string& groupby, const string& orderby, const int limit, const int offset) {
    string queryString;
    string columnsStr;
    string tablesStr;
    string whereStr;

    ((SLparam *)param)->database = this->dataset;
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

    ((SLparam *)param)->database = this->dataset;
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

    ((SLparam *)param)->database = this->dataset;
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
    ((SLparam *)param)->database = this->dataset;
    return initString;
}

string SLQueryBuilder::getBeginQuery()
{
    return "BEGIN TRANSACTION;";
}
string SLQueryBuilder::getCommitQuery()
{
    return "COMMIT TRANSACTION;";
}
string SLQueryBuilder::getRollbackQuery()
{
    return "ROLLBACK TRANSACTION;";
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

bool SLQueryBuilder::keyIntervalEvent(const std::string& key, const IntervalEvent& value, const std::string& from) {
    if (key.empty()) return VT_FAIL;
    else {
        TKeyValue<IntervalEvent> *tk = new TKeyValue<IntervalEvent>("vtevent", key, value, from);
        key_values_main.push_back(tk);
        return VT_OK;
    }
}

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

#endif