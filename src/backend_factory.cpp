
#include <vtapi_global.h>
#include <backends/vtapi_backendfactory.h>

using namespace vtapi;
    
    
BackendFactory vtapi::g_BackendFactory;


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
            connection = new PGConnection(fmap, connectionInfo, logger);
            break;
        case SQLITE:
            connection = new SLConnection(fmap, connectionInfo, logger);
            break;
        default:
            break;
    }
    return connection;
};

TypeManager* BackendFactory::createTypeManager(fmap_t *fmap, Connection *connection, Logger *logger) {
    TypeManager *typeManager = NULL;
    switch (backend) {
        case POSTGRES:
            typeManager = new PGTypeManager(fmap, connection, logger);
            break;
        case SQLITE:
            typeManager = new SLTypeManager(fmap, connection, logger);
            break;
        default:
            break;
    }
    return typeManager;
};

QueryBuilder* BackendFactory::createQueryBuilder(fmap_t *fmap, Connection *connection, Logger *logger, const string& initString) {
    QueryBuilder *queryBuilder = NULL;
    switch (backend) {
        case POSTGRES:
            queryBuilder = new PGQueryBuilder(fmap, connection, logger, initString);
            break;
        case SQLITE:
            queryBuilder = new SLQueryBuilder(fmap, connection, logger, initString);
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
            resultSet = new PGResultSet(fmap, typeManager, logger);
            break;
        case SQLITE:
            resultSet = new SLResultSet(fmap, typeManager, logger);
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
            libLoader = new PGLibLoader(logger);
            break;
        case SQLITE:
            libLoader = new SLLibLoader(logger);
            break;
        default:
            break;
    }
    return libLoader;
};
