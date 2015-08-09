#pragma once

#include <string>
#include "../common/types.h"
#include "backend_resultset.h"


namespace vtapi {


/**
 * @brief Class encapsulating all database connection functionality including
 * executing queries and fetching results
 *
 * Object of this class should be passed to all entities as an attribute of
 * an instance of class @ref Commons.
 */
class Connection
{
public:
    /**
     * Constructor
     * @param connectionInfo initial connection string @see vtapi.conf
     */
    explicit Connection(const std::string& connectionInfo)
        : _connInfo(connectionInfo) {}

    /**
     * Virtual destructor
     */
    virtual ~Connection() { }

    /**
     * Performs connection to database, may load dbtypes information
     * @return success
     */
    virtual bool connect() = 0;

    /**
     * Disconnects from database
     */
    virtual void disconnect () = 0;

    /**
     * Checks database connection
     * @return success
     */
    virtual bool isConnected () = 0;

    /**
     * Executes query without fetching any result set
     * @param query SQL query string
     * @param param query parameters
     * @return success
     */
    virtual bool execute(const std::string& query, void *param) = 0;

    /**
     * Executes query and fetc hes new result set
     * @param query SQl query string
     * @param param query parameters
     * @param resultSet new result set object
     * @return number of rows fetched or negative value on error
     */
    virtual int fetch(const std::string& query, void *param, ResultSet &resultSet) = 0;

    /**
     * Gets database connection object
     * @return
     */
    virtual void* getConnectionObject() = 0;

    /**
     * Gets map of preloaded database types
     * @return reference to type map
     */
    DBTYPES_MAP *getDBTypes()
    { return &this->_dbtypes; }

    /**
     * Returns last error message
     * @return error message
     */
    std::string getErrorMessage()
    { return this->_errorMessage; }

protected:
    std::string _connInfo;      /**< connection string to access the database */
    std::string _errorMessage;   /**< error message string */
    DBTYPES_MAP _dbtypes;        /**< map of database types definitions */
};


} // namespace vtapi
