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

#include "../data/commons.h"
#include "../plugins/backend_interface.h"
#include "../plugins/backend_connection.h"
#include "../plugins/backend_querybuilder.h"
#include "../plugins/backend_resultset.h"
#include <string>
#include <memory>

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
    Query(const Commons& commons, const std::string& sql, bool sql_is_table = false)
        : _connection(const_cast<Commons&>(commons).connection())
    {
        // query builder and resul tset objects
        _pquerybuilder = std::shared_ptr<QueryBuilder>(commons.backend().createQueryBuilder(_connection));
        _presultset = std::shared_ptr<ResultSet>(commons.backend().createResultSet(_connection.getDBTypes()));

        // set default schema
        _pquerybuilder->useDefaultSchema(commons._context.dataset);

        // set initial query string and default table
        if (sql_is_table)
            _pquerybuilder->useDefaultTable(sql);
        else
            _pquerybuilder->useQueryString(sql);
    }

    Query(Query &&) = default;
    Query & operator=(Query &&) = default;

    /**
     * @brief Destructor
     */
    virtual ~Query() {}
    
    /**
     * @brief Returns currently build query
     * @return string value with the query
     */
    virtual std::string getQuery() const
    { return _pquerybuilder->getGenericQuery(); }
    
    /**
     * This will commit your query
     * @return success
     */
    virtual bool execute()
    { return _connection.execute(this->getQuery(), _pquerybuilder->getQueryParam()); }
    
    /**
     * @brief Clears the query object to its original state
     */
    void reset()
    {
        _pquerybuilder->reset();
        _presultset->clear();
    }

    /**
     * @brief QueryBuilder object accessor
     * @return query builder
     */
    QueryBuilder & querybuilder()
    { return *_pquerybuilder; }

    /**
     * @brief ResultSet object accessor
     * @return result set
     */
    ResultSet & resultset()
    { return *_presultset; }

protected:
    Connection &_connection;                        /**< shared connection to backend */
    std::shared_ptr<QueryBuilder> _pquerybuilder;   /**< interface for building queries */
    std::shared_ptr<ResultSet> _presultset;         /**< result set interface */

private:
    Query() = delete;

    friend class KeyValues;
};


} // namespace vtapi
