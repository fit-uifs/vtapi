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

#include <string>
#include <Poco/Util/AbstractConfiguration.h>
#include <Poco/ClassLoader.h>
#include "../plugins/backend_interface.h"

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
    Commons(const Commons& orig, bool new_copy);

    /**
     * Destructor
     */
    virtual ~Commons();

protected:
    // global configuration (shared by pointer between all objects)
    typedef struct _CONFIG
    {
        std::string     configfile;         /**< VTApi config file path */
        std::string     datasets_dir;       /**< Folder with datasets */
        std::string     modules_dir;        /**< Folder with module binaries */
        std::string     connection;         /**< Connection string (URI://[string]) */
        bool            log_errors;         /**< Enables logging error messages */
        bool            log_warnings;       /**< Enables logging warning messages */
        bool            log_debug;          /**< Enables logging debug messages */
        std::string     logfile;            /**< Path to log file (empty => stdout/stderr) */

        _CONFIG();
    } CONFIG;

    // local context (each object owns its copy)
    typedef struct _CONTEXT
    {
        std::string     datasetLocation;    /**< Current dataset location */
        std::string     sequenceLocation;   /**< Current sequence location */
        std::string     dataset;            /**< Current dataset name */
        std::string     sequence;           /**< Current sequence name */
        std::string     selection;          /**< Current selection name */
        std::string     method;             /**< Current method name */
        int             process;            /**< Current process ID */
        std::string     task;               /**< Current task name */

        _CONTEXT();
    } CONTEXT;

    /**
     * @brief configuration accessor
     * @return config
     */
    const CONFIG& config();

    /**
     * @brief local context accessor
     * @return context
     */
    CONTEXT& context();

    /**
     * @brief backend interface accessor
     * @return backend interface
     */
    const IBackendInterface& backend();

    /**
     * @brief connection accessor
     * @return connection
     */
    Connection& connection();

    /**
     * @brief Loads configuration into commons
     * @param config source configuration
     */
    void loadConfig(const Poco::Util::AbstractConfiguration &config);

    /**
     * @brief Saves configuration from commons
     * @param config destination configuration
     */
    void saveConfig(Poco::Util::AbstractConfiguration &config);

private:
    CONFIG          *_pconfig;              /**< Global configuration */
    CONTEXT         _context;               /**< Local context for VTApi objects */
    IBackendInterface *_pbackend;           /**< Backend library interface */
    Connection      *_pconnection;          /**< Database connection object */

    bool _is_owner;                         /**< owns its resources */
    Poco::ClassLoader<IBackendInterface> *_ploader;   /** backend library loader/unloader */

    void loadBackend();
    void unloadBackend();
    std::string getBackendLibName();

    Commons() = delete;
    Commons(const Commons&) = delete;
    Commons& operator=(const Commons&) = delete;

};

} // namespace vtapi
