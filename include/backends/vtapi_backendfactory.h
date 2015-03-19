/* 
 * File:   vtapi_backendfactory.h
 * Author: vojca
 *
 * Created on May 7, 2013, 1:02 PM
 */

#ifndef VTAPI_BACKENDFACTORY_H
#define	VTAPI_BACKENDFACTORY_H

#include "vtapi_backendbase.h"
#include "vtapi_connection.h"
#include "vtapi_querybuilder.h"
#include "vtapi_resultset.h"

namespace vtapi {

/**
 * @brief Factory for creating objects of backend-specific polymorphic classes
 *
 * Class needs to be initialized with initialize() function before using it.
 * It creates objects of these four classes:
 * - @ref BackendBase
 *      base class of other classes, handles logging/library loading
 * - @ref Connection
 *      communication with backend
 * - @ref ResultSet
 *      accessing retrieved values
 * - @ref LibLoader
 *      loading library functions
 */
class BackendFactory {
public:
    
// backend type
typedef enum {
    BACKEND_UNKNOWN = 0,
    BACKEND_SQLITE,
    BACKEND_POSTGRES
} BACKEND_T;

public:

    /**
     * Converts string to backend type
     * @param backendType backend type string
     * @return backend type enum
     */
    static BACKEND_T type(const std::string& backendType);

    /**
     * Creates object of class @ref BackendBase
     * @param backend backend type
     * @param logger message logging object
     * @return pointer to new backend base object
     */
    static BackendBase* createBackendBase(BACKEND_T backend, Logger *logger);

    /**
     * Creates object of class @ref Connection
     * @param backend backend type
     * @param base previously created backend base
     * @param connectionInfo connection string
     * @return pointer to new connection object
     */
    static Connection* createConnection(BackendFactory::BACKEND_T backend, const BackendBase &base, const std::string& connectionInfo);
    
    /**
     * Creates object of class @ref QueryBuilder
     * @param backend backend type
     * @param base previously created backend base
     * @param connection connection object
     * @param initString initializing string (query or table)
     * @see Query
     * @return pointer to new query building object
     */
    static QueryBuilder* createQueryBuilder(BACKEND_T backend, const BackendBase &base, void *connection, const std::string& initString = "");

    /**
     * @param backend backend type
     * @param base previously created backend base
     * @param dbtypes pre-filled map of database types definitions
     * @return pointer to new result set object
     */
    static ResultSet* createResultSet(BACKEND_T backend, const BackendBase &base, DBTYPES_MAP *dbtypes = NULL);
};

} // namespace vtapi

#endif	/* VTAPI_BACKENDFACTORY_H */

