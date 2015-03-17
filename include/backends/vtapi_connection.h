/* 
 * File:   vtapi_connection.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:36 PM
 */

#ifndef VTAPI_CONNECTION_H
#define	VTAPI_CONNECTION_H

#include "vtapi_backendbase.h"
#include "../common/vtapi_types.h"
#include "vtapi_resultset.h"


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
protected:
    std::string connInfo;       /**< connection string to access the database */
    std::string errorMessage;   /**< error message string */
    VTAPI_DBTYPES_MAP dbtypes;  /**< map of database types definitions */

public:
    /**
     * Constructor
     * @param connectionInfo initial connection string @see vtapi.conf
     */
    Connection(const std::string& connectionInfo)
    {
        this->connInfo  = connectionInfo;
    };
    /**
     * Virtual destructor
     */
    virtual ~Connection() { };

    /**
     * Performs connection to database, may load dbtypes information
     * @param connectionInfo connection string @see vtapi.conf
     * @return success
     */
    virtual bool connect (const std::string& connectionInfo = "") = 0;
    /**
     * Reconnects to database
     * @param connectionInfo connection string @see vtapi.conf
     * @return success
     */
    virtual bool reconnect (const std::string& connectionInfo = "") = 0;
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
    VTAPI_DBTYPES_MAP *getDBTypes() { return &this->dbtypes; }
    
    /**
     * Returns last error message
     * @return
     */
    std::string getErrorMessage() { return this->errorMessage; };

};

#if HAVE_POSTGRESQL
class PGConnection : public Connection, public PGBackendBase
{
protected:
    PGconn *conn;     /**< handler of the current database connection */
    
public:
    PGConnection(const PGBackendBase &base, const std::string& connectionInfo);
    ~PGConnection();

    bool connect (const std::string& connectionInfo);
    bool reconnect (const std::string& connectionInfo);
    void disconnect ();
    bool isConnected ();

    bool execute(const std::string& query, void *param);
    int fetch(const std::string& query, void *param, ResultSet *resultSet);

    void* getConnectionObject();

protected:
    bool loadDBTypes();
    short typeCategoryCharToType(char c);


    static PGConnection *glob;
    static int enum_get(PGtypeArgs *args);
    static int enum_put(PGtypeArgs *args);
    int enum_get_helper(PGtypeArgs *args);
    int enum_put_helper(PGtypeArgs *args);
    
#if HAVE_POSTGIS
    static int geometry_get(PGtypeArgs *args);
    static int geometry_put(PGtypeArgs *args);
    int geometry_get_helper(PGtypeArgs *args);
    int geometry_put_helper(PGtypeArgs *args);
#endif
};
#endif

#if HAVE_SQLITE
class SLConnection : public Connection, public SLBackendBase
{
protected:
    sqlite3 *conn;          /**< handler of the current database connection */

public:
    SLConnection(const SLBackendBase &base, const std::string& connectionInfo);
    ~SLConnection();

    bool connect (const std::string& connectionInfo);
    bool reconnect (const std::string& connectionInfo);
    void disconnect ();
    bool isConnected ();

    bool execute(const std::string& query, void *param);
    int fetch(const std::string& query, void *param, ResultSet *resultSet);

    void* getConnectionObject();

private:

    /**
     * Corrects angle of slashes and removes all trailing slashes
     * @param path input/output string
     * @return success
     */
    bool fixSlashes(std::string& path);
    /**
     * Attaches database vtapi_[dbfile].db
     * @param db
     * @return success
     */
    bool attachDatabase(std::string& dbfile);

};
#endif

} // namespace vtapi

#endif	/* VTAPI_CONNECTION_H */

