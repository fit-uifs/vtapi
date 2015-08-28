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

#include <ctime>
#include <list>
#include "../data/processstate.h"
#include "../data/intervalevent.h"
#include "../data/commons.h"
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
class Select : public QueryWhere
{
public:
    /**
     * Constructor of a SELECT query object
     * @param commons configuration object of Commons class
     */
    Select(const Commons& commons, const std::string &table);
    
    /**
     * Gets SELECT query string
     * @return query string
     */
    std::string getQuery() override;
    
    /**
     * Executes SELECT query and fetches result
     * @return success
     */
    bool execute() override;
    
    /**
     * Shifts offset, executes SELECT query and fetches result
     * @return success
     */
    bool executeNext();
    
    /**
     * This is used to specify a table for FROM statement and a column list for SELECT statement
     * @param table    table to select
     * @param column   column for select
     * @return success
     * @note It may be called more times.
     */
    bool from(const std::string& table, const std::string& column);

    /**
     * Sets number of rows to be fetched by LIMIT section of query
     * @param limit   number of rows to be fetched
     */
    void setLimit(const int limit);
    
    /**
     * Order result set by key (possibly including ASC/DESC)
     * @param key key to order resultset by
     */
    void setOrderBy(const std::string& key);

    /**
     * GROUP BY clause (one column only)
     * @param key key to group by
     */
    void setGroupBy(const std::string& key);

private:
    std::string _groupby;    /**< String used for the GROUP BY statement */
    std::string _orderby;    /**< String used for the ORDER BY statement */

    int _limit;      /**< Specify a size (a number of rows) of the resultset */
    int _offset;     /**< Specify an index of row, where the resultset starts */
    
    Select() = delete;
    Select(const Select&) = delete;
    Select& operator=(const Select&) = delete;
};

} // namespace vtapi
