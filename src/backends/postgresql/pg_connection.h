#pragma once

#include <vtapi/plugins/backend_connection.h>
#include <libpq-fe.h>
#include <libpqtypes.h>

namespace vtapi {


class PGConnection : public Connection
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


    explicit PGConnection(const std::string& connection_info)
        : Connection(connection_info) {}

    virtual ~PGConnection()
    { disconnect(); }

private:
    bool loadDBTypes();
    void getTypeCategoryFlags(char c, const std::string &name,
                              short int & category, char & flags) const;

    static void noticeProcessor(void *arg, const char *message);

    static int enum_get(PGtypeArgs *args);
    static int enum_put(PGtypeArgs *args);
};


}
