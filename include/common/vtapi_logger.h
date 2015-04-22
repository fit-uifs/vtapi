/**
 * @file
 * @brief   Declaration of Logger class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef VTAPI_LOGGER_H
#define	VTAPI_LOGGER_H


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
 *
 * @note Error codes 10*
 * 
 * @todo @b doc: lack of clarity sentence: "Programmers may use also write (just it) and a conditional debug log." - what the autor wanted to say?
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 *
 *//***************************************************************************/
class Logger {
protected:

    std::string     logFilename;    /**< Name of the file for storing logs */
    std::ofstream   logStream;      /**< Stream of file for storing logs */
    bool            m_verbose;      /**< Print warnings */
    bool            m_debug;        /**< Print queries etc. */

public:

    /**
     * Constructor
     * @param filename   name of the file for storing logs
     * @param verbose verbosity
     * @param debug print debug
     */
    Logger(const std::string& filename = "", bool verbose = false, bool debug = false);
    /**
     * Destructor
     */
    ~Logger();

    /**
     * Log function puts a timestamp with a message into a logstream
     * @param message   log level message
     */
    void log(const std::string& message);
    /**
     * This is to write to the standard error log
     * @param message   logged message
     */
    void write(const std::string& message);
    /**
     * Gets a timestamp for logging
     * @return string value with timestamp
     * @note Timestamp format is "YYYY-MM-DD HH:NN:SS", where NN represents minutes
     */
    std::string timestamp();
    /**
     * This causes a serious death
     * @param errnum       number of the error (error code)
     * @param logline      error message to display
     * @param thisMethod   method in which error occurred
     */
    void error(int errnum, const std::string& logline, const std::string& thisMethod);
    /**
     * This causes a serious death
     * @param message      error message to display
     * @param thisMethod   method in which error occurred
     */
    void error(const std::string& message, const std::string& thisMethod);

    /**
     * This is just a warning
     * @param errnum       number of the warning (warning code)
     * @param logline      warning message to display
     * @param thisMethod   method in which warning occurred
     */
    void warning(int errnum, const std::string& logline, const std::string& thisMethod);
    /**
     * This is just a warning
     * @param message      warning message to display
     * @param thisMethod   method in which warning occurred
     */
    void warning(const std::string& message, const std::string& thisMethod);
    /**
     * Debug function flushes a timestamp with a message immediately into a logstream
     * @param message   debug level message
     */
    void debug(const std::string& message);
};

} // namespace vtapi

#endif	/* VTAPI_LOGGER_H */

