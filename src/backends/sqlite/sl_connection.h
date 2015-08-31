#pragma once

#include <sqlite3.h>
#include <vtapi/plugins/backend_connection.h>
#include "sl_resultset.h"

namespace vtapi {


class SLConnection : public Connection
{
public:
    // ////////////////////////////////////////////////////////////////////////
    // INTERFACE IMPLEMENTATION
    // ////////////////////////////////////////////////////////////////////////

    /**
     * Performs connection to database, may load dbtypes information
     * @return success
     */
    bool connect() override;

    /**
     * Disconnects from database
     */
    void disconnect() override;

    /**
     * Checks database connection
     * @return success
     */
    bool isConnected() const override;

    /**
     * Executes query without fetching any result set
     * @param query SQL query string
     * @param param query parameters
     * @return success
     */
    bool execute(const std::string& query, void *param) override;

    /**
     * Executes query and fetc hes new result set
     * @param query SQl query string
     * @param param query parameters
     * @param resultSet new result set object
     * @return number of rows fetched or negative value on error
     */
    int fetch(const std::string& query, void *param, ResultSet &resultSet) override;


    // ////////////////////////////////////////////////////////////////////////
    // OWN IMPLEMENTATION
    // ////////////////////////////////////////////////////////////////////////


    SLConnection(const std::string& connection_string)
        : Connection(connection_string) {}

    virtual ~SLConnection()
    { disconnect(); }

private:
    bool fixSlashes(std::string& path) const;
    bool attachDatabase(std::string& dbfile);

};

}
