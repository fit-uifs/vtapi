
#include <Poco/ClassLibrary.h>
#include "pg_connection.h"
#include "pg_querybuilder.h"
#include "pg_resultset.h"
#include "pg_backend.h"

using namespace std;

namespace vtapi {


Connection * PGBackend::createConnection(const string& connection_string) const
{
    return new PGConnection(connection_string);
}

QueryBuilder * PGBackend::createQueryBuilder(Connection &connection) const
{
    PGConnection &pgconnection = dynamic_cast<PGConnection&>(connection);
    return new PGQueryBuilder(pgconnection);
}

ResultSet *PGBackend::createResultSet(const DatabaseTypes &dbtypes) const
{
    return new PGResultSet(dbtypes);
}


}

POCO_BEGIN_MANIFEST(vtapi::IBackendInterface)
    POCO_EXPORT_CLASS(vtapi::PGBackend)
POCO_END_MANIFEST
