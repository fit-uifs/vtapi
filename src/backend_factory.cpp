/**
 * @file
 * @brief   Factory methods of all polymorphic classes - create: Connection, TypeManager, ResultSet, QueryBuilder and LibLoader
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <common/vtapi_global.h>
#include <common/vtapi_serialize.h>
#include <backends/vtapi_backendfactory.h>

using std::string;

using namespace vtapi;
    

BackendFactory::BACKEND_T BackendFactory::type(const string& backendType) {
   if (backendType.compare("sqlite") == 0) {
       return BACKEND_SQLITE;
   }
   else if (backendType.compare("postgres") == 0) {
       return BACKEND_POSTGRES;
   }
   else {
       return BACKEND_UNKNOWN;
   }
};

BackendBase* BackendFactory::createBackendBase(BackendFactory::BACKEND_T backend, Logger *logger)
{
    BackendBase *base = NULL;
    switch (backend) {
        case BACKEND_POSTGRES:
#if HAVE_POSTGRESQL
            base = new PGBackendBase(logger);
#endif
            break;
        case BACKEND_SQLITE:
#if HAVE_SQLITE
            base = new SLBackendBase(logger);
#endif
            break;
        default:
            break;
    }
    return base;
}

Connection* BackendFactory::createConnection(BackendFactory::BACKEND_T backend, const BackendBase &base, const string& connectionInfo) {
    Connection *connection = NULL;
    switch (backend)
    {
        case BACKEND_POSTGRES:
#if HAVE_POSTGRESQL
            connection = new PGConnection((PGBackendBase &)base, connectionInfo);
#endif
            break;
        case BACKEND_SQLITE:
#if HAVE_SQLITE
            connection = new SLConnection((SLBackendBase &) base, connectionInfo);
#endif
            break;
        default:
            break;
    }
    return connection;
};

QueryBuilder* BackendFactory::createQueryBuilder(BackendFactory::BACKEND_T backend, const BackendBase &base, void *connection, const string& initString) {
    QueryBuilder *queryBuilder = NULL;
    switch (backend) {
        case BACKEND_POSTGRES:
#if HAVE_POSTGRESQL
            queryBuilder = new PGQueryBuilder((PGBackendBase &) base, connection, initString);
#endif
          break;
        case BACKEND_SQLITE:
#if HAVE_SQLITE
            queryBuilder = new SLQueryBuilder((SLBackendBase &) base, connection, initString);
#endif
            break;
        default:
            break;
    }
    return queryBuilder;
};

ResultSet* BackendFactory::createResultSet(BACKEND_T backend, const BackendBase &base, VTAPI_DBTYPES_MAP *dbtypes) {
    ResultSet *resultSet = NULL;
    switch (backend) {
        case BACKEND_POSTGRES:
#if HAVE_POSTGRESQL
            resultSet = new PGResultSet((PGBackendBase &) base, dbtypes);
#endif
         break;
        case BACKEND_SQLITE:
#if HAVE_SQLITE
            resultSet = new SLResultSet((SLBackendBase &) base);
#endif
            break;
        default:
            break;
    }
    return resultSet;
};
