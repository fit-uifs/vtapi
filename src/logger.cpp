/**
 * @file
 * @brief   Methods of Logger class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <iostream>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <vtapi/common/global.h>
#include <vtapi/common/exception.h>
#include <vtapi/common/serialize.h>
#include <vtapi/common/logger.h>

using namespace std;

namespace vtapi {


Logger& Logger::instance()
{
    static Logger instance;
    
    return instance;
}

void Logger::config(const string& logfile, bool errors, bool warnings, bool debug)
{
    if (_log.is_open())
        _log.close();

    if (!logfile.empty()) {
        Poco::Path logpath = Poco::Path(logfile).makeAbsolute();
        Poco::File(logpath.parent()).createDirectories();

        _log.open(logpath.toString(), ios::app);
        if (!_log.is_open())
            throw BadConfigurationException("cannot create log file: " + logpath.toString());

        message("------------------------------------------------------");
    }
    
    _log_errors = errors;
    _log_warnings = warnings;
    _log_debug = debug;
}

void Logger::message(const string& line)
{
    output(cout, line);
}

void Logger::error(const string& line, const string& where)
{
    if (_log_errors) {
        string message(timestamp());
        message += " ERROR: ";
        message += line;
        if (!where.empty()) {
            message += " @ ";
            message += where;
        }

        output(cerr, message);
    }
}

void Logger::warning(const string& line, const string& where)
{
    if (_log_warnings) {
        string message(timestamp());
        message += " WARNING: ";
        message += line;
        if (!where.empty()) {
            message += " @ ";
            message += where;
        }
        
        output(cerr, message);
    }
}

void Logger::debug(const string& line, const string& where)
{
    if (_log_debug) {
        string message(timestamp());
        message += " DEBUG: ";
        message += line;
        if (!where.empty()) {
            message += " @ ";
            message += where;
        }

        output(cout, message);
    }
}

string Logger::timestamp()
{
    time_t timer;
    time(&timer);
    char timeBuffer[64];
    strftime(timeBuffer, 64, "%Y-%m-%d %H:%M:%S", gmtime(&timer));
    return string(timeBuffer);
}

void Logger::output(std::ostream & stream, const std::string& line)
{
    if (_log.is_open())
        _log << line << endl;
    else
        stream << line << endl;
}


}
