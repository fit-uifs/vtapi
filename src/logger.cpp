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

Logger::Logger()
    : _log_errors(false), _log_warnings(false), _log_messages(false), _log_queries(false)
{}


void Logger::config(const string& logfile, bool errors, bool warnings, bool messages, bool queries)
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
    _log_messages = messages;
    _log_queries = queries;
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

void Logger::message(const string& line, const string& where)
{
    if (_log_messages) {
        string message(timestamp());
        message += " INFO: ";
        message += line;
        if (!where.empty()) {
            message += " @ ";
            message += where;
        }

        output(cout, message);
    }
}

void Logger::query(const string& line)
{
    if (_log_queries) {
        output(cout, line);
    }
}

string Logger::timestamp() const
{
    return toString(chrono::system_clock::now());
}

void Logger::output(std::ostream & stream, const std::string& line)
{
    if (_log.is_open())
        _log << line << endl;
    else
        stream << line << endl;
}


}
