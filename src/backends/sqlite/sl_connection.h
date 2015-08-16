#pragma once

#include <sqlite3.h>
#include <vtapi/plugins/backend_connection.h>
#include "sl_resultset.h"

namespace vtapi {


class SLConnection : public Connection
{
public:
    SLConnection(const std::string& connection_string);
    ~SLConnection();

    bool connect() override;
    void disconnect () override;
    bool isConnected () override;

    bool execute(const std::string& query, void *param) override;
    int fetch(const std::string& query, void *param, ResultSet &resultSet) override;

    void* getConnectionObject() override;

private:
    sqlite3 *_conn;

    bool fixSlashes(std::string& path);
    bool attachDatabase(std::string& dbfile);

};

}
