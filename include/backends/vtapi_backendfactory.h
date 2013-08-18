/* 
 * File:   vtapi_backendfactory.h
 * Author: vojca
 *
 * Created on May 7, 2013, 1:02 PM
 */

#ifndef VTAPI_BACKENDFACTORY_H
#define	VTAPI_BACKENDFACTORY_H

namespace vtapi {
    class BackendFactory;
}

#include "vtapi_connection.h"
#include "vtapi_typemanager.h"
#include "vtapi_querybuilder.h"
#include "vtapi_resultset.h"
#include "vtapi_libloader.h"
#include "../common/vtapi_logger.h"
#include "../common/vtapi_global.h"

namespace vtapi {

typedef enum {
    UNKNOWN = 0,
    SQLITE,
    POSTGRES
} backend_t;


/**
 * @brief Factory for creating objects of backend-specific polymorphic classes
 *
 * Class needs to be initialized with initialize() function before using it.
 * It creates objects of these five classes:
 * - @ref Connection
 *      communication with backend
 * - @ref TypeManager
 *      registering and managing data types
 * - @ref QueryBuilder
 *      building SQL queries
 * - @ref ResultSet
 *      accessing retrieved values
 * - @ref LibLoader
 *      loading library functions
 */
class BackendFactory {
public:

    static backend_t        backend;        /**< backend type */

    /**
     * Initializes factory with given backend type
     * @param backendType backend type
     */
    static void initialize(const string& backendType = "") {
       if (backendType.compare("sqlite") == 0) {
           backend = SQLITE;
       }
       else if (backendType.compare("postgres") == 0) {
           backend = POSTGRES;
       }
       else {
           backend = UNKNOWN;
       }
    };

    /**
     * Creates object of class @ref Connection
     * @param FUNC_MAP library functions address book
     * @param connectionInfo connection string or database folder
     * @param logger message logging object
     * @return NULL if factory is uninitialized, connection object otherwise
     */
    static Connection* createConnection(func_map_t *FUNC_MAP, const string& connectionInfo, Logger *logger) {
        Connection *connection = NULL;
        switch (backend) {
            case POSTGRES:
                connection = new PGConnection(FUNC_MAP, connectionInfo, logger);
                break;
            case SQLITE:
                connection = new SLConnection(FUNC_MAP, connectionInfo, logger);
                break;
            default:
                break;
        }
        return connection;
    };

    /**
     * Creates object of class @ref TypeManager
     * @param FUNC_MAP library functions address book
     * @param connection connection object
     * @param logger message logging object
     * @return NULL if factory is uninitialized, data type managing object otherwise
     */
    static TypeManager* createTypeManager(func_map_t *FUNC_MAP, Connection *connection, Logger *logger) {
        TypeManager *typeManager = NULL;
        switch (backend) {
            case POSTGRES:
                typeManager = new PGTypeManager(FUNC_MAP, connection, logger);
                break;
            case SQLITE:
                typeManager = new SLTypeManager(FUNC_MAP, connection, logger);
                break;
            default:
                break;
        }
        return typeManager;
    };

    /**
     * Creates object of class @ref QueryBuilder
     * @param FUNC_MAP library functions address book
     * @param connection connection object
     * @param logger message logging object
     * @param initString initializing string (query or table)
     * @see Query
     * @return NULL if factory is uninitialized, query building object otherwise
     */
    static QueryBuilder* createQueryBuilder(func_map_t *FUNC_MAP, Connection *connection, Logger *logger, const string& initString) {
        QueryBuilder *queryBuilder = NULL;
        switch (backend) {
            case POSTGRES:
                queryBuilder = new PGQueryBuilder(FUNC_MAP, connection, logger, initString);
                break;
            case SQLITE:
                queryBuilder = new SLQueryBuilder(FUNC_MAP, connection, logger, initString);
                break;
            default:
                break;
        }
        return queryBuilder;
    };

    /**
     * Creates object of class @ref ResultSet
     * @param FUNC_MAP library functions address book
     * @param typeManager data type managing object
     * @param logger message logging object
     * @return NULL if factory is uninitialized, result set object otherwise
     */
    static ResultSet* createResultSet(func_map_t *FUNC_MAP, TypeManager *typeManager, Logger *logger) {
        ResultSet *resultSet = NULL;
        switch (backend) {
            case POSTGRES:
                resultSet = new PGResultSet(FUNC_MAP, typeManager, logger);
                break;
            case SQLITE:
                resultSet = new SLResultSet(FUNC_MAP, typeManager, logger);
                break;
            default:
                break;
        }
        return resultSet;
    };

    /**
     * Creates object of class @ref LibLoader
     * @return NULL if factory is uninitialized, library loading object otherwise
     */
    static LibLoader* createLibLoader(Logger *logger) {
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
};

} // namespace vtapi

#endif	/* VTAPI_BACKENDFACTORY_H */

