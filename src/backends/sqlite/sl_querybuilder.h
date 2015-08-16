#pragma once

#include <sqlite3.h>
#include <vtapi/common/tkeyvalue.h>
#include "sl_connection.h"
#include <vtapi/plugins/backend_querybuilder.h>

namespace vtapi {


class SLQueryBuilder : public QueryBuilder
{
public:

    SLQueryBuilder(SLConnection &connection, const std::string& init_string);
    ~SLQueryBuilder();

    void reset();

    void *createQueryParam();
    void destroyQueryParam(void *param);
    void *duplicateQueryParam(void *param);

    std::string getGenericQuery();
    std::string getSelectQuery(const std::string& groupby, const std::string& orderby, const int limit, const int offset);
    std::string getInsertQuery();
    std::string getUpdateQuery();
    std::string getCountQuery();
    std::string getBeginQuery();
    std::string getCommitQuery();
    std::string getRollbackQuery();
    std::string getDatasetCreateQuery(
        const std::string& name,
        const std::string& location,
        const std::string& friendly_name,
        const std::string& description);
    std::string getDatasetResetQuery(const std::string& name);
    std::string getDatasetDeleteQuery(const std::string& name);
    std::string getMethodCreateQuery(
        const std::string& name,
        const MethodKeys keys_definition,
        const MethodParams params_definition,
        const std::string& description);
    std::string getMethodDeleteQuery(const std::string& name);
    std::string getSequenceDeleteQuery(const std::string& name);
    std::string getLastInsertedIdQuery();

    bool keyFrom(const std::string& table, const std::string& column);
    bool keyString(const std::string& key, const std::string& value, const std::string& from);
    bool keyStringA(const std::string& key, std::string* values, const int size, const std::string& from);
    bool keyBool(const std::string& key, bool value, const std::string& from);
    bool keyInt(const std::string& key, int value, const std::string& from);
    bool keyIntA(const std::string& key, int* values, const int size, const std::string& from);
    bool keyFloat(const std::string& key, float value, const std::string& from);
    bool keyFloatA(const std::string& key, float* values, const int size, const std::string& from);
    bool keySeqtype(const std::string& key, const std::string& value, const std::string& from);
    bool keyInouttype(const std::string& key, const std::string& value, const std::string& from);
    bool keyPStatus(const std::string& key, ProcessState::STATUS_T value, const std::string& from);
    bool keyTimestamp(const std::string& key, const time_t& value, const std::string& from);
    bool keyCvMat(const std::string& key, const cv::Mat& value, const std::string& from);
    bool keyIntervalEvent(const std::string& key, const IntervalEvent& value, const std::string& from);

    bool whereString(const std::string& key, const std::string& value, const std::string& oper, const std::string& from);
    bool whereBool(const std::string& key, bool value, const std::string& oper, const std::string& from);
    bool whereInt(const std::string& key, const int value, const std::string& oper, const std::string& from);
    bool whereFloat(const std::string& key, const float value, const std::string& oper, const std::string& from);
    bool whereSeqtype(const std::string& key, const std::string& value, const std::string& oper, const std::string& from);
    bool whereInouttype(const std::string& key, const std::string& value, const std::string& oper, const std::string& from);
    bool wherePStatus(const std::string& key, ProcessState::STATUS_T value, const std::string& oper, const std::string& from);
    bool whereTimestamp(const std::string& key, const time_t& value, const std::string& oper, const std::string& from);
    bool whereTimeRange(const std::string& key_start, const std::string& key_length, const time_t& value_start, const uint value_length, const std::string& oper, const std::string& from);
    bool whereRegion(const std::string& key, const IntervalEvent::box& value, const std::string& oper, const std::string& from);
    bool whereExpression(const std::string& expression, const std::string& value, const std::string& oper);
    bool whereStringList(const std::string& key, const std::list<std::string>& values, const std::string& oper, const std::string& from);
    bool whereIntList(const std::string& key, const std::list<int>& values, const std::string& oper, const std::string& from);

private:
    TKeyValues      _keyValuesMain;     /**< table key/values storage for SELECT FROM, INSERT INTO and UPDATE SET clauses */
    TKeyValues      _keyValuesWhere;    /**< table key/values storage for SELECT FROM, INSERT INTO and UPDATE SET clauses */
    std::vector<std::string> _opers;    /**< operators */

    std::string escapeColumn(const std::string& key, const std::string& table);
    std::string escapeIdent(const std::string& ident);
    std::string escapeLiteral(const std::string& literal);

    void destroyKeys();

};

}
