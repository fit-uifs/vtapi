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
#include <boost/current_function.hpp>

#define VTLOG_MESSAGE(line) Logger::instance().message(line)
#define VTLOG_WARNING(line) Logger::instance().warning(line, BOOST_CURRENT_FUNCTION)
#define VTLOG_ERROR(line)   Logger::instance().error(line, BOOST_CURRENT_FUNCTION)
#define VTLOG_DEBUG(line)   Logger::instance().debug(line, BOOST_CURRENT_FUNCTION)


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
     * @param file set file as log output
     * @param verbose set verbose mode on/off
     * @param debug set debug mode on/off
     * @return succesful config
     */
    bool config(const std::string& filepath, bool verbose, bool debug);
    
    /**
     * Log custom message (happens always)
     * @param line printed line
     */
    void message(const std::string& line);

    /**
     * Log warning (happens only if verbose mode is activated)
     * @param line warning line
     * @param where warning location
     */
    void warning(const std::string& line, const std::string& where = std::string());
    
    /**
     * Log error (happens always)
     * @param line error line
     * @param where error location
     */
    void error(const std::string& line, const std::string& where = std::string());
    
    /**
     * Log debug message (happens where debug mode is activated)
     * @param line debug line
     * @param where debug message location
     */
    void debug(const std::string& line, const std::string& where = std::string());

private:
    std::ofstream   _log;          /**< File stream for logging */
    bool            _verbose;      /**< Print warnings */
    bool            _debug;        /**< Print queries etc. */
    

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
