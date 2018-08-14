#pragma once

#include <sqlite3.h>
#include <vtapi/common/tkey.h>
#include "sl_connection.h"
#include <vtapi/plugins/backend_querybuilder.h>

namespace vtapi {


class SLQueryBuilder : public QueryBuilder
{
public:
    // ////////////////////////////////////////////////////////////////////////
    // INTERFACE IMPLEMENTATION
    // ////////////////////////////////////////////////////////////////////////


    /**
     * @brief Resets query builder to initial state
     */
    void reset() override;

    // ////////////////////////////////////////////////////////////////////////
    // query parameter methods
    // ////////////////////////////////////////////////////////////////////////

    /**
     * @brief Creates new query param
     * @note should be destroyed with destroyQueryParam()
     * @return new query param object
     */
    void *createQueryParam() const override;

    /**
     * @brief Destroys query param structure
     */
    void destroyQueryParam(void *param) const override;

    /**
     * @brief Duplicates existing query param structure
     * @return duplicated query param object
     */
    void *duplicateQueryParam(void *param) const override;

    // ////////////////////////////////////////////////////////////////////////
    // query builder methods
    // ////////////////////////////////////////////////////////////////////////

    /**
     * @brief Gets query from initialization string
     * @return query string, empty on error
     */
    std::string getGenericQuery() const override;

    /**
     * @brief Builds SELECT query string
     * @param groupby GROUP BY argument
     * @param orderby ORDER BY argument
     * @param limit LIMIT argument, 0 = no limit
     * @param offset OFFSET argument, 0 = no offset
     * @return query string, empty on error
     */
    std::string getSelectQuery(
        const std::string& groupby,
        const std::string& orderby,
        int limit,
        int offset) const override;

    /**
     * @brief Builds INSERT query string
     * @return query string, empty on error
     */
    std::string getInsertQuery() const override;

    /**
     * Builds UPDATE query
     * @return query string, empty on error
     */
    std::string getUpdateQuery() const override;

    /**
     * @brief Builds DELETE query
     * @return query string, empty on error
     */
    std::string getDeleteQuery() const override;

    /**
     * Builds SELECT COUNT(*) query
     * @return query string, empty on error
     */
    std::string getCountQuery() const override;

    /**
     * Builds BEGIN TRANSACTION query
     * @return query string, empty on error
     */
    std::string getBeginQuery() const override;

    /**
     * Builds COMMIT TRANSACTION query
     * @return query string, empty on error
     */
    std::string getCommitQuery() const override;

    /**
     * Builds ROLLBACK TRANSACTION query
     * @return query string, empty on error
     */
    std::string getRollbackQuery() const override;

    /**
     * @brief Builds query to create new dataset
     * @param name dataset name
     * @param location dataset location
     * @param friendly_name dataset user friendly name
     * @param description dataset description
     * @return query string, empty on error
     */
    std::string getDatasetCreateQuery(const std::string& name,
                                      const std::string& location,
                                      const std::string& friendly_name,
                                      const std::string& description) const override;

    /**
     * @brief Builds query to clear dataset data
     * @param name dataset name
     * @return query string, empty on error
     */
    std::string getDatasetResetQuery(const std::string& name) const override;

    /**
     * @brief Builds query to completely delete dataset
     * @param name dataset name
     * @return query string, empty on error
     */
    std::string getDatasetDeleteQuery(const std::string& name) const override;

    /**
     * @brief Builds query to create new method
     * @param name new method name
     * @param keys_definition definition of used columns
     * @param params_definition definition of used parameteres
     * @param description method description
     * @return query string, empty on error
     */
    std::string getMethodCreateQuery(const std::string &name,
                                     const TaskKeyDefinitions &keys_definition,
                                     const TaskParamDefinitions &params_definition,
                                     const std::string &description) const override;

