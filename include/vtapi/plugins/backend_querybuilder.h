#pragma once

#include <string>
#include <list>
#include <opencv2/opencv.hpp>
#include "../common/tkeyvalue.h"
#include "../data/processstate.h"
#include "../data/intervalevent.h"
#include "../data/methodkeys.h"
#include "../data/methodparams.h"
#include "backend_connection.h"

namespace vtapi {


/**
 * @brief Class provides functionality to build various SQL queries
 *
 * Query parameters must first be inserted through keyX and whereX methods.
 * KeyX methods should be used to add key(column)/value pairs (or just keys) for
 * main section of SELECT, UPDATE and INSERT queries. WhereX methods construct
 * WHERE clause for SELECT and UPDATE queries.
 *
 * It's possible to set default dataset and table which will be used if those
 * are not specified in keyX or whereX call.
 *
 * Query string can be obtained via getSelectQuery, getUpdateQuery or
 * getInsertQuery methodhs.
 */
class QueryBuilder {
public:
    /**
     * Constructor
     * @param connection connection object
     * @param initString initialization string (query/table or empty)
     */
    QueryBuilder(Connection& connection, const std::string& init_string)
    : _connection(connection), _init_string(init_string), _pquery_param(NULL)
    {}

    /**
     * Virtual destructor - destroy implementation first
     */
    virtual ~QueryBuilder() { }

    /**
     * Resets query builder to initial state
     */
    virtual void reset() = 0;

    /**
     * Create new query param structure, remember to destroQueryParam() it
     * @return new query param object
     */
    virtual void *createQueryParam() = 0;

    /**
     * Destroys query param structure
     */
    virtual void destroyQueryParam(void *param) = 0;

    /**
     * Duplicates existing query param structure, destroys the old one
     * @return new query param object
     */
    virtual void *duplicateQueryParam(void *param) = 0;

    /**
     * Gets object for parametrized queries
     * @return param object for parametrized queries
     */
    void *getQueryParam()
    { return _pquery_param; }

    /**
     * Specify custom SQL string for query
     * @param sql custom SQL string
     */
    void useQueryString(const std::string& sql)
    { _init_string = sql; }

    /**
     * This is to specify default schema/dataset to be inserted into
     * @param schema DB schema into which new data will be inserted
     */
    void useDefaultSchema(const std::string& schema)
    { _defaultSchema = schema; }

   /**
     * This is to specify the (single) table to be inserted in
     * @param table table into which new data will be inserted
     */
    void useDefaultTable(const std::string& table)
    { _defaultTable = table; }

    /**
     * Gets query from initialization string
     * @return query string
     */
    virtual std::string getGenericQuery() = 0;

    /**
     * Builds SELECT query string
     * @param groupby GROUP BY argument
     * @param orderby ORDER BY argument
     * @param limit LIMIT argument
     * @param offset OFFSET argument
     * @return SELECT query string
     */
    virtual std::string getSelectQuery(
        const std::string& groupby,
        const std::string& orderby,
        const int limit,
        const int offset) = 0;

    /**
     * Builds INSERT query string
     * @return INSERT query string
     */
    virtual std::string getInsertQuery() = 0;

    /**
     * Builds UPDATE query
     * @return UPDATE query string
     */
    virtual std::string getUpdateQuery() = 0;

    /**
     * Builds SELECT COUNT(*) query
     * @return SELECT COUNT query string
     */
    virtual std::string getCountQuery() = 0;

    /**
     * Builds BEGIN TRANSACTION query
     * @return begin query string
     */
    virtual std::string getBeginQuery() = 0;

    /**
     * Builds COMMIT TRANSACTION query
     * @return commit query string
     */
    virtual std::string getCommitQuery() = 0;

    /**
     * Builds ROLLBACK TRANSACTION query
     * @return rollback string
     */
    virtual std::string getRollbackQuery() = 0;

    virtual std::string getDatasetCreateQuery(const std::string& name,
                                              const std::string& location,
                                              const std::string& friendly_name,
                                              const std::string& description) = 0;

    virtual std::string getDatasetResetQuery(const std::string& name) = 0;

    virtual std::string getDatasetDeleteQuery(const std::string& name) = 0;

    virtual std::string getMethodCreateQuery(const std::string& name,
                                             const MethodKeys keys_definition,
                                             const MethodParams params_definition,
                                             const std::string& description) = 0;

    virtual std::string getMethodDeleteQuery(const std::string& name) = 0;

    virtual std::string getSequenceDeleteQuery(const std::string& name) = 0;

    virtual std::string getTaskCreateQuery(const std::string& name,
                                           const std::string& mtname,
                                           const std::string& params,
                                           const std::string& prereq_task,
                                           const std::string& outputs) = 0;

    virtual std::string getTaskDeleteQuery(const std::string& name) = 0;

