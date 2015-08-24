/**
 * @file
 * @brief   Methods of Commons class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <Poco/Manifest.h>
#include <Poco/Path.h>
#include <vtapi/common/exception.h>
#include <vtapi/common/global.h>
#include <vtapi/data/commons.h>

using namespace std;

namespace vtapi {


Commons::Commons(const Poco::Util::AbstractConfiguration &config)
{
    _is_owner = true;
    _pconfig = new CONFIG();
    _pbackend = NULL;
    _pconnection = NULL;
    _ploader = NULL;

    try {
        // load config
        loadConfig(config);

        // initialize global logger
        Logger::instance().config(_pconfig->logfile, _pconfig->log_errors,
                                  _pconfig->log_warnings, _pconfig->log_debug);

        // load backend interface + connection
        loadBackend();
    }
    catch (Exception &e)
    {
        vt_destruct(_pconfig);
        throw;
    }
}

Commons::Commons(const Commons& orig, bool new_copy)
{
    _is_owner = new_copy;
    _pconfig = NULL;
    _pbackend = NULL;
    _pconnection = NULL;
    _ploader = NULL;

    if (new_copy) {
        // copy config
        _pconfig = new CONFIG(*orig._pconfig);

        try
        {
            // load backend interface + connection
            loadBackend();
        }
        catch (Exception &e)
        {
            vt_destruct(_pconfig);
            throw;
        }
    }
    else {
        _pconfig = orig._pconfig;
        _pbackend = orig._pbackend;
        _pconnection = orig._pconnection;
        _ploader = orig._ploader;
    }

    _context = orig._context;
}

void Commons::loadBackend()
{
    // get library path
    string lib_name = getBackendLibName();

    try
    {
        // load library
        _ploader = new Poco::ClassLoader<IBackendInterface>();
        _ploader->loadLibrary(lib_name);

        // load plugin interface
        auto & plugins = *_ploader->begin()->second;
        string plugin_name;
        for (const auto & plugin : plugins) {
            plugin_name = plugin->name();
            break;  // get first plugin
        }
        if (plugin_name.empty())
            throw(BackendException(lib_name, "library is not a VTApi backend plugin"));

        _pbackend = _ploader->create(plugin_name);

        // create connection object and connect to database
        _pconnection = _pbackend->createConnection(_pconfig->connection);

        if (!_pconnection->connect())
            throw DatabaseConnectionException(_pconfig->connection,_pconnection->getErrorMessage());
    }
    catch(Poco::Exception &e)
    {
        unloadBackend();
        throw BackendException(lib_name, e.message());
    }
    catch(...)
    {
        unloadBackend();
        throw;
    }
}

void Commons::unloadBackend()
{
    vt_destruct(_pconnection);
    vt_destruct(_pbackend);
    if (_ploader) {
        try
        {
            string lib_name = getBackendLibName();
            if (_ploader->isLibraryLoaded(lib_name))
                _ploader->unloadLibrary(lib_name);
        }
        catch(Exception &e) {}

        vt_destruct(_ploader);
    }

}

string Commons::getBackendLibName() const
{
    size_t uri_end = _pconfig->connection.find("://");
    if (uri_end != string::npos) {
        string backend_name = _pconfig->connection.substr(0, uri_end);
        return "libvtapi_" + backend_name + Poco::SharedLibrary::suffix();
    }
    else {
        throw BadConfigurationException("invalid \'connection\' format: " + _pconfig->connection);
    }
}

Commons::~Commons()
{
    if (_is_owner) {
        unloadBackend();
        vt_destruct(_pconfig);
    }
}

Commons::_CONFIG::_CONFIG()
{
    log_errors = false;
    log_warnings = false;
    log_debug = false;
}

Commons::_CONTEXT::_CONTEXT()
{
    process = 0;
}


const Commons::CONFIG& Commons::config() const
{
    return *_pconfig;
}

Commons::CONTEXT& Commons::context()
{
    return _context;
}


const IBackendInterface& Commons::backend() const
{
    return *_pbackend;
}


Connection& Commons::connection()
{
    return *_pconnection;
}

void Commons::loadConfig(const Poco::Util::AbstractConfiguration &config)
{
    try
    {
        // required properties

        if (config.hasProperty("datasets_dir"))
            _pconfig->datasets_dir = config.getString("datasets_dir");
        else
            throw BadConfigurationException("datasets_dir option missing");
        if (config.hasProperty("modules_dir"))
            _pconfig->modules_dir = config.getString("modules_dir");
        else
            throw BadConfigurationException("modules_dir option missing");
        if (config.hasProperty("connection"))
            _pconfig->connection = config.getString("connection");
        else
            throw BadConfigurationException("connection option missing");

        // optional properties

        if (config.hasProperty("config"))
            _pconfig->configfile = Poco::Path(config.getString("config")).makeAbsolute().toString();
        if (config.hasProperty("logfile"))
            _pconfig->logfile = Poco::Path(config.getString("logfile")).makeAbsolute().toString();
        _pconfig->log_errors = config.hasProperty("log_errors");
        _pconfig->log_warnings = config.hasProperty("log_warnings");
        _pconfig->log_debug = config.hasProperty("log_debug");

        // context properties

        if (config.hasProperty("dataset"))
            _context.dataset = config.getString("dataset");
        if (config.hasProperty("process"))
            _context.process = config.getInt("process");
    }
    catch (Poco::SyntaxException &e)
    {
        throw (BadConfigurationException(e.message()));
    }
}

void Commons::saveConfig(Poco::Util::AbstractConfiguration &config) const
{
    // required properties

    config.setString("datasets_dir", _pconfig->datasets_dir);
    config.setString("modules_dir", _pconfig->modules_dir);
    config.setString("connection", _pconfig->connection);

    // optional properties

    if (!_pconfig->configfile.empty())
        config.setString("config", _pconfig->configfile);
    if (!_pconfig->logfile.empty())
        config.setString("logfile", _pconfig->logfile);
    if (_pconfig->log_errors)
        config.setBool("log_errors", true);
    if (_pconfig->log_warnings)
        config.setBool("log_warnings", true);
    if (_pconfig->log_debug)
        config.setBool("log_debug", true);

    // context properties

    if (!_context.dataset.empty())
        config.setString("dataset", _context.dataset);
    if (_context.process != 0)
        config.setInt("process", _context.process);
}


}
