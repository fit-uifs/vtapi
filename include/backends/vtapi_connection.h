/* 
 * File:   vtapi_connection.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:36 PM
 */

#ifndef VTAPI_CONNECTION_H
#define	VTAPI_CONNECTION_H

namespace vtapi {
    class Connection;
    class PGConnection;
    class SLConnection;
}

#include "vtapi_libloader.h"
#include "vtapi_resultset.h"
#include "../common/vtapi_logger.h"
#include "../common/vtapi_global.h"

namespace vtapi {

typedef struct {
    string  database;
} sl_param_t;

typedef struct {
    PGparam *args;
} pg_param_t;


//TODO: comment
class Connection {
protected:

    func_map_t  *FUNC_MAP;      /**< function address book */
    string      connInfo;       /**< connection string to access the database */
    Logger      *logger;        /**< logger object for output messaging */
    string      thisClass;      /**< class name */

    string      errorMessage;   /**< error message string */

public:

    Connection (func_map_t *FUNC_MAP, const string& connectionInfo, Logger *logger) {
        this->logger    = logger;
        this->connInfo  = connectionInfo;
        this->FUNC_MAP  = FUNC_MAP;
    };
    virtual ~Connection() { };

    virtual bool connect (const string& connectionInfo) = 0;
    virtual bool reconnect (const string& connectionInfo = "") = 0;
    virtual void disconnect () = 0;
    virtual bool isConnected () = 0;

    virtual int execute(const string& query, void *param) = 0;
    virtual int fetch(const string& query, void *param, ResultSet *resultSet) = 0;

    virtual void* getConnectionObject() = 0;

    string getErrorMessage() { return this->errorMessage; };

};


class PGConnection : public Connection {
private:

    PGconn *conn;     /**< handler of the current database connection */

public:

    PGConnection(func_map_t *FUNC_MAP, const string& connectionInfo, Logger* logger = NULL);
    ~PGConnection();

    bool connect (const string& connectionInfo);
    bool reconnect (const string& connectionInfo);
    void disconnect ();
    bool isConnected ();

    int execute(const string& query, void *param);
    int fetch(const string& query, void *param, ResultSet *resultSet);

    void* getConnectionObject();

};


class SLConnection : public Connection {
private:

    sqlite3 *conn;          /**< handler of the current database connection */

public:

    SLConnection(func_map_t *FUNC_MAP, const string& connectionInfo, Logger* logger = NULL);
    ~SLConnection();

    bool connect (const string& connectionInfo);
    bool reconnect (const string& connectionInfo);
    void disconnect ();
    bool isConnected ();

    int execute(const string& query, void *param);
    int fetch(const string& query, void *param, ResultSet *resultSet);

    void* getConnectionObject();

private:

    int fixSlashes(string& path);

    int attachDatabase(string& db);

};

} // namespace vtapi

#endif	/* VTAPI_CONNECTION_H */

