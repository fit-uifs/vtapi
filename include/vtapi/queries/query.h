/**
 * @file
 * @brief   Declaration of Query class which is general class for SQL queries/commands.
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include <string>
#include "../data/commons.h"
#include "../plugins/backend_interface.h"
#include "../plugins/backend_connection.h"
#include "../plugins/backend_querybuilder.h"
#include "../plugins/backend_resultset.h"

namespace vtapi {
    
/**
 * @brief Base query class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Query
{
public:

    /**
     * Constructs a query object
     * @param commons pointer of the existing Commons object
     * @param sql explicitly specified query string (or default table name)
     * @param sql_is_table if true, sql parameter is default table name
     */
    Query(const Commons& commons, const std::string& sql, bool sql_is_table = false);

    /**
     * Destructor
     */
    virtual ~Query();
    
    /**
     * This expands the query, so you can check it before the execution
     * @return string value with the query
     */
    virtual std::string getQuery();
    
    /**
     * This will commit your query
     * @return success
     */
    virtual bool execute();
    
    /**
     * @brief Has query been executed
     * @return bool
     */
    bool isExecuted();

    /**
     * Clears the query object to its original state
     */
    void reset();

    /**
     * @brief Query builder accessor
     * @return query builder
     */
    QueryBuilder& querybuilder();

    /**
     * @brief Result set accessor
     * @return result set
     */
    ResultSet& resultset();

protected:
    bool _executed;                         /**< false if query has been changes */

    /**
     * @brief backend interface accessor
     * @return backend interface
     */
    const IBackendInterface & backend();

    /**
     * @brief connection accessor
     * @return connection
     */
    Connection & connection();

private:
    const IBackendInterface &_backend;      /**< shared backend interface */
    Connection &_connection;                /**< shared connection to backend */
    QueryBuilder *_pquerybuilder;           /**< interface for building queries */
    ResultSet *_presultset;                 /**< result set interface */

    Query() = delete;
    Query(const Query&) = delete;
    Query& operator=(const Query&) = delete;
};

/**
 * @brief Base class for queries with WHERE clause
 */
class QueryWhere : public Query
{
public:
    /**
     * Constructs a query object
     * @param commons pointer of the existing Commons object
     * @param table table name
     */
    QueryWhere(const Commons& commons, const std::string& table);

    /**
     * Destructor
     */
    virtual ~QueryWhere();

    /**
     * This is a WHERE statement construction function for strings
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparision operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
    bool whereString(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = std::string());

    /**
     * This is a WHERE statement construction function for bools
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparision operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
    bool whereBool(const std::string& key, bool value, const std::string& oper = "=", const std::string& from = std::string());

    /**
     * This is a WHERE statement construction function for integers
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparision operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
    bool whereInt(const std::string& key, const int value, const std::string& oper = "=", const std::string& from = std::string());

    /**
     * This is a WHERE statement construction function for floats
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparison operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
    bool whereFloat(const std::string& key, const float value, const std::string& oper = "=", const std::string& from = std::string());

    /**
     * This is a WHERE statement construction function for seqtype
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparison operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
     bool whereSeqtype(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = std::string());

    /**
     * This is a WHERE statement construction function for inouttype
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparison operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
     bool whereInouttype(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = std::string());

    /**
     * This is a WHERE statement construction function for pstate type
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparison operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
     bool whereProcessStatus(const std::string& key, ProcessState::STATUS_T value, const std::string& oper = "=", const std::string& from = std::string());

    /**
     * This is a WHERE statement construction function for timestamp
     * @param key     key to compare with a value
     * @param value   requested value for key
     * @param oper    comparison operator between key and value
     * @param from    table where the key is situated
     * @return success
     * @note It may be called several times.
     */
     bool whereTimestamp(const std::string& key, const time_t& value, const std::string& oper = "=", const std::string& from = std::string());

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
     bool whereTimeRange(const std::string& key_start, const std::string& key_length, const time_t& value_start, const uint value_length, const std::string& oper = "&&", const std::string& from = std::string());

     /**
     * This is a WHERE statement construction function for filters by geometric box region
     * @param key  key to compare with a value
     * @param value requested value for key
     * @param oper comparison operator between key and value
     * @param from    table where the key is situated
     * @return success
     */
     bool whereRegion(const std::string& key, const IntervalEvent::box& value, const std::string& oper = "&&", const std::string& from = std::string());

    /**
     * This is a WHERE statement construction function for custom expression
     * @param expression expression
     * @param value requested value for expression
     * @param oper comparison operator between expression and value
     * @return  success
     */
     bool whereExpression(const std::string& expression, const std::string& value, const std::string& oper = "=");

     /**
      * @brief This is a WHERE statement construction function for filter by string value IN list
      * @param key key whose value should be in list of values
      * @param values list of values
      * @param from table where the key is situated
      * @return success
      */
     bool whereStringInList(const std::string& key, const std::list<std::string>& values, const std::string& from = std::string());

     /**
      * @brief This is a WHERE statement construction function for filter by string value IN list
      * @param key key whose value should be in list of values
      * @param values list of values
      * @param from table where the key is situated
      * @return success
      */
     bool whereIntInList(const std::string& key, const std::list<int>& values, const std::string& from = std::string());
};

} // namespace vtapi
