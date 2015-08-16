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

    bool connect() override;
    void disconnect() override;
    bool isConnected() override;

    bool execute(const std::string& query, void *param) override;
    int fetch(const std::string& query, void *param, ResultSet &resultSet) override;

    void* getConnectionObject() override;

private:
    PGconn *_conn;

    bool loadDBTypes();
    short getTypeCategoryFlags(char c, const std::string &name);

    static void noticeProcessor(void *arg, const char *message);

    static int enum_get(PGtypeArgs *args);
    static int enum_put(PGtypeArgs *args);
};


}
