/* 
 * File:   vtapi_backendfactory.h
 * Author: vojca
 *
 * Created on May 7, 2013, 1:02 PM
 */

#ifndef VTAPI_BACKENDFACTORY_H
#define	VTAPI_BACKENDFACTORY_H

#include "vtapi_libloader.h"
#include "vtapi_connection.h"
#include "vtapi_querybuilder.h"
#include "vtapi_resultset.h"
#include "vtapi_typemanager.h"

namespace vtapi {

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
    
// backend type
typedef enum {
    UNKNOWN = 0,
    SQLITE,
    POSTGRES
} backend_t;

public:

    static backend_t backend;        /**< backend type */

    /**
     * Initializes factory with given backend type
     * @param backendType backend type
     * @return success
     */
    static bool initialize(const std::string& backendType = "");

    /**
     * Creates object of class @ref Connection
     * @param fmap library functions address book
     * @param connectionInfo connection string or database folder
     * @param logger message logging object
     * @return NULL if factory is uninitialized, connection object otherwise
     */
    static Connection* createConnection(fmap_t *fmap, const std::string& connectionInfo, Logger *logger);

    /**
     * Creates object of class @ref TypeManager
     * @param fmap library functions address book
     * @param connection connection object
     * @param logger message logging object
     * @return NULL if factory is uninitialized, data type managing object otherwise
     */
    static TypeManager* createTypeManager(fmap_t *fmap, Connection *connection, Logger *logger);

    /**
     * Creates object of class @ref QueryBuilder
     * @param fmap library functions address book
     * @param connection connection object
     * @param logger message logging object
     * @param initString initializing string (query or table)
     * @see Query
     * @return NULL if factory is uninitialized, query building object otherwise
     */
    static QueryBuilder* createQueryBuilder(fmap_t *fmap, Connection *connection, Logger *logger, const std::string& initString);

    /**
     * Creates object of class @ref ResultSet
     * @param fmap library functions address book
     * @param typeManager data type managing object
     * @param logger message logging object
     * @return NULL if factory is uninitialized, result set object otherwise
     */
    static ResultSet* createResultSet(fmap_t *fmap, TypeManager *typeManager, Logger *logger);

    /**
     * Creates object of class @ref LibLoader
     * @return NULL if factory is uninitialized, library loading object otherwise
     */
    static LibLoader* createLibLoader(Logger *logger);
};

} // namespace vtapi

#endif	/* VTAPI_BACKENDFACTORY_H */

