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

// Default constructor - cerr output
Logger::Logger() {
    constructor("");
};

Logger::Logger(const String& filename) {
    constructor(filename);
}

void Logger::constructor(const String& filename) {
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
    this->logger = logger;
    reconnect(connectionInfo);
}

/**
 * YES, this is a fun that connects to the database
 * @param connectionInfo
 * @return success
 */
bool Connector::reconnect(const String& connectionInfo) {
    conninfo = connectionInfo;
    conn = PQconnectdb(conninfo.c_str());

    PQinitTypes(conn);  // libpqtypes ... http://libpqtypes.esilo.com/
    if (PQstatus(conn) != CONNECTION_OK) {
        logger->log("ERROR 50! " + String(PQerrorMessage(conn)));
        return false;
    }

    return true; // success
}


bool Connector::connected() {
    bool success = false;

    PQinitTypes(conn);
    if (PQstatus(conn) != CONNECTION_OK) {
        logger->log("ERROR 55! " + String(PQerrorMessage(conn)));
        return success = false;
    }

    // SELECT version();
    PGresult *res = PQexecf(conn, "SELECT version();");
    PGtext text;                        // char* .. not necessarily initialized

    success = PQgetf(res, 0, "%text",   // get formated field values from tuple 0
                          0, &text);    // 0th text field

    if(!success) {
        logger->log(String("ERROR 56! ") +  PQgeterror());
    } else {
        logger->log(text);
    }

    PQclear(res);
    return success;
}


Connector::~Connector() {
}




/* ************************************************************************** */
Commons::Commons(const Commons& orig) {
    logger    = orig.logger;
    connector = orig.connector;
    isDoom    = 1 + orig.isDoom;
}

Commons::Commons(const String& connStr) {
    Commons(connStr, "");
}

Commons::Commons(const String& connStr, const String& logFilename) {
    logger    = new Logger(logFilename);
    connector = new Connector(connStr, logger);
    isDoom    = false; // finally, we can destroy the above objects without any DOOM :D
}


/**
 * This is a doom destructor, which should never happen :)
 */
Commons::~Commons() {
    if (!isDoom) {
        destruct(connector);        // the doom is very close     !!!!!
        destruct(logger);           // you shouldn't debug these lines!
    }
}







