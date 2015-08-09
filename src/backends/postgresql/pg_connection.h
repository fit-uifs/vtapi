#pragma once

#include <libpq-fe.h>
#include <libpqtypes.h>
#include <vtapi/plugins/backend_connection.h>

namespace vtapi {


class PGConnection : public Connection
{
public:
    PGConnection(const std::string& connection_string);
    ~PGConnection();

    bool connect();
    void disconnect();
    bool isConnected();

    bool execute(const std::string& query, void *param);
    int fetch(const std::string& query, void *param, ResultSet &resultSet);

    void* getConnectionObject();

private:
    PGconn *_conn;

    bool loadDBTypes();
    short getTypeCategoryFlags(char c, const std::string &name);

    static int enum_get(PGtypeArgs *args);
    static int enum_put(PGtypeArgs *args);
};


}
