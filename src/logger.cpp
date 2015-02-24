/**
 * @file
 * @brief   Methods of Logger class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref Licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <common/vtapi_global.h>
#include "common/vtapi_serialize.h"
#include <common/vtapi_logger.h>

using std::string;
using std::ofstream;
using std::cout;
using std::cerr;
using std::endl;

using namespace vtapi;


Logger::Logger(const string& filename, bool verbose) {
    logFilename     = filename;
    this->verbose   = verbose;

    if (!logFilename.empty()) {
        logStream.open(filename.c_str());
        // FIXME: logStream.setf nolock, nolockbuf

        if (logStream.fail()) {
            logFilename = ""; // just cerr??? logger failures cannot be logged :)
            cerr << endl << timestamp() << ": ERROR 101! Logger cannot open the file specified, trying stderr instead." << endl;
        }
    }
    if (logFilename.empty()) {
#ifdef COPYRDBUF
        // http://stdcxx.apache.org/doc/stdlibug/34-2.html
        logStream.copyfmt(cerr);
        logStream.clear(cerr.rdstate());
        logStream.basic_ios<char>::rdbuf(cerr.rdbuf());
        // FIXME: if this is screwed, we won't log - there are no exceptions necessary :)
#else
        cerr << timestamp() << ": ERROR 102! Logger cannot open the file specified, NO LOGGING ENABLED." << endl;
#endif
    }
}

Logger::~Logger() {
    if (!logFilename.empty() && !logStream.fail()) logStream.close();
}

void Logger::write(const string& logline) {
    if (logStream.good()) {
        logStream << logline;
        logStream.flush();
    }
}

void Logger::log(const string& logline) {
    if (logStream.good()) {
        logStream << timestamp() << ": " << logline << endl;
        logStream.flush();
    }
}

void Logger::debug(const string& logline) {
#ifdef _DEBUG
    if (logStream.good() && verbose) {
        log(logline);
    }
#endif
}

string Logger::timestamp() {
    time_t timer;
    time(&timer);
    char timeBuffer[64];
    strftime(timeBuffer, 64, "%Y-%m-%d %H:%M:%S", gmtime(&timer));
    return string(timeBuffer);
}

void Logger::error(const string& logline, const string& thisMethod) {
    log(string("ERROR at ") + thisMethod + ":\n" + logline);
    exit(1);
}

void Logger::error(int errnum, const string& logline, const string& thisMethod) {
    log(string("ERROR ") + toString(errnum) + " at " + thisMethod + ": " + logline);
    exit(1);
}

void Logger::warning(const string& logline, const string& thisMethod) {
    log(string("WARNING at ") + thisMethod + ": " + logline);
}

void Logger::warning(int errnum, const string& logline, const string& thisMethod) {
    log(string("WARNING ") + toString(errnum) + " at " + thisMethod + ": " + logline);
}
