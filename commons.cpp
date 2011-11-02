/*
 * File:   VTApi.cpp
 * Author: chmelarp
 *
 * Created on 29. 9. 2011, 10:42
 */

#include "commons.h"

#include "postgresql/libpqtypes.h"
#include "postgresql/vt-print.h"
#include "vtapi.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <map>


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
        logger->log("WARNING 122:" + String(PQerrorMessage(conn)));
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
    logger    = orig.logger;
    connector = orig.connector;
    verbose   = orig.verbose;
    user      = orig.user;
    format    = orig.format;
    dataset   = orig.dataset;
    sequence  = orig.sequence;
    interval  = orig.interval;
    method    = orig.method;
    process   = orig.process;
    selection = orig.selection;

    dscontext = orig.dscontext;
    typemap   = orig.typemap;
    doom      = true;       // won't destroy the connector and logger
}

Commons::Commons(Connector& orig) {
    logger    = orig.getLogger();
    connector = &orig;
    typemap   = new TypeMap();
    doom      = true;       // won't destroy the connector and logger
}

Commons::Commons(const String& connStr, const String& logFilename) {
    logger    = new Logger(logFilename);
    connector = new Connector(connStr, logger);
    typemap   = new TypeMap();
    doom      = false;        // finally, we can destroy the above objects without any DOOM :D
}

/**
 * The most best ever of all VTAPI Commons constructors that should be always used
 */
Commons::Commons(const gengetopt_args_info& args_info, const String& logFilename) {
    logger    = new Logger(logFilename);
    connector = new Connector(args_info.connection_arg, logger);
    typemap   = new TypeMap();
    verbose   = args_info.verbose_given;

    user      = String(args_info.user_arg);
    format    = String(args_info.format_arg);
    // FIXME: proc je mozna nastavit vice datasetu? (i vseho ostatniho)
    // protoze jsem to tak dal ve vtapi.ggo (argument je multiple) --Vojta
    // FIXME Vojta: hmm a ma to vyznam? mohlo by to nekoho mast, kdyz to dal nepouzivame... resp. budem?
    // takto to ma pouziti jako omezeni kontextu pro selecty atp. -- Vojta
    // pak bude asi zbytecne plnit tyhle promenne dataset, sequence tady v konstruktoru...
    for (int i = 0; i < args_info.dataset_given; i++)
        dscontext.insert(String(args_info.dataset_arg[i]));
/*
    dataset   = String(args_info.dataset_arg[0]);
    sequence  = String(args_info.sequence_arg[0]);
    interval  = String(args_info.interval_arg[0]);
    method    = String(args_info.method_arg[0]);
    process   = String(args_info.process_arg[0]);
    selection = String(args_info.selection_arg[0]);
*/
    doom      = false;           // finally, we can destroy the above objects without any DOOM :D
}

/**
 * This is a doom destructor, which should never happen :)
 */
Commons::~Commons() {
    if (!doom) {
        destruct(connector);        // the doom is very close     !!!!!
        destruct(logger);           // you shouldn't debug these lines!
        destruct(typemap);
    }
}

Connector* Commons::getConnector() {
    return connector;
} 

Logger* Commons::getLogger() {
    return logger;
}


void Commons::error(const String& logline) {
    logger->log("ERROR! " + logline);
    exit(1);
}

void Commons::error(int errnum, const String& logline) {
    logger->log("ERROR " + toString(errnum) + "! " + logline);
    exit(1);
}

void Commons::warning(const String& logline) {
    logger->log("ERROR! " + logline);
}

void Commons::warning(int errnum, const String& logline) {
    logger->log("WARNING " + toString(errnum) + ": " + logline);
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

