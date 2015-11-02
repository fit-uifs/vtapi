/**
 * @file
 * @brief   Declaration of Logger class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include <string>
#include <fstream>


#ifndef VTAPI_DEBUG
    #define VTLOG_ERROR(line)   vtapi::Logger::instance().error(line)
    #define VTLOG_WARNING(line) vtapi::Logger::instance().warning(line)
    #define VTLOG_MESSAGE(line) vtapi::Logger::instance().message(line)
    #define VTLOG_QUERY(line)   vtapi::Logger::instance().query(line)
#else
    #define VT_S1(x) #x
    #define VT_S2(x) VT_S1(x)
    #define VT_LOCATION __FILE__ " : " VT_S2(__LINE__)
    #define VTLOG_ERROR(line)   vtapi::Logger::instance().error(line, VT_LOCATION)
    #define VTLOG_WARNING(line) vtapi::Logger::instance().warning(line, VT_LOCATION)
    #define VTLOG_MESSAGE(line) vtapi::Logger::instance().message(line, VT_LOCATION)
    #define VTLOG_QUERY(line)   vtapi::Logger::instance().query(line)
#endif


namespace vtapi {

/**
 * @brief Standard logger
 * 
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 *
 *//***************************************************************************/
class Logger
{
public:
    /**
     * Access singleton instance
     * @return logger singleton
     */
    static Logger& instance();

    /**
     * Configures logger instance
     * @param appname application name
     * @param logfile set file as log output
     * @param errors log errors
     * @param warnings log warnings
     * @param messages log info messages
     * @param queries log queries
     * @return succesful config
     */
    void config(const std::string& appname, const std::string& logfile,
                bool errors, bool warnings, bool messages, bool queries);
    
    /**
     * Logs error message
     * @param line error line
     * @param where error location
     */
    void error(const std::string& line, const std::string& where = std::string());

    /**
     * Logs warning message
     * @param line warning line
     * @param where warning location
     */
    void warning(const std::string& line, const std::string& where = std::string());
    
    /**
     * Logs information message
     * @param line information line
     * @param where information message location
     */
    void message(const std::string& line, const std::string& where = std::string());

    /**
     * Log SQL query message (happens where debug mode is activated)
     * @param line debug line
     * @param where debug message location
     */
    void query(const std::string& line);

private:
    std::ofstream   _log;          /**< File stream for logging */
    bool            _log_errors;   /**< Log errors */
    bool            _log_warnings; /**< Log warnings */
    bool            _log_messages; /**< Log info messages */
    bool            _log_queries;  /**< Log queries */
    
    Logger();

    /**
     * Gets a timestamp for logging
     * @return string value with timestamp
     */
    std::string timestamp() const;
    
    /**
     * Outputs line to specified stream
     * @param stream output
     * @param line write this
     */
    void output(std::ostream & stream, const std::string& line);
    
    // forbidden stuff
    
    Logger (const Logger&) = delete;
    void operator=(const Logger&) = delete;
};

} // namespace vtapi