    virtual std::string getLastInsertedIdQuery() = 0;

    /**
     * This is used to specify the table for FROM statement and the column list for SELECT statement
     * It may be called more times.
     * @param table table to select
     * @param column column for select
     * @return success
     */
    virtual bool keyFrom(const std::string& table, const std::string& column) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyString(const std::string& key, const std::string& value, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyStringA(const std::string& key, std::string* values, const int size, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyBool(const std::string& key, bool value, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyInt(const std::string& key, int value, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyIntA(const std::string& key, int* values, const int size, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyFloat(const std::string& key, float value, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyFloatA(const std::string& key, float* values, const int size, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyFloat8(const std::string& key, double value, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyFloat8A(const std::string& key, double* values, const int size, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keySeqtype(const std::string& key, const std::string& value, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyInouttype(const std::string& key, const std::string& value, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyPStatus(const std::string& key, ProcessState::STATUS_T value, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyTimestamp(const std::string& key, const time_t& value, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyCvMat(const std::string& key, const cv::Mat& value, const std::string& from) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyIntervalEvent(const std::string& key, const IntervalEvent& value, const std::string& from) = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * This is a WHERE statement construction class
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereString(const std::string& key, const std::string& value, const std::string& oper, const std::string& from) = 0;

    /**
     * This is a WHERE statement construction class for bools
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereBool(const std::string& key, bool value, const std::string& oper, const std::string& from) = 0;

    /**
     * This is a WHERE statement construction class for integers
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereInt(const std::string& key, const int value, const std::string& oper, const std::string& from) = 0;

    /**
     * This is a WHERE statement construction class for floats
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereFloat(const std::string& key, const float value, const std::string& oper, const std::string& from) = 0;
    /**
     * This is a WHERE statement construction class for seqtype
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereSeqtype(const std::string& key, const std::string& value, const std::string& oper, const std::string& from) = 0;
    /**
     * This is a WHERE statement construction class for inouttype
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereInouttype(const std::string& key, const std::string& value, const std::string& oper, const std::string& from) = 0;
    /**
     * This is a WHERE statement construction class for pstatus type
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool wherePStatus(const std::string& key, ProcessState::STATUS_T value, const std::string& oper, const std::string& from) = 0;
    /**
     * This is a WHERE statement construction class for timestamp
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereTimestamp(const std::string& key, const time_t& value, const std::string& oper, const std::string& from) = 0;
     /**
      * This is a WHERE statement construction function for filters by overlapping time ranges
      * @param key_start    key with range start time [timestamp]
      * @param key_length   key with range length [s]
      * @param value_start  compared range start time [UNIX time]
      * @param value_length compared range length [s]
      * @param oper comparison operator between ranges
      * @param from table where keys are situated
      * @return success
      */
     virtual bool whereTimeRange(const std::string& key_start, const std::string& key_length, const time_t& value_start, const uint value_length, const std::string& oper, const std::string& from) = 0;
    /**
     * This is a WHERE statement construction function for filters by box
     * @param key  key to compare with a value
     * @param value requested value for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereRegion(const std::string& key, const IntervalEvent::box& value, const std::string& oper, const std::string& from) = 0;

    /**
     * This is a WHERE statement construction function for custom expression
     * @param expression
     * @param value requested value for expression
     * @param oper comparison operator between expression and value
     * @return  success
     */
    virtual bool whereExpression(const std::string& expression, const std::string& value, const std::string& oper) = 0;

    /**
     * This is a WHERE statement construction function for filters by string list
     * @param key  key to compare with a value
     * @param values requested values for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereStringList(const std::string& key, const std::list<std::string>& values, const std::string& oper, const std::string& from) = 0;

    /**
     * This is a WHERE statement construction function for filters by integer list
     * @param key  key to compare with a value
     * @param values requested values for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereIntList(const std::string& key, const std::list<int>& values, const std::string& oper, const std::string& from) = 0;

protected:
    Connection  &_connection;    /**< connection object */
    void        *_pquery_param;    /**< object for parametrized queries */
    std::string _init_string;     /**< init query string (whole query or table) */
    std::string _defaultSchema;  /**< default db schema for queries */
    std::string _defaultTable;   /**< default table for queries */

    /**
     * Checks validity of seqtype value
     * @param value seqtype value
     * @return success
     */
    bool checkSeqtype(const std::string& value)
    {
        return (
            (value.compare("images") == 0)  ||
            (value.compare("video") == 0)   ||
            (value.compare("data") == 0));
    }
    /**
     * Checks validity of inouttype value
     * @param value inouttype value
     * @return success
     */
    bool checkInouttype(const std::string& value)
    {
        return (
            (value.compare("in_param") == 0)    ||
            (value.compare("in_process") == 0)  ||
            (value.compare("out_table") == 0));
    }
};


} // namespace vtapi
