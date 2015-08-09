#pragma once

#include <sqlite3.h>
#include <vtapi/plugins/vtapi_backend_connection.h>
#include "sl_resultset.h"

namespace vtapi {


class SLConnection : public Connection
{
public:
    SLConnection(const std::string& connection_string);
    ~SLConnection();

    bool connect();
    void disconnect ();
    bool isConnected ();

    bool execute(const std::string& query, void *param);
    int fetch(const std::string& query, void *param, ResultSet &resultSet);

    void* getConnectionObject();

private:
    sqlite3 *_conn;

    bool fixSlashes(std::string& path);
    bool attachDatabase(std::string& dbfile);

};

}
