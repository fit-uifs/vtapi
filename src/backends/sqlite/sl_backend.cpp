
#include <Poco/ClassLibrary.h>
#include "sl_connection.h"
#include "sl_querybuilder.h"
#include "sl_resultset.h"
#include "sl_backend.h"

using namespace std;

namespace vtapi {


Connection * SLBackend::createConnection(const string& connection_string) const
{
    return new SLConnection(connection_string);
}

QueryBuilder * SLBackend::createQueryBuilder(Connection &connection,
                                             const string& init_string) const
{
    SLConnection &slconnection = dynamic_cast<SLConnection&>(connection);
    return new SLQueryBuilder(slconnection, init_string);
}

ResultSet *SLBackend::createResultSet(DBTYPES_MAP *dbtypes) const
{
    (dbtypes);
    return new SLResultSet();
}


}

POCO_BEGIN_MANIFEST(vtapi::IBackendInterface)
    POCO_EXPORT_CLASS(vtapi::SLBackend)
POCO_END_MANIFEST
