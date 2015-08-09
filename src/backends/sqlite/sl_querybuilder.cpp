
#include <vtapi/common/vtapi_global.h>
#include "sl_querybuilder.h"

#define DEF_NO_SCHEMA   "!NO_SCHEMA!"
#define DEF_NO_TABLE    "!NO_TABLE!"
#define DEF_NO_COLUMN   "!NO_COLUMN!"
#define DEF_NO_VALUES   "!NO_VALUES!"
#define DEF_NO_QUERY    "!NO_QUERY!"


using namespace std;

namespace vtapi {

SLQueryBuilder::SLQueryBuilder(SLConnection &connection, const string& init_string)
    : QueryBuilder (connection, init_string)
{
    _pquery_param  = createQueryParam();
}

SLQueryBuilder::~SLQueryBuilder()
{
    destroyQueryParam(_pquery_param);
    destroyKeys();
}

void SLQueryBuilder::reset()
{
    _opers.clear();
    destroyKeys();
    destroyQueryParam(_pquery_param);
}

void *SLQueryBuilder::createQueryParam()
{
    return (void*) new SLparam();
}

void SLQueryBuilder::destroyQueryParam(void *param)
{
    if (param) {
        delete (SLparam *) param;
    }
}

void *SLQueryBuilder::duplicateQueryParam(void *param)
{
    if (param) {
        SLparam *p = (SLparam *) createQueryParam();
        if (p) p->database = ((SLparam *) param)->database;
        return p;
    }
    else {
        return NULL;
    }
}

void SLQueryBuilder::destroyKeys()
{
    for (TKeyValues::iterator it = _keyValuesMain.begin(); it != _keyValuesMain.end(); ++it) {
        vt_destruct(*it);
    }
    for (TKeyValues::iterator it = _keyValuesWhere.begin(); it != _keyValuesWhere.end(); ++it) {
        vt_destruct(*it);
    }
}

string SLQueryBuilder::getGenericQuery()
{
    ((SLparam *) _pquery_param)->database = _defaultSchema;
    return _init_string;
}

string SLQueryBuilder::getSelectQuery(const string& groupby, const string& orderby,
                                      const int limit, const int offset)
{
    string queryString;
    string columnsStr;
    string tablesStr;
    string whereStr;

    ((SLparam *) _pquery_param)->database = _defaultSchema;
    if (this->_keyValuesMain.empty()) return _init_string; // in case of a direct query

    // go through keys
    for (int i = 0; i < _keyValuesMain.size(); i++) {
        string tmpTable  = !_keyValuesMain[i]->m_from.empty() ? _keyValuesMain[i]->m_from : _defaultTable;
        string tmpColumn = _keyValuesMain[i]->m_key;
        size_t dotPos    = tmpTable.find(".");
        bool addTable    = true;

        // get escaped table
        if (dotPos != string::npos && dotPos != tmpTable.length() - 1) {
            tmpTable = tmpTable.substr(dotPos + 1, string::npos);
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
            if (_keyValuesMain[i]->m_from.compare(_keyValuesMain[j]->m_from) == 0 ||
            (_keyValuesMain[i]->m_from.empty() && _keyValuesMain[j]->m_from.empty())) {
                addTable = false;
                break;
            }
        }
        // add table
        if (addTable) {
            tablesStr += tmpTable + ", ";
        }
    }
    // erase commas
    if (!columnsStr.empty())    columnsStr.erase(columnsStr.length() - 2);
    if (!tablesStr.empty())     tablesStr.erase(tablesStr.length() - 2);

    // construct main part of the query
    queryString = "SELECT " + columnsStr + "\n FROM " + tablesStr;
    if (tablesStr.empty() || columnsStr.empty()) {
        VTLOG_ERROR("Incomplete query : " + queryString);
    }
    // construct WHERE and the rest of it all
    for (int i = 0; i < _keyValuesWhere.size(); i++) {
        if (!whereStr.empty()) whereStr += " AND ";
        whereStr += this->escapeColumn(_keyValuesWhere[i]->m_key, _keyValuesWhere[i]->m_from);
        whereStr += _opers[i];
        whereStr += escapeLiteral(_keyValuesWhere[i]->getValue());
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
        queryString += "\n LIMIT " + toString<int>(limit);
    }

    if (offset > 0) {
        queryString += "\n OFFSET " + toString<int>(offset);
    }
    queryString += ";";
    return (queryString);
}

string SLQueryBuilder::getInsertQuery()
{
    string queryString;
    string dstTable;
    string intoStr;
    string valuesStr;
    size_t dotPos;

    ((SLparam *) _pquery_param)->database = _defaultSchema;
    if (this->_keyValuesMain.empty()) return _init_string; // in case of a direct query

    // in case we're lazy, we have the table specified in initString or selection
    dstTable = (!_init_string.empty()) ? _init_string : _defaultTable;

    // go through keys
    for (int i = 0; i < this->_keyValuesMain.size(); ++i) {
        if (dstTable.empty()) dstTable = this->_keyValuesMain[i]->m_from;
        intoStr     += escapeIdent(this->_keyValuesMain[i]->m_key) + ", ";
        valuesStr   += escapeLiteral(_keyValuesMain[i]->getValue()) + ", ";
    }
    // this is to remove ending separators
    intoStr.erase(intoStr.length() - 2);
    valuesStr.erase(valuesStr.length() - 2);

    dotPos = dstTable.find(".");
    // add the dataset selected and escape table
    if (dotPos == string::npos) {
        dstTable = escapeColumn(dstTable, _defaultSchema);
    }
    else {
        dstTable = escapeColumn(dstTable.substr(dotPos + 1, string::npos), _defaultSchema);
    }
    queryString = "INSERT INTO " + dstTable + " (" + intoStr + ")\n VALUES (" + valuesStr + ");";
    return queryString;
    return "";
}

string SLQueryBuilder::getUpdateQuery()
{
    string queryString;
    string dstTable;
    string setStr;
    string whereStr;
    size_t dotPos;

    ((SLparam *) _pquery_param)->database = _defaultSchema;
    if (this->_keyValuesMain.empty()) return _init_string; // in case of a direct query

    // in case we're lazy, we have the table specified in initString or selection
    dstTable = (!_init_string.empty()) ? _init_string : _defaultTable;

    // go through keys
    for (int i = 0; i < this->_keyValuesMain.size(); ++i) {
        if (dstTable.empty()) dstTable = this->_keyValuesMain[i]->m_from;
        setStr  += escapeIdent(this->_keyValuesMain[i]->m_key);
        setStr  += "=" + escapeLiteral(_keyValuesMain[i]->getValue()) + ", ";
    }
    // this is to remove ending separators
    setStr.erase(setStr.length() - 2);

    dotPos = dstTable.find(".");
    // add the dataset selected and escape table
    if (dotPos == string::npos) {
        dstTable = escapeColumn(dstTable, _defaultSchema);
    }
    else {
        dstTable = escapeColumn(dstTable.substr(dotPos + 1, string::npos), _defaultSchema);
    }
    //construct main part of the query
    queryString = "UPDATE " + dstTable + "\n SET " + setStr;
    // construct WHERE clause
    for (int i = 0; i < _keyValuesWhere.size(); i++) {
        if (!whereStr.empty()) whereStr += " AND ";
        whereStr += this->escapeColumn(_keyValuesWhere[i]->m_key, _keyValuesWhere[i]->m_from);
        whereStr += _opers[i];
        whereStr += escapeLiteral(_keyValuesWhere[i]->getValue());
    }
    if (!whereStr.empty()) {
        queryString += "\n WHERE " + whereStr;
    }
    queryString += ";";
    return queryString;
}

string SLQueryBuilder::getCountQuery()
{
    string queryString;

    //    size_t fromPos = initString.find(" FROM ");
    //    if (fromPos != string::npos) {
    //        queryString = "SELECT COUNT(*) AS count" + initString.substr(fromPos);
    //    }
    //    else {
    //        queryString = DEF_NO_QUERY;
    //    }

    return queryString;
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

string SLQueryBuilder::getDatasetCreateQuery(
                                             const string& name,
                                             const string& location,
                                             const string& friendly_name,
                                             const string& description)
{
    return "";
}

string SLQueryBuilder::getDatasetResetQuery(const string& name)
{
    return "";
}

string SLQueryBuilder::getDatasetDeleteQuery(const string& name)
{
    return "";
}

string SLQueryBuilder::getMethodCreateQuery(
                                            const string& name,
                                            const MethodKeys keys_definition,
                                            const MethodParams params_definition,
                                            const string& description)
{
    return "";
}

string SLQueryBuilder::getMethodDeleteQuery(const string& name)
{
    return "";
}

string SLQueryBuilder::getLastInsertedIdQuery()
{
    return "";
}

bool SLQueryBuilder::keyFrom(const string& table, const string& column)
{
    if (column.empty()) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("", column, "", table);
        _keyValuesMain.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyString(const string& key, const string& value, const string& from)
{
    if (key.empty() || value.empty()) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("text", key, value, from);
        _keyValuesMain.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyStringA(const string& key, string* values, const int size, const string& from)
{
    if (key.empty() || !values || size <= 0) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("textA", key, values, size, from);
        _keyValuesMain.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyInt(const string& key, int value, const string& from)
{
    if (key.empty()) return false;
    else {
        TKeyValue<int> *tk = new TKeyValue<int>("integer", key, value, from);
        _keyValuesMain.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyIntA(const string& key, int* values, const int size, const string& from)
{
    if (key.empty() || !values || size <= 0) return false;
    else {
        TKeyValue<int> *tk = new TKeyValue<int>("integerA", key, values, size, from);
        _keyValuesMain.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyFloat(const string& key, float value, const string& from)
{
    if (key.empty()) return false;
    else {
        TKeyValue<float> *tk = new TKeyValue<float>("float", key, value, from);
        _keyValuesMain.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyFloatA(const string& key, float* values, const int size, const string& from)
{
    if (key.empty() || !values || size <= 0) return false;
    else {
        TKeyValue<float> *tk = new TKeyValue<float>("floatA", key, values, size, from);
        _keyValuesMain.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keySeqtype(const string& key, const string& value, const string& from)
{
    if (key.empty() || value.empty() || !this->checkSeqtype(value)) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("seqtype", key, value, from);
        _keyValuesMain.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyInouttype(const string& key, const string& value, const string& from)
{
    if (key.empty() || value.empty() || !this->checkInouttype(value)) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("inouttype", key, value, from);
        _keyValuesMain.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyPStatus(const string& key, ProcessState::STATUS_T value, const string& from)
{
    if (key.empty() || value == ProcessState::STATUS_NONE) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("pstatus", key, ProcessState::toStatusString(value), from);
        _keyValuesMain.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyTimestamp(const string& key, const time_t& value, const string& from)
{
    if (key.empty()) return false;
    else {
        TKeyValue<time_t> *tk = new TKeyValue<time_t>("timestamp", key, value, from);
        _keyValuesMain.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyCvMat(const string& key, const cv::Mat& value, const string& from)
{
    if (key.empty()) return false;
    else {
        TKeyValue<cv::Mat> *tk = new TKeyValue<cv::Mat>("cvmat", key, value, from);
        _keyValuesMain.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::keyIntervalEvent(const string& key, const IntervalEvent& value, const string& from)
{
    if (key.empty()) return false;
    else {
        TKeyValue<IntervalEvent> *tk = new TKeyValue<IntervalEvent>("vtevent", key, value, from);
        _keyValuesMain.push_back(tk);
        return true;
    }
}

bool SLQueryBuilder::whereString(const string& key, const string& value, const string& oper, const string& from)
{
    if (key.empty() || value.empty()) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("text", key, value, from);
        _keyValuesWhere.push_back(tk);
        if (value.compare("NULL") == 0 || value.compare("NOT NULL") == 0) _opers.push_back(" IS ");
        else _opers.push_back(oper);
        return true;
    }
}

bool SLQueryBuilder::whereInt(const string& key, const int value, const string& oper, const string& from)
{
    if (key.empty()) return false;
    else {
        TKeyValue<int> *tk = new TKeyValue<int>("integer", key, value, from);
        _keyValuesWhere.push_back(tk);
        _opers.push_back(oper);
        return true;
    }
}

bool SLQueryBuilder::whereFloat(const string& key, const float value, const string& oper, const string& from)
{
    if (key.empty()) return false;
    else {
        TKeyValue<float> *tk = new TKeyValue<float>("float", key, value, from);
        _keyValuesWhere.push_back(tk);
        _opers.push_back(oper);
        return true;
    }
}

bool SLQueryBuilder::whereSeqtype(const string& key, const string& value, const string& oper, const string& from)
{
    if (key.empty() || value.empty() || !this->checkSeqtype(value)) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("seqtype", key, value, from);
        _keyValuesWhere.push_back(tk);
        _opers.push_back(oper);
        return true;
    }
}

bool SLQueryBuilder::whereInouttype(const string& key, const string& value, const string& oper, const string& from)
{
    if (key.empty() || value.empty() || !this->checkInouttype(value)) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("inouttype", key, value, from);
        _keyValuesWhere.push_back(tk);
        _opers.push_back(oper);
        return true;
    }
}

bool SLQueryBuilder::wherePStatus(const string& key, ProcessState::STATUS_T value, const string& oper, const string& from)
{
    if (key.empty() || value == ProcessState::STATUS_NONE) return false;
    else {
        TKeyValue<string> *tk = new TKeyValue<string>("pstatus", key, ProcessState::toStatusString(value), from);
        _keyValuesWhere.push_back(tk);
        _opers.push_back(oper);
        return true;
    }
}

bool SLQueryBuilder::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from)
{
    if (key.empty()) return false;
    else {
        TKeyValue<time_t> *tk = new TKeyValue<time_t>("timestamp", key, value, from);
        _keyValuesWhere.push_back(tk);
        _opers.push_back(oper);
        return true;
    }
}

bool SLQueryBuilder::whereTimeRange(const string& key_start, const string& key_length, const time_t& value_start, const uint value_length, const string& oper, const string& from)
{
    return false;
}

bool SLQueryBuilder::whereRegion(const string& key, const IntervalEvent::box& value, const string& oper, const string& from)
{
    return false;
}

bool SLQueryBuilder::whereExpression(const string& expression, const string& value, const string& oper)
{
    return false;
}

string SLQueryBuilder::escapeColumn(const string& key, const string& table)
{
    if (table.empty()) return escapeIdent(key);
    else return escapeIdent(table) + "." + escapeIdent(key);
}

string SLQueryBuilder::escapeIdent(const string& ident)
{
    return "[" + ident + "]";
}

string SLQueryBuilder::escapeLiteral(const string& ident)
{
    string escaped  = ident;
    for (int i = 0; i < escaped.length(); ++i) {
        if (escaped.at(i) == '\'') {
            escaped.insert(i, "\'");
            i++;
        }
    }
    return "\'" + escaped + "\'";
}


}
