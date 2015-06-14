#pragma once

#include <string>
#include "vtapi_backendbase.h"
#include "vtapi_resultset.h"
#include "../common/vtapi_types.h"


namespace vtapi {

class Connection;
class PGConnection;
class SLConnection;


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
    : connInfo(connectionInfo) {}
    
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
    virtual int fetch(const std::string& query, void *param, ResultSet *resultSet) = 0;

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
    { return &this->dbtypes; }
    
    /**
     * Returns last error message
     * @return error message
     */
    std::string getErrorMessage()
    { return this->errorMessage; };

protected:
    std::string connInfo;       /**< connection string to access the database */
    std::string errorMessage;   /**< error message string */
    DBTYPES_MAP dbtypes;        /**< map of database types definitions */
};

#if HAVE_POSTGRESQL
class PGConnection : public Connection, public PGBackendBase
{
public:
    PGConnection(const PGBackendBase &base, const std::string& connectionInfo);
    ~PGConnection();

    bool connect();
    void disconnect();
    bool isConnected();

    bool execute(const std::string& query, void *param);
    int fetch(const std::string& query, void *param, ResultSet *resultSet);

    void* getConnectionObject();

private:
    PGconn *conn;
    
    bool loadDBTypes();
    short getTypeCategoryFlags(char c, const std::string &name);

    static int enum_get(PGtypeArgs *args);
    static int enum_put(PGtypeArgs *args);
};
#endif

#if HAVE_SQLITE
class SLConnection : public Connection, public SLBackendBase
{
public:
    SLConnection(const SLBackendBase &base, const std::string& connectionInfo);
    ~SLConnection();

    bool connect();
    void disconnect ();
    bool isConnected ();

    bool execute(const std::string& query, void *param);
    int fetch(const std::string& query, void *param, ResultSet *resultSet);

    void* getConnectionObject();

private:
    sqlite3 *conn;

    bool fixSlashes(std::string& path);
    bool attachDatabase(std::string& dbfile);

};
#endif

} // namespace vtapi
