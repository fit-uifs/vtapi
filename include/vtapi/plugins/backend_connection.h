#pragma once

#include "../common/dbtypes.h"
#include "backend_resultset.h"
#include <string>


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
     * Performs connection to database, may load dbtypes information
     * @return success
     */
    virtual bool connect() = 0;

    /**
     * Disconnects from database
     */
    virtual void disconnect() = 0;

    /**
     * Checks database connection
     * @return success
     */
    virtual bool isConnected() const = 0;

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


    // ////////////////////////////////////////////////////////////////////////
    // IMPLEMENTED METHODS
    // ////////////////////////////////////////////////////////////////////////


    /**
     * Constructor
     * @param connectionInfo initial connection string @see vtapi.conf
     */
    explicit Connection(const std::string& connection_info)
        : _connection_info(connection_info), _conn(NULL) {}

    virtual ~Connection() {}

    /**
     * Gets database connection object
     * @return
     */
    inline void* getConnectionObject()
    { return _conn; }

    /**
     * Gets const database connection object
     * @return
     */
    inline const void* getConnectionObject() const
    { return _conn; }

    /**
     * Gets map of preloaded database types
     * @return reference to type map
     */
    inline const DatabaseTypes & getDBTypes() const
    { return this->_dbtypes; }

    /**
     * Returns last error message
     * @return error message
     */
    inline const std::string getErrorMessage() const
    { return this->_error_message; }

protected:
    void *_conn;                    /**< connection object */
    std::string _connection_info;   /**< connection string to access the database */
    std::string _error_message;     /**< error message string */
    DatabaseTypes _dbtypes;         /**< map of database types definitions */

private:
    Connection() = delete;
    Connection(const Connection&) = delete;
    Connection & operator=(const Connection&) = delete;
};


} // namespace vtapi
