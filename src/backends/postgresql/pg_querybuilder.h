#pragma once

#include <list>
#include <libpq-fe.h>
#include <libpqtypes.h>
#include <vtapi/common/tkey.h>
#include "pg_connection.h"
#include <vtapi/plugins/backend_querybuilder.h>

namespace vtapi {


class PGQueryBuilder : public QueryBuilder
{
public:
    PGQueryBuilder(PGConnection &connection, const std::string& init_string);
    ~PGQueryBuilder();

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
    std::string getLastInsertedIdQuery();

    bool keyFrom(const std::string& table, const std::string& column);
    bool keyString(const std::string& key, const std::string& value, const std::string& from);
    bool keyStringA(const std::string& key, std::string* values, const int size, const std::string& from);
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
    bool whereInt(const std::string& key, const int value, const std::string& oper, const std::string& from);
    bool whereFloat(const std::string& key, const float value, const std::string& oper, const std::string& from);
    bool whereSeqtype(const std::string& key, const std::string& value, const std::string& oper, const std::string& from);
    bool whereInouttype(const std::string& key, const std::string& value, const std::string& oper, const std::string& from);
    bool wherePStatus(const std::string& key, ProcessState::STATUS_T value, const std::string& oper, const std::string& from);
    bool whereTimestamp(const std::string& key, const time_t& value, const std::string& oper, const std::string& from);
    bool whereTimeRange(const std::string& key_start, const std::string& key_length, const time_t& value_start, const uint value_length, const std::string& oper, const std::string& from);
    bool whereRegion(const std::string& key, const IntervalEvent::box& value, const std::string& oper, const std::string& from);
    bool whereExpression(const std::string& expression, const std::string& value, const std::string& oper);

private:
    typedef struct _MAIN_ITEM
    {
        TKey            key;            /**< table column */
        unsigned int    idParam;        /**< bind value ID (PGparam used) */

        _MAIN_ITEM(const std::string& key,
                   const std::string& table,
                   const unsigned int idParam)
        : key("", key, 1, table), idParam(idParam) { }
    } MAIN_ITEM;
    typedef std::list<MAIN_ITEM>    MAIN_LIST;
    typedef MAIN_LIST::iterator     MAIN_LIST_IT;

    typedef struct _WHERE_ITEM
    {
        TKey            key;            /**< key(is expression if idParam = 0) + table */
        std::string     oper;           /**< operator */
        std::string     value;          /**< explicit value (PGparam not used) */
        unsigned int    idParam;        /**< bind value ID (PGparam used) */

        _WHERE_ITEM(const std::string& key,
                    const std::string& table,
                    const std::string& oper,
                    const unsigned int idParam)
            : key("", key, 1, table), oper(oper), idParam(idParam)
        { }

        _WHERE_ITEM(const std::string& exp,
                    const std::string& oper,
                    const std::string& value)
            : key("", exp, 1, ""), oper(oper), value(value), idParam(0)
        { }
    } WHERE_ITEM;
    typedef std::list<WHERE_ITEM>   WHERE_LIST;
    typedef WHERE_LIST::iterator    WHERE_LIST_IT;

    WHERE_LIST      _listWhere;        /**< list of WHERE clause items */
    MAIN_LIST       _listMain;         /**< list of items for main query part */
    uint            _cntParam;         /**< keys counter */


    std::string constructTable(const std::string& table = "", const std::string& schema = "");
    std::string constructColumn(const std::string& column, const std::string& table = "");
    std::string constructColumnNoTable(const std::string& column);
    std::string constructAlias(const std::string& column);
    std::string escapeIdent(const std::string& ident);
    std::string escapeLiteral(const std::string& literal);

    PGtimestamp UnixTimeToTimestamp(const time_t& utime);
    std::string UnixTimeToTimestampString(const time_t& utime);

    template<typename T>
    uint addToParam(const char* type, T value);

    template<typename T>
    bool keySingleValue(const std::string& key, T value, const char *type, const std::string& from);
    template<typename T>
    bool keyArray(const std::string& key, T* values, const int size, const char *type, const char* type_arr, const std::string& from);
    template<typename T>
    bool whereSingleValue(const std::string& key, T value, const char *type, const std::string& oper, const std::string& from);
};

}
