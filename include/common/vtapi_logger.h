/* 
 * File:   vtapi_logger.h
 * Author: vojca
 *
 * Created on May 7, 2013, 1:05 PM
 */

#ifndef VTAPI_LOGGER_H
#define	VTAPI_LOGGER_H

namespace vtapi {
    class Logger;
} 

#include "vtapi_global.h"

namespace vtapi {


/**
 * @brief Standard logger
 *
 * Allows to log to a cerr or to a file if specified...
 *
 * // log("Testing the logging first.");
 * 2011-09-30 19:34:20: Testing the logging first.
 *
 * Programmers may use also write (just it) and a conditional debug log.
 * @note Error codes 10*
 *
 *//***************************************************************************/
class Logger {
protected:

    string          logFilename;    /**< Name of the file for storing logs */
    ofstream        logStream;      /**< Stream of file for storing logs */
    bool            verbose;        /**< Print queries and debug info */

public:

    /**
     * Constructor
     * @param filename Name of the file for storing logs
     */
    Logger(const string& filename = "", bool verbose = false);
    /**
     * Destructor
     */
    ~Logger();

    /**
     * Log function puts a timestamp with the message into the logstream
     * @param message message, which to be logged
     */
    void log(const string& message);
    /**
     * Debug function flushes a timestamp with the message immediately into the logstream
     * @param message
     */
    void debug(const string& message);

    /**
     * This is to write to the standard error log
     * @param message
     */
    void write(const string& message);
    /**
     * Get a timestamp for logging
     * @return string value with timestamp
     * @note Timestamp format is "YYYY-MM-DD HH:NN:SS", where NN represents minutes
     */
    string timestamp();
        /**
     * This causes a serious death
     * @param errnum number of the error
     * @param logline error message to display
     * @param thisMethod method in which error occurred
     */
    void error(int errnum, const string& logline, const string& thisMethod);
    /**
     * This causes a serious death
     * @param message error message to display
     * @param thisMethod method in which error occurred
     */
    void error(const string& message, const string& thisMethod);

    /**
     * This is just a warning
     * @param errnum number of the warning
     * @param logline warning message to display
     * @param thisMethod method in which warning occurred
     */
    void warning(int errnum, const string& logline, const string& thisMethod);
    /**
     * This is just a warning
     * @param message warning message to display
     * @param thisMethod method in which warning occurred
     */
    void warning(const string& message, const string& thisMethod);

};

} // namespace vtapi

#endif	/* VTAPI_LOGGER_H */

