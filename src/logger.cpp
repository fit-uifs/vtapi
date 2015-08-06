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
#include <common/vtapi_global.h>
#include <common/vtapi_serialize.h>
#include <common/vtapi_logger.h>

using namespace std;

namespace vtapi {


Logger& Logger::instance()
{
    static Logger instance;
    
    return instance;
}

bool Logger::config(const string& filepath, bool verbose, bool debug)
{
    bool ret = true;

    if (_log.is_open())
        _log.close();

    if (!filepath.empty()) {
        _log.open(filepath);
        ret = _log.is_open();
    }
    
    _verbose = verbose;
    _debug = debug;
    
    return ret;
}

void Logger::message(const string& line)
{
    output(cout, line);
}

void Logger::warning(const string& line, const string& where)
{
    if (_verbose) {
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

void Logger::error(const string& line, const string& where)
{
    string message(timestamp());
    message += " ERROR: ";
    message += line;
    if (!where.empty()) {
        message += " @ ";
        message += where;
    }

    output(cerr, message);
}

void Logger::debug(const string& line, const string& where)
{
    if (_debug) {
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
