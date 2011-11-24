/**
 * @file
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * 
 * @section DESCRIPTION
 *
 * Methods of classes for commons
 */



#include <fstream>
#include <iostream>
#include <map>
#include <stdlib.h>

#include "postgresql/libpqtypes.h"
#include "postgresql/vt-print.h"

#include "vtapi_commons.h"
#include "vtapi.h"



/* ************************************************************************** */

Logger::Logger(const String& filename) {
    logFilename = filename;

    if (!logFilename.empty()) {
        logStream.open(filename.c_str());
        // FIXME: logStream.setf nolock, nolockbuf

        if (logStream.fail()) {
            logFilename = ""; // just cerr??? logger failures cannot be logged :)
            std::cerr << timestamp() << ": ERROR 101! Logger cannot open the file specified, trying stderr instead." << std::endl;
        }
    }

    if (logFilename.empty()) {
        // http://stdcxx.apache.org/doc/stdlibug/34-2.html
        logStream.copyfmt(std::cout);
        logStream.clear(std::cout.rdstate());
        logStream.basic_ios<char>::rdbuf(std::cout.rdbuf());
        // FIXME: if this is scrued, we won't log - there are no exceptions necessary :)
    }
}

void Logger::write(const String& logline) {
    logStream << logline;

#ifdef _DEBUG
    logStream.flush();
#endif
}

void Logger::log(const String& logline) {
    logStream << timestamp() << ": " << logline << std::endl;

#ifdef _DEBUG
    logStream.flush();
#endif
}

void Logger::debug(const String& logline) {
#ifdef _DEBUG
    log(logline);
#endif
}

String Logger::timestamp() {
    time_t timer;
    time(&timer);
    // struct tm* times = localtime(&rawtime);
    char timeBuffer[BUFFERSize];
    strftime(timeBuffer, BUFFERSize, "%Y-%m-%d %H:%M:%S", localtime(&timer));

    return String(timeBuffer);
}


Logger::~Logger() {
    if (!logFilename.empty() && !logStream.fail()) logStream.close();
}



/* ************************************************************************** */
Connector::Connector(const String& connectionInfo, Logger* logger) {
    if (logger) this->logger = logger;
    else logger = new Logger();

    conn = NULL;
    if (!reconnect(connectionInfo)) {
        logger->log("ERROR 121! The connection couldn't been established.");
        exit(1);
    }
}

/**
 * YES, this is a fun that connects to the database
 * @param connectionInfo
 * @return success
 */
bool Connector::reconnect(const String& connectionInfo) {
     PQfinish(conn);

    conninfo = connectionInfo;
    conn = PQconnectdb(conninfo.c_str());

    if (PQstatus(conn) != CONNECTION_OK) {
        logger->log("WARNING 122: " + String(PQerrorMessage(conn)));
        return false;
    }

    PQinitTypes(conn);  // libpqtypes ... http://libpqtypes.esilo.com/

    return true; // success
}


bool Connector::connected() {
    bool success = false;

    if (PQstatus(conn) != CONNECTION_OK) {
        logger->log("WARNING 125!" + String(PQerrorMessage(conn)));
        return success = false;
    }

    // SELECT version();
    PGresult *res = PQexecf(conn, "SELECT version();");
    PGtext text;                        // char* .. not necessarily initialized

    success = PQgetf(res, 0, "%text",   // get formated field values from tuple 0
                          0, &text);    // 0th text field

    if(!success) {
        logger->log("WARNING 126!" + String(PQgeterror()));
        PQfinish(conn);
    } else {
        logger->log(text);
    }

    PQclear(res);
    res = NULL;

    return success;
}


Logger* Connector::getLogger() {
    return logger;
}

PGconn* Connector::getConn() {           // connection
    return conn;
}

Connector::~Connector() {
    PQfinish(conn);
}




/* ************************************************************************** */
Commons::Commons(const Commons& orig) {
    thisClass = orig.thisClass + "+Commons(Commons&)";

    logger    = orig.logger;
    connector = orig.connector;
    verbose   = orig.verbose;
    user      = orig.user;
    format    = orig.format;
    baseLocation = orig.baseLocation;

    dataset   = orig.dataset;
    datasetLocation = orig.datasetLocation;
    sequence  = orig.sequence;
    sequenceLocation  = orig.sequenceLocation;
    interval  = orig.interval;

    method    = orig.method;
    process   = orig.process;
    selection = orig.selection;

    typemap   = orig.typemap;
    doom      = true;       // won't destroy the connector and logger
}

Commons::Commons(Connector& orig) {
    thisClass = "Commons(Connector&)";

    logger    = orig.getLogger();
    connector = &orig;
    typemap   = new TypeMap();
    doom      = true;       // won't destroy the connector and logger
}

Commons::Commons(const String& connStr, const String& logFilename) {
    thisClass = "Commons(String&, String&)";

    logger    = new Logger(logFilename);
    connector = new Connector(connStr, logger);
    typemap   = new TypeMap();
    doom      = false;        // finally, we can destroy the above objects without any DOOM :D
}

