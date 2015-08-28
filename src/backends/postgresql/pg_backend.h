#pragma once

#include <vtapi/plugins/backend_interface.h>

namespace vtapi {


class PGBackend : public vtapi::IBackendInterface
{
public:
    Connection *createConnection(const std::string& connection_string) const override;
    QueryBuilder *createQueryBuilder(Connection &connection) const override;
    ResultSet *createResultSet(DBTYPES_MAP *dbtypes) const override;
};


}
