#pragma once

#include "backend_connection.h"
#include "backend_querybuilder.h"
#include "backend_resultset.h"


namespace vtapi {

class IBackendInterface
{
public:
    virtual ~IBackendInterface() {}

    /**
     * @brief Creates database connection object
     * @param connection_string connection string
     * @return new connection object
     */
    virtual Connection *createConnection(const std::string& connection_string) const = 0;

    /**
     * @brief Creates query building object
     * @param connection previously created database connection object
     * @return new query building object
     */
    virtual QueryBuilder *createQueryBuilder(Connection &connection) const = 0;

    /**
     * @brief Creates result set object
     * @param dbtypes database types object
     * @return  new result set object
     */
    virtual ResultSet *createResultSet(const DatabaseTypes &dbtypes) const = 0;
};

}
