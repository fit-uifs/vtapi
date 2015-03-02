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
    
    
BackendFactory::backend_t BackendFactory::backend = UNKNOWN;


bool BackendFactory::initialize(const string& backendType) {
   if (backendType.compare("sqlite") == 0) {
       backend = SQLITE;
       return VT_OK;
   }
   else if (backendType.compare("postgres") == 0) {
       backend = POSTGRES;
       return VT_OK;
   }
   else {
       backend = UNKNOWN;
       return VT_FAIL;
   }
};


Connection* BackendFactory::createConnection(fmap_t *fmap, const string& connectionInfo, Logger *logger) {
    Connection *connection = NULL;
    switch (backend) {
        case POSTGRES:
#if HAVE_POSTGRESQL
            connection = new PGConnection(fmap, connectionInfo, logger);
#endif
            break;
        case SQLITE:
#if HAVE_SQLITE
       connection = new SLConnection(fmap, connectionInfo, logger);
#endif
            break;
        default:
            break;
    }
    return connection;
};

TypeManager* BackendFactory::createTypeManager(fmap_t *fmap, Connection *connection, Logger *logger, std::string &schema) {
    TypeManager *typeManager = NULL;
    switch (backend) {
        case POSTGRES:
#if HAVE_POSTGRESQL
       typeManager = new PGTypeManager(fmap, connection, logger, schema);
 #endif
           break;
        case SQLITE:
#if HAVE_SQLITE
        typeManager = new SLTypeManager(fmap, connection, logger, schema);
#endif
            break;
        default:
            break;
    }
    return typeManager;
};

QueryBuilder* BackendFactory::createQueryBuilder(fmap_t *fmap, Connection *connection, TypeManager *typeManager, Logger *logger, const string& initString) {
    QueryBuilder *queryBuilder = NULL;
    switch (backend) {
        case POSTGRES:
#if HAVE_POSTGRESQL
       queryBuilder = new PGQueryBuilder(fmap, connection, typeManager, logger, initString);
#endif
          break;
        case SQLITE:
#if HAVE_SQLITE
            queryBuilder = new SLQueryBuilder(fmap, connection, typeManager, logger, initString);
#endif
            break;
        default:
            break;
    }
    return queryBuilder;
};

ResultSet* BackendFactory::createResultSet(fmap_t *fmap, TypeManager *typeManager, Logger *logger) {
    ResultSet *resultSet = NULL;
    switch (backend) {
        case POSTGRES:
#if HAVE_POSTGRESQL
     resultSet = new PGResultSet(fmap, typeManager, logger);
#endif
         break;
        case SQLITE:
#if HAVE_SQLITE
            resultSet = new SLResultSet(fmap, typeManager, logger);
#endif
            break;
        default:
            break;
    }
    return resultSet;
};

LibLoader* BackendFactory::createLibLoader(Logger *logger) {
    LibLoader *libLoader = NULL;
    switch (backend) {
        case POSTGRES:
#if HAVE_POSTGRESQL
      libLoader = new PGLibLoader(logger);
 #endif
           break;
        case SQLITE:
#if HAVE_SQLITE
            libLoader = new SLLibLoader(logger);
#endif
            break;
        default:
            break;
    }
    return libLoader;
};
