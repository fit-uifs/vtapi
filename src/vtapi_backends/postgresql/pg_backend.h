#pragma once

#include <vtapi/plugins/backend_interface.h>

namespace vtapi {


class PGBackend : public vtapi::IBackendInterface
{
public:
    /**
     * @brief Creates database connection object
     * @param connection_string connection string
     * @return new connection object
     */
    Connection *createConnection(const std::string& connection_string) const override;

    /**
     * @brief Creates query building object
     * @param connection previously created database connection object
     * @return new query building object
     */
    QueryBuilder *createQueryBuilder(Connection &connection) const override;

    /**
     * @brief Creates result set object
     * @param dbtypes database types object
     * @return  new result set object
     */
    ResultSet *createResultSet(const DatabaseTypes &dbtypes) const override;
};


}
