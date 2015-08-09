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

#define VT_S1(x) #x
#define VT_S2(x) VT_S1(x)
#define VT_LOCATION __FILE__ " : " VT_S2(__LINE__)

#define VTLOG_MESSAGE(line) Logger::instance().message(line)
#define VTLOG_WARNING(line) Logger::instance().warning(line, VT_LOCATION)
#define VTLOG_ERROR(line)   Logger::instance().error(line, VT_LOCATION)
#define VTLOG_DEBUG(line)   Logger::instance().debug(line, VT_LOCATION)


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
     * @param logfile set file as log output
     * @param errors log errors
     * @param warnings log warnings
     * @param debug log debug messages
     * @return succesful config
     */
    bool config(const std::string& logfile, bool errors, bool warnings, bool debug);
    
    /**
     * Log custom message (happens always)
     * @param line printed line
     */
    void message(const std::string& line);

    /**
     * Log error (happens always)
     * @param line error line
     * @param where error location
     */
    void error(const std::string& line, const std::string& where = std::string());

    /**
     * Log warning (happens only if verbose mode is activated)
     * @param line warning line
     * @param where warning location
     */
    void warning(const std::string& line, const std::string& where = std::string());
    
    /**
     * Log debug message (happens where debug mode is activated)
     * @param line debug line
     * @param where debug message location
     */
    void debug(const std::string& line, const std::string& where = std::string());

private:
    std::ofstream   _log;          /**< File stream for logging */
    bool            _log_errors;   /**< Log errors */
    bool            _log_warnings; /**< Log warnings */
    bool            _log_debug;    /**< Log debug messages */
    

    /**
     * Gets a timestamp for logging
     * @return string value with timestamp
     */
    std::string timestamp();
    
    /**
     * Outputs line to specified stream
     * @param stream output
     * @param line write this
     */
    void output(std::ostream & stream, const std::string& line);
    
    // forbidden stuff
    
    Logger() {}
    Logger (const Logger&) = delete;
    void operator=(const Logger&) = delete;
};

} // namespace vtapi