/**
 * The most best ever of all VTAPI Commons constructors that should be always used
 */
Commons::Commons(const gengetopt_args_info& args_info, const String& logFilename) {
    thisClass = "Commons(gengetopt_args_info&, String&)";

    logger    = new Logger(logFilename);
    connector = new Connector(args_info.connection_arg, logger);
    typemap   = new TypeMap();
    verbose   = args_info.verbose_given;

    user      = args_info.user_given ? String(args_info.user_arg) : String ("");
    format    = args_info.format_given ? String(args_info.format_arg) : String ("");
    dataset   = args_info.dataset_given ? String(args_info.dataset_arg) : String ("");
    sequence  = args_info.sequence_given ? String(args_info.sequence_arg) : String ("");
    method    = args_info.method_given ? String(args_info.method_arg) : String ("");
    process   = args_info.process_given ? String(args_info.process_arg) : String ("");
    selection = args_info.selection_given ? String(args_info.selection_arg) : String ("");
    // TODO: interval pair

    user      = args_info.user_given ? String(args_info.user_arg) : String("");
    format    = args_info.format_given ? String(args_info.format_arg) : String("");

    baseLocation = args_info.location_given ? String(args_info.location_arg) : String("");
    doom      = false;           // finally, we can destroy the above objects without any DOOM :D
}

/**
 * This should be called from any other (virtual) constructor
 * @return
 */
void Commons::beDoomed() {
    if (!doom) {
        destruct(connector);        // the doom is very close     !!!!!
        destruct(logger);           // you shouldn't debug these lines!
        destruct(typemap);
    }
}

/**
 * This is a doom destructor, which should never happen :)
 */
Commons::~Commons() {
    this->beDoomed();
}

Connector* Commons::getConnector() {
    return connector;
} 

Logger* Commons::getLogger() {
    return logger;
}


void Commons::error(const String& logline) {
    logger->log("ERROR at " + thisClass + ": " + logline);
    exit(1);
}

void Commons::error(int errnum, const String& logline) {
    logger->log("ERROR " + toString(errnum) + " at " + thisClass + ": " + logline);
    exit(1);
}

void Commons::warning(const String& logline) {
    logger->log("WARNING at " + thisClass + ": " + logline);
}

void Commons::warning(int errnum, const String& logline) {
    logger->log("WARNING " + toString(errnum) + " at " + thisClass + ": " + logline);
}


String Commons::getDataset() {
    if (dataset.empty()) warning(153, "No dataset specified");
    return (dataset);
}

String Commons::getSequence() {
    if (sequence.empty()) warning(153, "No sequence specified");
    return (sequence);
}

String Commons::getSelection() {
    if (selection.empty()) warning(155, "No selection specified");
    return selection;
}


String Commons::getDataLocation() {
    if (baseLocation.empty()) error(156, "No (base) location specified");
    if (datasetLocation.empty()) error(156, "No (dataset) location specified");
    if (sequenceLocation.empty()) warning(156, "No sequence location specified");
    
    return (baseLocation + datasetLocation + sequenceLocation);
}

void Commons::printRes(PGresult* res, const String& format) {
    printRes(res, -1, format);
}

void Commons::printRes(PGresult* res, int pTuple, const String& format) {
    if(!res) {
        warning(158, "No result set to print.\n" + String(PQgeterror()));
        return;
    }

    String f;
    if (!format.empty()) f = format;
    else f = this->format;

    PQprintOpt opt = {0};
    if (f.compare("standard") == 0) {
        opt.header    = 1;
        opt.align     = 1;
        opt.fieldSep  = (char *) "|";
    }
    else if (f.compare("csv") == 0) {
        opt.fieldSep  = (char *) ",";
    }
    else if (f.compare("html") == 0) {
        opt.html3    = 1;
        //opt.caption  = (char *) "";
        //opt.tableOpt = (char *) ""
        opt.fieldSep = (char *) "";
    }
    // TODO: eventuelne dalsi formaty (binary, sparse)
    else {
        opt.fieldSep  = (char *) "";
        warning(159, "Print format not implemented " + f);
    }

    vtPQprint(stdout, res, &opt, pTuple);
}

void Commons::registerTypes() {
    if (! this->typemap->dataloaded) {
        KeyValues* kv = new KeyValues(*this);
        kv->select = new Select(*this);
        kv->select->from("pg_catalog.pg_type", "oid, typname");

        while (kv->next()) {
            this->typemap->insert(kv->getOid("oid"), kv->getName("typname"));
        }

        this->typemap->dataloaded = true;

        delete kv;
    }

}

int Commons::toOid(String typname) {
    this->registerTypes();
    return this->typemap->toOid(typname);
}

String Commons::toTypname(const int oid) {
    this->registerTypes();
    return this->typemap->toTypname(oid);
}


