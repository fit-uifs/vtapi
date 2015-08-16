#pragma once

#include <vtapi/plugins/backend_interface.h>

namespace vtapi {


class SLBackend : public vtapi::IBackendInterface
{
public:
    Connection *createConnection(const std::string& connection_string) const override;
    QueryBuilder *createQueryBuilder(Connection &connection,
                                     const std::string& init_string) const override;
    ResultSet *createResultSet(DBTYPES_MAP *dbtypes) const override;
};


}
