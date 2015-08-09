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

#include "../data/vtapi_commons.h"
#include "../plugins/vtapi_backend_querybuilder.h"
#include "../plugins/vtapi_backend_resultset.h"

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
class Query : public Commons
{
public:

    /**
     * Constructs a query object
     * @param commons      pointer of the existing Commons object
     * @param sql           SQL query string (or default table for derived query objects)
     */
    Query(const Commons& commons, const std::string& sql = std::string());

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

    /**
     * @brief Has query been executed
     * @return bool
     */
    bool isExecuted();

protected:
    bool                _executed;          /**< false if query has been changes */

private:
    QueryBuilder        *_pquerybuilder;    /**< interface for building queries */
    ResultSet           *_presultset;       /**< result set interface */

    Query() = delete;
    Query(const Query&) = delete;
    Query& operator=(const Query&) = delete;
};

} // namespace vtapi
