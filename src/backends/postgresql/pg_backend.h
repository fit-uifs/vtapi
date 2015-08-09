#pragma once

#include <vtapi/plugins/backend_interface.h>

namespace vtapi {


class PGBackend : public vtapi::IBackendInterface
{
public:
    Connection *createConnection(const std::string& connection_string) const;
    QueryBuilder *createQueryBuilder(Connection &connection,
                                     const std::string& init_string) const;
    ResultSet *createResultSet(DBTYPES_MAP *dbtypes) const;
};


}
