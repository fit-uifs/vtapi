#pragma once

#include <string>
#include <list>
#include "vtapi_backendbase.h"
#include "../common/vtapi_tkeyvalue.h"
#include "../data/vtapi_processstate.h"
#include "../data/vtapi_intervalevent.h"

namespace vtapi {

class QueryBuilder;
class PGQueryBuilder;
class SLQueryBuilder;

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
    QueryBuilder(void *connection, const std::string& initString)
    : connection(connection), initString(initString), queryParam(NULL) {}
    
    /**
     * Virtual destructor - destroy implementation first
     */
    virtual ~QueryBuilder() { };

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
    { return this->queryParam; }
    
    /**
     * Specify custom SQL string for query
     * @param sql custom SQL string
     */
    void useQueryString(const std::string& sql)
    { this->initString = sql; }
    
    /**
     * This is to specify default schema/dataset to be inserted into
     * @param schema DB schema into which new data will be inserted
     */
    void useDefaultSchema(const std::string& schema)
    { this->defaultSchema = schema; }
    
   /**
     * This is to specify the (single) table to be inserted in
     * @param table table into which new data will be inserted
     */
    void useDefaultTable(const std::string& table)
    { this->defaultTable = table; }
    
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

#ifdef VTAPI_HAVE_OPENCV
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyCvMat(const std::string& key, const cv::Mat& value, const std::string& from) = 0;
#endif
    
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
    
protected:
    void                *connection;    /**< connection object */
    void                *queryParam;    /**< object for parametrized queries */
    std::string         initString;     /**< init query string (whole query or table) */
    std::string         defaultSchema;  /**< default db schema for queries */
    std::string         defaultTable;   /**< default table for queries */
    
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

#if VTAPI_HAVE_POSTGRESQL
class PGQueryBuilder : public QueryBuilder, public PGBackendBase
{
public:

    PGQueryBuilder(const PGBackendBase &base, void *connection, const std::string& initString = "");
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
#if VTAPI_HAVE_OPENCV
    bool keyCvMat(const std::string& key, const cv::Mat& value, const std::string& from);
#endif
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

        _MAIN_ITEM(const std::string& key, const std::string& table, const unsigned int idParam)
        : key("", key, 1, table), idParam(idParam) { };
    } MAIN_ITEM;
    typedef std::list<MAIN_ITEM>    MAIN_LIST;
    typedef MAIN_LIST::iterator     MAIN_LIST_IT;

    typedef struct _WHERE_ITEM
    {
        TKey            key;            /**< key(is expression if idParam = 0) + table */
        std::string     oper;           /**< operator */
        std::string     value;          /**< explicit value (PGparam not used) */
        unsigned int    idParam;        /**< bind value ID (PGparam used) */

        _WHERE_ITEM(const std::string& key, const std::string& table, const std::string& oper, const unsigned int idParam)
        : key("", key, 1, table), oper(oper), idParam(idParam) { };

        _WHERE_ITEM(const std::string& exp, const std::string& oper, const std::string& value)
        : key("", exp, 1, ""), oper(oper), value(value), idParam(0) { };
    } WHERE_ITEM;
    typedef std::list<WHERE_ITEM>   WHERE_LIST;
    typedef WHERE_LIST::iterator    WHERE_LIST_IT;


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
    

    WHERE_LIST      m_listWhere;        /**< list of WHERE clause items */
    MAIN_LIST       m_listMain;
    uint            m_cntParam;         /**< keys counter */
    
};
#endif

#if VTAPI_HAVE_SQLITE
class SLQueryBuilder : public QueryBuilder, public SLBackendBase
{
private:

    TKeyValues      key_values_main;    /**< table key/values storage for SELECT FROM, INSERT INTO and UPDATE SET clauses */
    TKeyValues      key_values_where;   /**< table key/values storage for SELECT FROM, INSERT INTO and UPDATE SET clauses */
    std::vector     <std::string> opers; /**< operators */

public:

    SLQueryBuilder(const SLBackendBase &base, void *connection, const std::string& initString = "");
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
#if VTAPI_HAVE_OPENCV
    bool keyCvMat(const std::string& key, const cv::Mat& value, const std::string& from);
#endif
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

protected:

    std::string escapeColumn(const std::string& key, const std::string& table);
    std::string escapeIdent(const std::string& ident);
    std::string escapeLiteral(const std::string& literal);
    
private:

    /**
     * Frees all key s vectors
     */
    void destroyKeys();

};
#endif

} // namespace vtapi
