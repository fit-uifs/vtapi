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
#include "../data/vtapi_commons.h"
#include "../backends/vtapi_querybuilder.h"
#include "../backends/vtapi_resultset.h"

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
    QueryBuilder        *_queryBuilder;  /**< Object implementing interface for building queries */
    ResultSet           *_resultSet;     /**< Object implementing result set interface */
    bool                _executed;       /**< Flag, disable on any change to query, enable on execute */

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
     * Checks query object for validity (query builder, result set)
     * @return success
     */
    bool checkQueryObject();

private:
    Query() = delete;
    Query(const Query&) = delete;
    Query& operator=(const Query&) = delete;
};

} // namespace vtapi
