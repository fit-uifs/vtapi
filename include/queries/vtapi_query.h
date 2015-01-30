/* 
 * File:   vtapi_query.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:59 PM
 */

#ifndef VTAPI_QUERY_H
#define	VTAPI_QUERY_H

#include "../data/vtapi_commons.h"
#include "../backends/vtapi_querybuilder.h"
#include "../backends/vtapi_resultset.h"

namespace vtapi {
    
/**
 * @brief Base query class
 *
 * TODO: It will be used for delayed queries (store())
 *
 * @note Error codes 20*
 */
class Query : public Commons {
public:

    QueryBuilder        *queryBuilder;  /**< Object implementing interface for building queries */
    ResultSet           *resultSet;     /**< Object implementing result set interface */
    bool                executed;       /**< Flag, disable on any change to query, enable on execute */

public:

    /**
     * Construct a query object
     * @param commons pointer of the existing commons object
     * @param initString initial query string
     */
    Query(const Commons& commons, const std::string& initString = "");
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

#endif	/* VTAPI_QUERY_H */

