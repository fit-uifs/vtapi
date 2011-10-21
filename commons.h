/**
 * File:   internals.h
 * Author: chmelarp
 *
 * Created on 29. září 2011, 0:26
 */

#ifndef INTERNALS_H
#define	INTERNALS_H

#include "postgresql/libpqtypes.h"

#include <fstream>
#include "cli_settings.h"

#include <string>
#include <vector>
typedef std::string String;

#define BUFFERSize 255

// be nice while destructing
#define destruct(v) if (v) { free(v); (v) = NULL; }
#define boolStr(b) ((b) ? "true" : "false")




/**
 * Standard logger
 * Allows to log to a cerr or to a file if specified...
 *
 * // log("Testing the logging first.");
 * 2011-09-30 19:34:20: Testing the logging first.
 *
 * Programmers may use also write (just it) and a conditional debug log.
 * Error codes 10-30.
 *
 * @param
 *//***************************************************************************/
class Logger {
public:
    Logger(const String& filename = "");
    virtual ~Logger();

    void log(const String& message);
    void debug(const String& message);

    /**
     * This causes a serious death
     * @param meassage
     */
    void error(const String& message);

    void write(const String& message);

    String timestamp();

protected:
    String logFilename;
    std::ofstream logStream;

};


/**
 * Maintains connection to the database
 * Based on a connection string supplied of the form:
 *   Connector("host=localhost port=5432 dbname=db user=postgres password='secret'", new Logger());
 * The class requires a Logger too.
 *
 * Error codes 50-70.
 *
 * @param orig
 *//***************************************************************************/
class Connector {
public:
    Connector(const Connector& orig);

    /**
     * This is the fine constructor
     * @param connectionInfo
     * @param logger
     */
    Connector(const String& connectionInfo, Logger* logger = NULL);

    virtual ~Connector();

    /**
     * YES, this is a fun that (re)connects to the database
     * @param connectionInfo
     * @return success
     */
    bool reconnect(const String& connectionInfo);
    
    /**
     * SELECT and log version of the database :)
     * @return success
     */
    bool connected();

    Logger* getLogger();
    PGconn* getConnection();           // connection

protected:
    String conninfo;   // connection info
    PGconn* conn;           // connection
    Logger* logger;         // logger
};



/**
 * This is a class where queries will be constructed (but it may be KeyValues as well)
 * TODO: discuss and use libpqtypes
 *
 *//***************************************************************************/
class Select {
public:
    void field(String);
    void condition(String);
    void order(String);

    // discuss the use
    bool execute();

protected:
    // this should be some vectors
    String select;
    String from;
    String where;
    String groupby;
    String orderby;
    int limit;
    int offset;

    // this should be it here or should it be there?
    PGresult* res;
};


/**
 * This is common predecessor to each object in the VTApi - manages connection, logging and error handling.
 *
 * This class is inherited by many and many other classes, but it manages just single resources,
 * thus there may bee a doom if someone destroys the original ones. Well, destructor should only happen
 * when isDoom is false, which is set by the only constructor: Commons(String connStr); .
 *//***************************************************************************/
class Commons {
public:
    /**
     * This is OK for most of applications if there are some Commons or derived classes.
     * The default constructor should never exist - Commons();
     * @param orig
     */
    Commons(const Commons& orig); // hmm, much better

    /**
     * This is OK for most of applications if there are some Commons or derived classes.
     * The default constructor should never exist - Commons();
     * @param orig
     */
    Commons(Connector& other); // hmm, much better

    /**
     * A custom API startup constructor
     * This construtors should not cause any doom
     */
    Commons(const String& connStr, const String& logFilename = "");
    Commons(const String connStr, const String location, const String user, const String password, const String logFilename = "");

    /**
     * And a shorter one...
     */
    Commons(const gengetopt_args_info& args_info, const String& logFilename = "");

    // this constructor is close to doom as capitalism;
    // this is different from comunism, which has been destroyed already
    virtual ~Commons();


    Connector* getConnector();
    Logger* getLogger();

protected:
    Connector* connector; // this was most probably inherited
    Logger* logger;

    bool doom; // every derived class will have +1 = (true :)
};

#endif	/* INTERNALS_H */
