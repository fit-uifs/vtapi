/**
 * File:   internals.h
 * Author: chmelarp
 *
 * Created on 29. září 2011, 0:26
 */

#ifndef INTERNALS_H
#define	INTERNALS_H

#include <typeinfo>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <sstream>
#include <string>

#include "postgresql/libpqtypes.h"
#include "cli_settings.h"

typedef std::string String;
#define BUFFERSize 255

// be nice while destructing
#define destruct(v) if (v) { delete(v); (v) = NULL; }
#define destructall(v) if (v) { delete[](v); (v) = NULL; }
// #define boolStr(b) ((b) ? "true" : "false")

// format: 0=text, 1=binary
#define PGF 1

class TypeMap;

/**
 * Standard logger
 * Allows to log to a cerr or to a file if specified...
 *
 * // log("Testing the logging first.");
 * 2011-09-30 19:34:20: Testing the logging first.
 *
 * Programmers may use also write (just it) and a conditional debug log.
 * Error codes 10*
 *
 * @param
 *//***************************************************************************/
class Logger {
public:
    Logger(const String& filename = "");
    virtual ~Logger();

    /**
     * Log function
     * @param message
     */
    void log(const String& message);
    void debug(const String& message);

    /**
     * This is to write to the standard error log
     * @param message
     */
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
 * Error codes 12*
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
    PGconn* getConn();      // connection

// protected:
    String  conninfo;       // connection info
    PGconn* conn;           // connection
    Logger* logger;         // logger
};



/**
 * This is common predecessor to each object in the VTApi - manages connection, logging and error handling.
 *
 * This class is inherited by many and many other classes, but it manages just single resources,
 * thus there may bee a doom if someone destroys the original ones. Well, destructor should only happen
 * when isDoom is false, which is set by the only constructor: Commons(String connStr); .
 *
 * Error codes 15*
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

    /**
     * This causes a serious death
     * @param meassage
     */
    void error(int errnum, const String& logline);
    void error(const String& message);

    /**
     * This is just a warning
     * @param message
     */
    void warning(int errnum, const String& logline);
    void warning(const String& message);

    // this is to print the results
    void printRes(PGresult* res, const String& format);
    void printRes(PGresult* res, int pTuple = -1, const String& format = "");
    void read(const String& format="");

    // some functions that may be usefull
    Connector* getConnector();
    Logger* getLogger();

    String getDataset();
    String getSequence();
    String getSelection();

    int toOid(String typname);
    String toTypname(int oid);

    std::set<String> dscontext;


    
protected:
    Connector* connector; // this was most probably inherited
    Logger* logger;

    bool verbose;
    String user;
    String format;


    String dataset;
    String datasetLocation;

    String sequence;
    String sequenceLocation;

    String interval;
    String method;
    String process;

    String selection;

    TypeMap* typemap;
    void registerTypes();

    bool doom; // every derived class will have +1 = (true :)
};

/**
 * Auxiliary class which holds maping between OID an typname of data type
 */
class TypeMap {
  private:
    std::map<int, String> oid2typname;
    std::map<String, int> typname2oid;

  public:
    bool dataloaded;

    TypeMap();
    virtual ~TypeMap();

    void clear();
    bool empty();
    void insert(int oid, String typname);
    void insert(String typname, int oid);
    int size();

    int toOid(String typname);
    String toTypname(int Oid);
};

/**
 * A generic function to convert any numeric type to string
 * (any numeric type, e.g. int, float, double, etc.)
 * @param t
 * @return string
 */
template <class T>
inline String toString(const T& t) {
    std::stringstream strstr;
    strstr << t;
    return strstr.str();
};


#endif	/* INTERNALS_H */