    /**
     * @brief Builds query to delete method
     * @param name method name
     * @return query string, empty on error
     */
    std::string getMethodDeleteQuery(const std::string &name) const override;

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
    std::string getTaskCreateQuery(const std::string& name,
                                   const std::string& dsname,
                                   const std::string& mtname,
                                   const std::string& params,
                                   const std::string& prereq_task,
                                   const std::string& outputs) const override;

    /**
     * @brief Builds query to delete task
     * @param dsname parent dataset name
     * @param taskname task name
     * @return query string, empty on error
     */
    std::string getTaskDeleteQuery(const std::string& dsname,
                                   const std::string& taskname) const override;

    /**
     * @brief Builds query to get last inserted ID
     * @return query string, empty on error
     */
    std::string getLastInsertedIdQuery() const override;


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
    bool keyFrom(const std::string& table,
                 const std::string& column) override;

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
     bool keyBool(const std::string& key,
                  bool value,
                  const std::string& from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyChar(const std::string& key,
                  char value,
                  const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyString(const std::string& key,
                   const std::string& value,
                   const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyStringVector(const std::string& key,
                         const std::vector<std::string> &values,
                         const std::string &from) override;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyInt(const std::string& key,
                int value,
                const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyIntVector(const std::string& key,
                      const std::vector<int> &values,
                      const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyInt8(const std::string& key,
                long long value,
                const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyInt8Vector(const std::string& key,
                      const std::vector<long long> &values,
                      const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyFloat(const std::string& key,
                  float value,
                  const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyFloatVector(const std::string& key,
                        const std::vector<float> &values,
                        const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyFloat8(const std::string& key,
                   double value,
                   const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyFloat8Vector(const std::string& key,
                         const std::vector<double> &values,
                         const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyTimestamp(const std::string& key,
                      const std::chrono::system_clock::time_point & value,
                      const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyCvMat(const std::string& key,
                  const cv::Mat &value,
                  const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyPoint(const std::string& key,
                  Point value,
                  const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyPointVector(const std::string& key,
                        const std::vector<Point> &values,
                        const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyIntervalEvent(const std::string& key,
                          const IntervalEvent& value,
                          const std::string &from) override;


    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyEdfDescriptor(const std::string &key,
                          const EyedeaEdfDescriptor &value,
                          const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyProcessStatus(const std::string& key,
                          ProcessState::Status value,
                          const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param data binary data
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyBlob(const std::string& key,
                 const std::vector<char> &data,
                 const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keySeqtype(const std::string& key,
                    const std::string& value,
                    const std::string &from) override;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
     bool keyInouttype(const std::string& key,
                      const std::string& value,
                      const std::string &from) override;

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
     bool whereBool(const std::string& key,
                   bool value,
                   const std::string& oper,
                   const std::string &from) override;

    /**
     * This is a WHERE statement construction class
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereChar(const std::string& key,
                   char value,
                   const std::string& oper,
                   const std::string &from) override;

    /**
     * This is a WHERE statement construction class
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereString(const std::string& key,
                     const std::string& value,
                     const std::string& oper,
                     const std::string &from) override;

    /**
     * This is a WHERE statement construction function for filters by string vector
     * @param key  key to compare with a value
     * @param values requested values for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereStringVector(const std::string& key,
                            const std::vector<std::string>& values,
                            const std::string& oper,
                            const std::string &from) override;

    /**
     * This is a WHERE statement construction class for integers
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereInt(const std::string& key,
                  int value,
                  const std::string& oper,
                  const std::string &from) override;

     /**
      * This is a WHERE statement construction function for filters by integer vector
      * @param key  key to compare with a value
      * @param values requested values for key
      * @param oper comparison operator between key and value
      * @param from table where the key is situated
      * @return success
     */
     bool whereIntVector(const std::string& key,
                         const std::vector<int>& values,
                         const std::string& oper,
                         const std::string &from) override;

    /**
     * This is a WHERE statement construction class for long integers
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereInt8(const std::string& key,
                    long long value,
                    const std::string& oper,
                    const std::string &from) override;

    /**
     * This is a WHERE statement construction function for filters by long integer vector
     * @param key  key to compare with a value
     * @param values requested values for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereInt8Vector(const std::string& key,
                      const std::vector<long long>& values,
                      const std::string& oper,
                      const std::string &from) override;

    /**
     * This is a WHERE statement construction class for floats
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereFloat(const std::string& key,
                     float value,
                     const std::string& oper,
                     const std::string &from) override;

    /**
     * This is a WHERE statement construction function for filters by float vector
     * @param key  key to compare with a value
     * @param values requested values for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereFloatVector(const std::string& key,
                           const std::vector<float>& values,
                           const std::string& oper,
                           const std::string &from) override;

    /**
     * This is a WHERE statement construction class for doubles
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereFloat8(const std::string& key,
                      double value,
                      const std::string& oper,
                      const std::string &from) override;

    /**
     * This is a WHERE statement construction function for filters by double vector
     * @param key  key to compare with a value
     * @param values requested values for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereFloat8Vector(const std::string& key,
                            const std::vector<double>& values,
                            const std::string& oper,
                            const std::string &from) override;

    /**
     * This is a WHERE statement construction class for timestamp
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereTimestamp(const std::string& key,
                        const std::chrono::system_clock::time_point& value,
                        const std::string& oper,
                        const std::string &from) override;

    /**
     * This is a WHERE statement construction class for points
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool wherePoint(const std::string& key,
                    Point value,
                    const std::string& oper,
                    const std::string &from) override;

    /**
     * This is a WHERE statement construction function for filters by point vector
     * @param key  key to compare with a value
     * @param values requested values for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool wherePointVector(const std::string& key,
                           const std::vector<Point>& values,
                           const std::string& oper,
                           const std::string &from) override;

    /**
     * This is a WHERE statement construction class for pstatus type
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereProcessStatus(const std::string& key,
                            ProcessState::Status value,
                            const std::string& oper,
                            const std::string &from) override;

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
      bool whereTimeRange(const std::string& key_start,
                         const std::string& key_length,
                         const std::chrono::system_clock::time_point &value_start,
                         const std::chrono::system_clock::time_point &value_end,
                         const std::string& oper,
                         const std::string &from) override;
    /**
     * This is a WHERE statement construction function for filters by box
     * @param key  key to compare with a value
     * @param value requested value for key
     * @param oper comparison operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereRegion(const std::string& key,
                     const Box& value,
                     const std::string& oper,
                     const std::string &from) override;

    /**
     * This is a WHERE statement construction function for custom expression
     * @param expression
     * @param value requested value for expression
     * @param oper comparison operator between expression and value
     * @return  success
     */
     bool whereExpression(const std::string& expression,
                         const std::string& value,
                         const std::string& oper) override;

    /**
     * This is a WHERE statement construction class for seqtype
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereSeqtype(const std::string& key,
                      const std::string& value,
                      const std::string& oper,
                      const std::string &from) override;

    /**
     * This is a WHERE statement construction class for inouttype
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereInouttype(const std::string& key,
                        const std::string& value,
                        const std::string& oper,
                        const std::string &from) override;

     /**
      * WHERE statement part for event filter
      * @param key event key by which to filter
      * @param taskname events' task name
      * @param seqnames events only for certain
      * @param filter event filter definition
      * @param from table where the key is situated
      * @return success
      */
     bool whereEvent(const std::string& key,
                     const std::string& taskname,
                     const std::vector<std::string>& seqnames,
                     const EventFilter & filter,
                     const std::string& from) override;

    // ////////////////////////////////////////////////////////////////////////
    // OWN IMPLEMENTATION
    // ////////////////////////////////////////////////////////////////////////


    explicit SLQueryBuilder(SLConnection &connection)
        : QueryBuilder(connection) {}

    ~SLQueryBuilder()
     { reset(); }

private:
    std::string escapeColumn(const std::string& key, const std::string& table);
    std::string escapeIdent(const std::string& ident);
    std::string escapeLiteral(const std::string& literal);
};

}
