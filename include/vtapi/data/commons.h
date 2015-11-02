/**
 * @file
 * @brief   Declaration of Common class and definition of format_t
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include <Poco/Util/AbstractConfiguration.h>
#include <Poco/ClassLoader.h>
#include "../plugins/backend_interface.h"
#include <string>
#include <memory>

namespace vtapi {

/**
 * @brief Common base class to each %VTApi object
 *
 * Holds configuration, backend interface, connection and local context
 *
 * @see Basic information on page @ref KEYVALUES
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 *//***************************************************************************/
class Commons
{
public:
    /**
     * @brief Commons Construct shared configuration object from configuration map
     * @param config configuration object
     */
    Commons(const Poco::Util::AbstractConfiguration &config);

    /**
     * @brief Copy constructor
     * @param orig original object
     * @param new_copy do full copy including configuration and connection
     */
    Commons(const Commons &orig, bool new_copy);

    /**
     * Destructor
     */
    virtual ~Commons();

protected:
    /**
     * @brief Global configuration (shared between all objects)
     */
    class Config
    {
    public:
        std::string     configfile;         /**< VTApi config file path */
        std::string     datasets_dir;       /**< Folder with datasets */
        std::string     modules_dir;        /**< Folder with module binaries */
        std::string     connection;         /**< Connection string (URI://[string]) */
        bool            log_errors;         /**< Enables logging error messages */
        bool            log_warnings;       /**< Enables logging warning messages */
        bool            log_messages;       /**< Enables logging info messages */
        bool            log_queries;        /**< Enables logging SQL queries */
        std::string     logfile;            /**< Path to log file (empty => stdout/stderr) */

        Config() : log_errors(false), log_warnings(false), log_messages(false), log_queries(false) {}
    };

    /**
     * @brief Local context - each object owns its copy
     */
    class Context
    {
    public:
        std::string     dataset_location;   /**< Current dataset location */
        std::string     sequence_location;  /**< Current sequence location */
        std::string     dataset;            /**< Current dataset name */
        std::string     sequence;           /**< Current sequence name */
        std::string     selection;          /**< Current selection name */
        std::string     method;             /**< Current method name */
        int             process;            /**< Current process ID */
        std::string     task;               /**< Current task name */

        Context() : process(0) {}
    };

    Context _context;   /**< Local context for VTApi objects */

    /**
     * @brief configuration accessor
     * @return config
     */
    const Config &config() const
    { return *_pconfig; }

    /**
     * @brief backend interface accessor
     * @return backend interface
     */
    const IBackendInterface &backend() const
    { return *_pbackend; }

    /**
     * @brief connection accessor
     * @return connection
     */
    Connection &connection()
    { return *_pconnection; }

    /**
     * @brief Loads configuration into commons
     * @param config source configuration
     */
    void loadConfig(const Poco::Util::AbstractConfiguration &config);

    /**
     * @brief Saves configuration from commons
     * @param config destination configuration
     */
    void saveConfig(Poco::Util::AbstractConfiguration &config) const;

private:
    std::shared_ptr<Config> _pconfig;               /**< Global configuration */
    std::shared_ptr<IBackendInterface> _pbackend;   /**< Backend library interface */
    std::shared_ptr<Connection> _pconnection;       /**< Database connection object */
    std::shared_ptr< Poco::ClassLoader<IBackendInterface> > _ploader;  /** backend library loader/unloader */

    bool _is_owner;                                 /**< owns its resources */

    void loadBackend();
    void unloadBackend();
    std::string getBackendLibName() const;

    Commons() = delete;
    Commons(const Commons&) = delete;
    Commons& operator=(const Commons&) = delete;

    friend class Query;
    friend class VTApi;
};

} // namespace vtapi
