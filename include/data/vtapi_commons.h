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
#include "../common/vtapi_config.h"
#include "../common/vtapi_settings.h"
#include "../backends/vtapi_connection.h"
#include "../backends/vtapi_backendfactory.h"

namespace vtapi {

/**
 * @brief This is common predecessor to each object in the %VTApi - manages connection, logging and error handling.
 *
 * This class is inherited by many and many other classes, but it manages just single resources,
 * thus there may bee a doom if someone destroys the original ones. Well, destructor should only happen
 * when isDoom is false, which is set by the only constructor: Commons(std::string connStr); .
 *
 * @see Basic information on page @ref KEYVALUES
 *
 * @note Error codes 15*
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
    // global configuration (shared by pointer between all objects)
    typedef struct _CONFIG
    {
        std::string     configfile;         /**< VTApi config file */
        BackendFactory::BACKEND_T backend;  /**< Used backend type */
        std::string     connection;         /**< Connection string */
        std::string     dbfolder;           /**< Databases folder */
        std::string     input;              /**< A filename from which we read an input */
        std::string     output;             /**< A filename into which we write an output */
        std::string     baseLocation;       /**< Base path location */
        int             queryLimit;         /**< Limit number of rows fetched at once */
        int             arrayLimit;         /**< Limit amount of printed array elements */
        std::string     logfile;            /**< Log file */
        bool            verbose;            /**< Verbose mode */
        bool            debug;              /**< Debug mode */

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

    CONFIG          *_config;       /**< Global configuration */
    CONTEXT         _context;       /**< Local context for VTApi objects */
    BackendBase     *_backendBase;  /**< base backend object, will be copied across backend-specific objects */
    Connection      *_connection;   /**< Connection object */

    /**
     * Recommended constructor. All arguments are loaded from gengetopt structure.
     * @param args_info   gengetopt structure of command line/configuration file arguments
     */
    explicit Commons(const gengetopt_args_info& args_info);
    
    /**
     * Copy constructor
     * @param orig orignal object
     * @param new_copy full copy including configuration
     */
    Commons(const Commons& orig, bool new_copy);
    
    /**
     * Destructor
     */
    virtual ~Commons();

    /**
     * Initialize
     * @param args_info
     * @return 
     */
    bool init(const gengetopt_args_info& args_info);


    /**
     * This is to check whether a file exists or not
     * @param filepath file path
     * @return success exists
     */
    static bool fileExists(const std::string& filepath);

    /**
     * This is to check whether a directory exists or not
     * @param dirpath directory path
     * @return success exists
     */
    static bool dirExists(const std::string& dirpath);

private:
    bool _is_owner; /**< owns its resources */
    
    Commons() = delete;
    Commons(const Commons&) = delete;
    Commons& operator=(const Commons&) = delete;

};

} // namespace vtapi
