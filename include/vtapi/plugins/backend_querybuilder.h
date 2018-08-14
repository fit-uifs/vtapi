#pragma once

#include "../data/processstate.h"
#include "../data/intervalevent.h"
#include "../data/eyedea_edfdescriptor.h"
#include "../data/taskkeys.h"
#include "../data/taskparams.h"
#include "../data/eventfilter.h"
#include "backend_connection.h"
#include <string>
#include <vector>
#include <chrono>
#include <opencv2/opencv.hpp>

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
 * getInsertQuery methods.
 */
class QueryBuilder {
public:
    /**
     * @brief Resets query builder to initial state
     */
    virtual void reset() = 0;

    // ////////////////////////////////////////////////////////////////////////
    // query parameter methods
    // ////////////////////////////////////////////////////////////////////////

    /**
     * @brief Creates new query param
     * @note should be destroyed with destroyQueryParam()
     * @return new query param object
     */
    virtual void *createQueryParam() const = 0;

    /**
     * @brief Destroys query param structure
     */
    virtual void destroyQueryParam(void *param) const = 0;

    /**
     * @brief Duplicates existing query param structure
     * @return duplicated query param object
     */
    virtual void *duplicateQueryParam(void *param) const = 0;

    // ////////////////////////////////////////////////////////////////////////
    // query builder methods
    // ////////////////////////////////////////////////////////////////////////

    /**
     * @brief Gets query from initialization string
     * @return query string, empty on error
     */
    virtual std::string getGenericQuery() const = 0;

    /**
     * @brief Builds SELECT query string
     * @param groupby GROUP BY argument
     * @param orderby ORDER BY argument
     * @param limit LIMIT argument, 0 = no limit
     * @param offset OFFSET argument, 0 = no offset
     * @return query string, empty on error
     */
    virtual std::string getSelectQuery(
        const std::string& groupby,
        const std::string& orderby,
        int limit,
        int offset) const = 0;

    /**
     * @brief Builds INSERT query string
     * @return query string, empty on error
     */
    virtual std::string getInsertQuery() const = 0;

    /**
     * Builds UPDATE query
     * @return query string, empty on error
     */
    virtual std::string getUpdateQuery() const = 0;

    /**
     * @brief Builds DELETE query
     * @return query string, empty on error
     */
    virtual std::string getDeleteQuery() const = 0;

    /**
     * Builds SELECT COUNT(*) query
     * @return query string, empty on error
     */
    virtual std::string getCountQuery() const = 0;

    /**
     * Builds BEGIN TRANSACTION query
     * @return query string, empty on error
     */
    virtual std::string getBeginQuery() const = 0;

    /**
     * Builds COMMIT TRANSACTION query
     * @return query string, empty on error
     */
    virtual std::string getCommitQuery() const = 0;

    /**
     * Builds ROLLBACK TRANSACTION query
     * @return query string, empty on error
     */
    virtual std::string getRollbackQuery() const = 0;

    /**
     * @brief Builds query to create new dataset
     * @param name dataset name
     * @param location dataset location
     * @param friendly_name dataset user friendly name
     * @param description dataset description
     * @return query string, empty on error
     */
    virtual std::string getDatasetCreateQuery(const std::string& name,
                                              const std::string& location,
                                              const std::string& friendly_name,
                                              const std::string& description) const = 0;

    /**
     * @brief Builds query to clear dataset data
     * @param name dataset name
     * @return query string, empty on error
     */
    virtual std::string getDatasetResetQuery(const std::string& name) const = 0;

    /**
     * @brief Builds query to completely delete dataset
     * @param name dataset name
     * @return query string, empty on error
     */
    virtual std::string getDatasetDeleteQuery(const std::string& name) const = 0;

    /**
     * @brief Builds query to create new method
     * @param name new method name
     * @param keys_definition definition of used columns
     * @param params_definition definition of used parameteres
     * @param description method description
     * @return query string, empty on error
     */
    virtual std::string getMethodCreateQuery(const std::string &name,
                                             const TaskKeyDefinitions &keys_definition,
                                             const TaskParamDefinitions &params_definition,
                                             const std::string &description) const = 0;

    /**
     * @brief Builds query to delete method
     * @param name method name
     * @return query string, empty on error
     */
    virtual std::string getMethodDeleteQuery(const std::string &name) const = 0;

    /**
     * @brief Builds query to create new task
     * @param name task name
     * @param dsname parent dataset name
     * @param mtname parent method name
     * @param params task parameters
     * @param prereq_task prerequisite task, empty on no prerequisite
     * @param outputs table for output data
     * @return query string, empty on error
     */
    virtual std::string getTaskCreateQuery(const std::string& name,
                                           const std::string& dsname,
                                           const std::string& mtname,
                                           const std::string& params,
                                           const std::string& prereq_task,
                                           const std::string& outputs) const = 0;

