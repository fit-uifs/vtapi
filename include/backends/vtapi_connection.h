/* 
 * File:   vtapi_connection.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:36 PM
 */

#ifndef VTAPI_CONNECTION_H
#define	VTAPI_CONNECTION_H

namespace vtapi {

class ResultSet;


typedef struct {
    string  database;
} sl_param_t;

typedef struct {
    PGparam *args;
} pg_param_t;


/**
 * @brief Class encapsulating all database connection functionality including
 * executing queries and fetching results
 *
 * Object of this class should be passed to all entities as an attribute of
 * an instance of class @ref Commons.
 */
class Connection {
protected:

    fmap_t      *fmap;          /**< function address book */
    string      connInfo;       /**< connection string to access the database */
    Logger      *logger;        /**< logger object for output messaging */
    string      thisClass;      /**< class name */

    string      errorMessage;   /**< error message string */

public:
    /**
     * Constructor
     * @param fmap function address book
     * @param connectionInfo initial connection string @see vtapi.conf
     * @param logger logger object
     */
    Connection (fmap_t *fmap, const string& connectionInfo, Logger *logger) {
        this->logger    = logger;
        this->connInfo  = connectionInfo;
        this->fmap      = fmap;
    };
    /**
     * Virtual destructor
     */
    virtual ~Connection() { };

    /**
     * Performs connection to database
     * @param connectionInfo connection string @see vtapi.conf
     * @return success
     */
    virtual bool connect (const string& connectionInfo) = 0;
    /**
     * Reconnects to database
     * @param connectionInfo connection string @see vtapi.conf
     * @return success
     */
    virtual bool reconnect (const string& connectionInfo = "") = 0;
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
    virtual bool execute(const string& query, void *param) = 0;
    /**
     * Executes query and fetc hes new result set
     * @param query SQl query string
     * @param param query parameters
     * @param resultSet new result set object
     * @return number of rows fetched or negative value on error
     */
    virtual int fetch(const string& query, void *param, ResultSet *resultSet) = 0;

    /**
     * Gets database connection object
     * @return
     */
    virtual void* getConnectionObject() = 0;
    /**
     * Returns last error message
     * @return
     */
    string getErrorMessage() { return this->errorMessage; };

};


class PGConnection : public Connection {
private:

    PGconn *conn;     /**< handler of the current database connection */

public:

    PGConnection(fmap_t *fmap, const string& connectionInfo, Logger* logger = NULL);
    ~PGConnection();

    bool connect (const string& connectionInfo);
    bool reconnect (const string& connectionInfo);
    void disconnect ();
    bool isConnected ();

    bool execute(const string& query, void *param);
    int fetch(const string& query, void *param, ResultSet *resultSet);

    void* getConnectionObject();

};


class SLConnection : public Connection {
private:

    sqlite3 *conn;          /**< handler of the current database connection */

public:

    SLConnection(fmap_t *fmap, const string& connectionInfo, Logger* logger = NULL);
    ~SLConnection();

    bool connect (const string& connectionInfo);
    bool reconnect (const string& connectionInfo);
    void disconnect ();
    bool isConnected ();

    bool execute(const string& query, void *param);
    int fetch(const string& query, void *param, ResultSet *resultSet);

    void* getConnectionObject();

private:

    /**
     * Corrects angle of slashes and removes all trailing slashes
     * @param path input/output string
     * @return success
     */
    bool fixSlashes(string& path);
    /**
     * Attaches database vtapi_[dbfile].db
     * @param db
     * @return success
     */
    bool attachDatabase(string& dbfile);

};

} // namespace vtapi

#endif	/* VTAPI_CONNECTION_H */
