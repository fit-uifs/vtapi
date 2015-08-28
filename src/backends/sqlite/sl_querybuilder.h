#pragma once

#include <sqlite3.h>
#include <vtapi/common/tkeyvalue.h>
#include "sl_connection.h"
#include <vtapi/plugins/backend_querybuilder.h>

namespace vtapi {


class SLQueryBuilder : public QueryBuilder
{
public:

    explicit SLQueryBuilder(SLConnection &connection);
    ~SLQueryBuilder();

    void reset() override;

    void *createQueryParam() override;
    void destroyQueryParam(void *param) override;
    void *duplicateQueryParam(void *param) override;

    std::string getGenericQuery() override;
    std::string getSelectQuery(const std::string& groupby,
                               const std::string& orderby,
                               int limit,
                               int offset) override;
    std::string getInsertQuery() override;
    std::string getUpdateQuery() override;
    std::string getDeleteQuery() override;
    std::string getCountQuery() override;
    std::string getBeginQuery() override;
    std::string getCommitQuery() override;
    std::string getRollbackQuery() override;
    std::string getDatasetCreateQuery(const std::string& name,
                                      const std::string& location,
                                      const std::string& friendly_name,
                                      const std::string& description) override;
    std::string getDatasetResetQuery(const std::string& name) override;
    std::string getDatasetDeleteQuery(const std::string& name) override;
    std::string getMethodCreateQuery(const std::string& name,
                                     const MethodKeys keys_definition,
                                     const MethodParams params_definition,
        const std::string& description) override;
    std::string getMethodDeleteQuery(const std::string& name) override;
    std::string getTaskCreateQuery(const std::string& name,
                                   const std::string& dsname,
                                   const std::string& mtname,
                                   const std::string& params,
                                   const std::string& prereq_task,
                                   const std::string& outputs) override;
    std::string getTaskDeleteQuery(const std::string& dsname,
                                   const std::string& taskname) override;
    std::string getLastInsertedIdQuery() override;

    bool keyFrom(const std::string& table, const std::string& column) override;
    bool keyString(const std::string& key, const std::string& value, const std::string& from) override;
    bool keyStringA(const std::string& key, std::string* values, const int size, const std::string& from) override;
    bool keyBool(const std::string& key, bool value, const std::string& from) override;
    bool keyInt(const std::string& key, int value, const std::string& from) override;
    bool keyIntA(const std::string& key, int* values, const int size, const std::string& from) override;
    bool keyFloat(const std::string& key, float value, const std::string& from) override;
    bool keyFloatA(const std::string& key, float* values, const int size, const std::string& from) override;
    bool keyFloat8(const std::string& key, double value, const std::string& from) override;
    bool keyFloat8A(const std::string& key, double* values, const int size, const std::string& from) override;
    bool keySeqtype(const std::string& key, const std::string& value, const std::string& from) override;
    bool keyInouttype(const std::string& key, const std::string& value, const std::string& from) override;
    bool keyProcessStatus(const std::string& key, ProcessState::STATUS_T value, const std::string& from) override;
    bool keyTimestamp(const std::string& key, const time_t& value, const std::string& from) override;
    bool keyCvMat(const std::string& key, const cv::Mat& value, const std::string& from) override;
    bool keyIntervalEvent(const std::string& key, const IntervalEvent& value, const std::string& from) override;

    bool whereString(const std::string& key, const std::string& value, const std::string& oper, const std::string& from) override;
    bool whereBool(const std::string& key, bool value, const std::string& oper, const std::string& from) override;
    bool whereInt(const std::string& key, const int value, const std::string& oper, const std::string& from) override;
    bool whereFloat(const std::string& key, const float value, const std::string& oper, const std::string& from) override;
    bool whereSeqtype(const std::string& key, const std::string& value, const std::string& oper, const std::string& from) override;
    bool whereInouttype(const std::string& key, const std::string& value, const std::string& oper, const std::string& from) override;
    bool whereProcessStatus(const std::string& key, ProcessState::STATUS_T value, const std::string& oper, const std::string& from) override;
    bool whereTimestamp(const std::string& key, const time_t& value, const std::string& oper, const std::string& from) override;
    bool whereTimeRange(const std::string& key_start, const std::string& key_length, const time_t& value_start, const uint value_length, const std::string& oper, const std::string& from) override;
    bool whereRegion(const std::string& key, const IntervalEvent::box& value, const std::string& oper, const std::string& from) override;
    bool whereExpression(const std::string& expression, const std::string& value, const std::string& oper) override;
    bool whereStringList(const std::string& key, const std::list<std::string>& values, const std::string& oper, const std::string& from) override;
    bool whereIntList(const std::string& key, const std::list<int>& values, const std::string& oper, const std::string& from) override;

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