    /**
     * @brief Builds query to delete task
     * @param dsname parent dataset name
     * @param taskname task name
     * @return query string, empty on error
     */
    virtual std::string getTaskDeleteQuery(const std::string& dsname,
                                           const std::string& taskname) const = 0;

    /**
     * @brief Builds query to get last inserted ID
     * @return query string, empty on error
     */
    virtual std::string getLastInsertedIdQuery() const = 0;


    // ////////////////////////////////////////////////////////////////////////
    // setting query table (for SELECT or DELETE)
    // ////////////////////////////////////////////////////////////////////////

    /**
     * @brief Specifies FROM column/table for SELECT or DELETE query
     * @note It may be called more times.
     * @param table table to select
     * @param column column for select
     * @return success
     */
    virtual bool keyFrom(const std::string& table,
                         const std::string& column) = 0;

    // ////////////////////////////////////////////////////////////////////////
    // setting query keys (for INSERT or UPDATE)
    // ////////////////////////////////////////////////////////////////////////

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyBool(const std::string& key,
                         bool value,
                         const std::string& from = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyChar(const std::string& key,
                         char value,
                         const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyString(const std::string& key,
                           const std::string& value,
                           const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyStringVector(const std::string& key,
                                 const std::vector<std::string> &values,
                                 const std::string& = std::string()) = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyInt(const std::string& key,
                        int value,
                        const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyIntVector(const std::string& key,
                              const std::vector<int> &values,
                              const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyInt8(const std::string& key,
                        long long value,
                        const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyInt8Vector(const std::string& key,
                              const std::vector<long long> &values,
                              const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyFloat(const std::string& key,
                          float value,
                          const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyFloatVector(const std::string& key,
                                const std::vector<float> &values,
                                const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyFloat8(const std::string& key,
                           double value,
                           const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyFloat8Vector(const std::string& key,
                                 const std::vector<double> &values,
                                 const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyTimestamp(const std::string& key,
                              const std::chrono::system_clock::time_point & value,
                              const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyCvMat(const std::string& key,
                          const cv::Mat &value,
                          const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyPoint(const std::string& key,
                          Point value,
                          const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyPointVector(const std::string& key,
                                const std::vector<Point> &values,
                                const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyIntervalEvent(const std::string& key,
                                  const IntervalEvent& value,
                                  const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyEdfDescriptor(const std::string &key,
                                  const EyedeaEdfDescriptor &value,
                                  const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyProcessStatus(const std::string& key,
                                  ProcessState::Status value,
                                  const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param data binary data
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyBlob(const std::string& key,
                         const std::vector<char> &data,
                         const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keySeqtype(const std::string& key,
                            const std::string& value,
                            const std::string& = std::string()) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyInouttype(const std::string& key,
                              const std::string& value,
                              const std::string& = std::string()) = 0;

    // ////////////////////////////////////////////////////////////////////////
    // setting query WHERE clause parameters (for SELECT, UPDATE or DELETE)
    // ////////////////////////////////////////////////////////////////////////

    /**
     * This is a WHERE statement construction class for bools
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereBool(const std::string& key,
                            bool value,
                            const std::string& oper = std::string("="),
                            const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction class
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereChar(const std::string& key,
                            char value,
                            const std::string& oper = std::string("="),
                            const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction class
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereString(const std::string& key,
                              const std::string& value,
                              const std::string& oper = std::string("="),
                              const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction function for filters by string vector
     * @param key  key to compare with a value
     * @param values requested values for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereStringVector(const std::string& key,
                                     const std::vector<std::string>& values,
                                     const std::string& oper = std::string("IN"),
                                     const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction class for integers
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereInt(const std::string& key,
                           int value,
                           const std::string& oper = std::string("="),
                           const std::string &from = std::string()) = 0;

     /**
      * This is a WHERE statement construction function for filters by integer vector
      * @param key  key to compare with a value
      * @param values requested values for key
      * @param oper comparison operator between key and value
      * @param from table where the key is situated
      * @return success
     */
     virtual bool whereIntVector(const std::string& key,
                                  const std::vector<int>& values,
                                  const std::string& oper = std::string("IN"),
                                  const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction class for long integers
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereInt8(const std::string& key,
                             long long value,
                             const std::string& oper = std::string("="),
                             const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction function for filters by long integer vector
     * @param key  key to compare with a value
     * @param values requested values for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereInt8Vector(const std::string& key,
                               const std::vector<long long>& values,
                               const std::string& oper = std::string("IN"),
                               const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction class for floats
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereFloat(const std::string& key,
                              float value,
                              const std::string& oper = std::string("="),
                              const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction function for filters by float vector
     * @param key  key to compare with a value
     * @param values requested values for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereFloatVector(const std::string& key,
                                    const std::vector<float>& values,
                                    const std::string& oper = std::string("IN"),
                                    const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction class for doubles
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereFloat8(const std::string& key,
                               double value,
                               const std::string& oper = std::string("="),
                               const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction function for filters by double vector
     * @param key  key to compare with a value
     * @param values requested values for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereFloat8Vector(const std::string& key,
                                     const std::vector<double>& values,
                                     const std::string& oper = std::string("IN"),
                                     const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction class for timestamp
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereTimestamp(const std::string& key,
                                 const std::chrono::system_clock::time_point& value,
                                 const std::string& oper = std::string("="),
                                 const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction class for points
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool wherePoint(const std::string& key,
                             Point value,
                             const std::string& oper = std::string("="),
                             const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction function for filters by point vector
     * @param key  key to compare with a value
     * @param values requested values for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool wherePointVector(const std::string& key,
                                    const std::vector<Point>& values,
                                    const std::string& oper = std::string("IN"),
                                    const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction class for pstatus type
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereProcessStatus(const std::string& key,
                                     ProcessState::Status value,
                                     const std::string& oper = std::string("="),
                                     const std::string &from = std::string()) = 0;

     /**
      * This is a WHERE statement construction function for filters by overlapping time ranges
      * @param key_start key with range start time [timestamp]
      * @param key_length key with range length [s]
      * @param value_start compared range start time [UNIX time]
      * @param value_end compared range duration [s]
      * @param oper comparison operator between ranges
      * @param from table where keys are situated
      * @return success
      */
      virtual bool whereTimeRange(const std::string& key_start,
                                  const std::string& key_length,
                                  const std::chrono::system_clock::time_point &value_start,
                                  const std::chrono::system_clock::time_point &value_end,
                                  const std::string& oper = std::string("&&"),
                                  const std::string &from = std::string()) = 0;
    /**
     * This is a WHERE statement construction function for filters by box
     * @param key  key to compare with a value
     * @param value requested value for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereRegion(const std::string& key,
                              const Box& value,
                              const std::string& oper = std::string("&&"),
                              const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction function for custom expression
     * @param expression
     * @param value requested value for expression
     * @param oper comparison operator between expression and value
     * @return  success
     */
     virtual bool whereExpression(const std::string& expression,
                                  const std::string& value,
                                  const std::string& oper) = 0;

    /**
     * This is a WHERE statement construction class for seqtype
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereSeqtype(const std::string& key,
                               const std::string& value,
                               const std::string& oper = std::string("="),
                               const std::string &from = std::string()) = 0;

    /**
     * This is a WHERE statement construction class for inouttype
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     virtual bool whereInouttype(const std::string& key,
                                 const std::string& value,
                                 const std::string& oper = std::string("="),
                                 const std::string &from = std::string()) = 0;

    /**
     * WHERE statement part for event filter
     * @param eventkey event key by which to filter
     * @param taskname events' task name
     * @param seqnames events only for certain
     * @param filter event filter definition
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereEvent(const std::string& eventkey,
                            const std::string& taskname,
                            const std::vector<std::string>& seqnames,
                            const EventFilter & filter,
                            const std::string& from = std::string()) = 0;

    // ////////////////////////////////////////////////////////////////////////
    // IMPLEMENTED METHODS
    // ////////////////////////////////////////////////////////////////////////


    /**
     * @brief Constructor
     * @param connection previously connected connection object
     */
    explicit QueryBuilder(Connection& connection)
        : _connection(connection), _pquery_param(NULL) {}

    virtual ~QueryBuilder() {}

    /**
     * Gets object for parametrized queries
     * @return param object for parametrized queries
     */
    inline void *getQueryParam()
    { return _pquery_param; }

    /**
     * Specify custom SQL string for query
     * @param sql custom SQL string
     */
    inline void useQueryString(const std::string& sql)
    { _init_string = sql; }

    /**
     * This is to specify default schema/dataset to be inserted into
     * @param schema DB schema into which new data will be inserted
     */
    inline void useDefaultSchema(const std::string& schema)
    { _defaultSchema = schema; }

   /**
     * This is to specify the (single) table to be inserted in
     * @param table table into which new data will be inserted
     */
    inline void useDefaultTable(const std::string& table)
    { _defaultTable = table; }

protected:
    Connection  &_connection;   /**< connection object */
    void        *_pquery_param; /**< object for parametrized queries */
    std::string _init_string;   /**< init query string (whole query or table) */
    std::string _defaultSchema; /**< default db schema for queries */
    std::string _defaultTable;  /**< default table for queries */

    /**
     * Checks validity of seqtype value
     * @param value seqtype value
     * @return success
     */
    inline bool checkSeqtype(const std::string& value) const
    {
        return (value == "images"  ||
                value == "video"   ||
                value == "data");
    }
    /**
     * Checks validity of inouttype value
     * @param value inouttype value
     * @return success
     */
    inline bool checkInouttype(const std::string& value) const
    {
        return (value == "in"  ||
                value == "out");
    }

private:
    QueryBuilder() = delete;
    QueryBuilder(const QueryBuilder&) = delete;
    QueryBuilder & operator=(const QueryBuilder&) = delete;
};


} // namespace vtapi
