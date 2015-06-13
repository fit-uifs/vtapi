/**
 * @file
 * @brief   Declaration of Query class which is general class for SQL queries/commands.
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include "../data/vtapi_commons.h"
#include "../backends/vtapi_querybuilder.h"
#include "../backends/vtapi_resultset.h"

namespace vtapi {
    
/**
 * @brief Base query class
 *
 * @todo: It will be used for delayed queries (store())
 *
 * @note Error codes 20*
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Query : public Commons {
public:

    QueryBuilder        *queryBuilder;  /**< Object implementing interface for building queries */
    ResultSet           *resultSet;     /**< Object implementing result set interface */
    bool                executed;       /**< Flag, disable on any change to query, enable on execute */

public:

    /**
     * Constructs a query object
     * @param commons      pointer of the existing Commons object
     * @param initString   initial query string
     */
    Query(const Commons& commons, const std::string& initString);

    /**
     * Destructor
     */
    ~Query();
    /**
     * This expands the query, so you can check it before the execution
     * @return string value with the query
     */
    std::string getQuery();
    /**
     * Begins transaction, executes will be stored
     * @return success
     */
    bool beginTransaction();
    /**
     * Commits pending transaction
     * @return success
     */
    bool commitTransaction();
    /**
     * Rolls back pending transaction
     * @return success
     */
    bool rollbackTransaction();
    
    /**
     * This will commit your query
     * @return success
     */
    bool execute();
    /**
     * Clears the query object to its original state
     */
    void reset();

    /**
     * Checks query object for validity (query builder, result set)
     * @return success
     */
    bool checkQueryObject();

};

} // namespace vtapi
