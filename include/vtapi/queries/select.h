/**
 * @file
 * @brief   Declaration of Select class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include "query.h"

namespace vtapi {

    
/**
 * @brief Class for constructing and executing SELECT queries
 * 
 * @see Basic information on page @ref KEYVALUES
 * 
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Select : public Query
{
public:
    /**
     * Constructor of a SELECT query object
     * @param table default table to SELECT FROM
     * @param commons configuration object of Commons class
     */
    Select(const Commons& commons, const std::string &table)
        : Query(commons, table, true), _limit(0), _offset(0) {}

    /**
     * Gets SELECT query string
     * @return query string
     */
    std::string getQuery() const override
    { return _pquerybuilder->getSelectQuery(_groupby, _orderby, _limit, _offset); }
    
    /**
     * Executes SELECT query and fetches result
     * @return success
     */
    bool execute() override
    { return _connection.fetch(this->getQuery(), _pquerybuilder->getQueryParam(), resultset()) >= 0; }
    
    /**
     * Shifts offset, executes SELECT query and fetches result
     * @return success
     */
    bool executeNext()
    {
        _offset += _limit;
        return this->execute();
    }
    
    /**
     * This is used to specify a table for FROM statement and a column vector for SELECT statement
     * @param table    table to select
     * @param column   column for select
     * @return success
     * @note It may be called more times.
     */
    bool from(const std::string& table, const std::string& column)
    { return _pquerybuilder->keyFrom(table, column); }

    /**
     * Sets number of rows to be fetched by LIMIT section of query
     * @param limit   number of rows to be fetched
     */
    void setLimit(int limit)
    { _limit = limit; }
    
    /**
     * Order result set by key (possibly including ASC/DESC)
     * @param key key to order resultset by
     */
    void setOrderBy(const std::string& key)
    { _orderby = key; }

    /**
     * GROUP BY clause (one column only)
     * @param key key to group by
     */
    void setGroupBy(const std::string& key)
    { _groupby = key; }

private:
    int _limit;             /**< LIMIT value */
    int _offset;            /**< OFFSET value */
    std::string _orderby;   /**< ORDER BY value  */
    std::string _groupby;   /**< GROUP BY value */
    
    Select() = delete;
};

} // namespace vtapi
