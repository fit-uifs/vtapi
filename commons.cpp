/*
 * File:   VTApi.cpp
 * Author: chmelarp
 *
 * Created on 29. 9. 2011, 10:42
 */

#include "commons.h"

#include "postgresql/libpqtypes.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>


/* ************************************************************************** */

Logger::Logger(const String& filename) {
    logFilename = filename;

    if (!logFilename.empty()) {
        logStream.open(filename.c_str());
        // FIXME: logStream.setf nolock, nolockbuf

        if (logStream.fail()) {
            logFilename = ""; // just cerr??? logger failures cannot be logged :)
            std::cerr << timestamp() << ": ERROR 10! Logger cannot open the file specified, trying stderr instead." << std::endl;
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

void Logger::error(const String& logline) {
    logStream << timestamp() << ": ERROR! " << logline << std::endl;

#ifdef _DEBUG
    logStream.flush();
#endif

    exit(1);
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
    if (!reconnect(connectionInfo)) logger->error("51: The connection couldn't been established.");
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
        logger->log("ERROR! 55: " + String(PQerrorMessage(conn)));
        return false;
    }

    PQinitTypes(conn);  // libpqtypes ... http://libpqtypes.esilo.com/

    return true; // success
}


bool Connector::connected() {
    bool success = false;

    if (PQstatus(conn) != CONNECTION_OK) {
        logger->log("ERROR! 55: " + String(PQerrorMessage(conn)));
        return success = false;
    }

    // SELECT version();
    PGresult *res = PQexecf(conn, "SELECT version();");
    PGtext text;                        // char* .. not necessarily initialized

    success = PQgetf(res, 0, "%text",   // get formated field values from tuple 0
                          0, &text);    // 0th text field

    if(!success) {
        logger->log(String("ERROR! 56: ") +  PQgeterror());
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

PGconn* Connector::getConnection() {           // connection
    return conn;
}

Connector::~Connector() {
    PQfinish(conn);
}




/* ************************************************************************** */
Commons::Commons(const Commons& orig) {
    logger    = orig.logger;
    connector = orig.connector;
    doom      = true;       // won't destroy the connector and logger
}

Commons::Commons(Connector& orig) {
    logger    = orig.getLogger();
    connector = &orig;
    doom      = true;       // won't destroy the connector and logger
}

// Commons::Commons(const String& connStr) {
//     Commons(connStr, "");   // TODO: different constructor, please
// }

Commons::Commons(const String& connStr, const String& logFilename) {
    logger    = new Logger(logFilename);
    connector = new Connector(connStr, logger);
    doom    = false;        // finally, we can destroy the above objects without any DOOM :D
}


/**
 * And a shorter one...
 */
Commons::Commons(const gengetopt_args_info& args_info) {
    logger    = new Logger("");
    String* connStr = new String(args_info.connection_arg);
    connector = new Connector(*connStr, logger);
    doom    = false;        // finally, we can destroy the above objects without any DOOM :D
}

/**
 * This is a doom destructor, which should never happen :)
 */
Commons::~Commons() {
    if (!doom) {
        destruct(connector);        // the doom is very close     !!!!!
        destruct(logger);           // you shouldn't debug these lines!
    }
}


Connector* Commons::getConnector() {
    return connector;
} 

Logger* Commons::getLogger() {

}



