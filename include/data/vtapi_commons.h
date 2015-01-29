/* 
 * File:   vtapi_commons.h
 * Author: vojca
 *
 * Created on May 7, 2013, 1:03 PM
 */

#ifndef VTAPI_COMMONS_H
#define	VTAPI_COMMONS_H

#include "backends/vtapi_backendfactory.h"
#include "backends/vtapi_backends.h"

namespace vtapi {

    
typedef enum {
    STANDARD = 0,
    CSV,
    HTML
}  format_t;


/**
 * @brief This is common predecessor to each object in the VTApi - manages connection, logging and error handling.
 *
 * This class is inherited by many and many other classes, but it manages just single resources,
 * thus there may bee a doom if someone destroys the original ones. Well, destructor should only happen
 * when isDoom is false, which is set by the only constructor: Commons(std::string connStr); .
 *
 * @see Basic information on page @ref KEYVALUES
 *
 * @note Error codes 15*
 *//***************************************************************************/
class Commons {
protected:
    std::string thisClass; /**< This should be filled in each constructor of a derived class */

    Connection      *connection;        /**< Connection object */
    TypeManager     *typeManager;       /**< Datatypes management object */
    LibLoader       *libLoader;         /**< Library loader object */
    Logger          *logger;            /**< Logger object */
    fmap_t          *fmap;              /**< Map of library functions */

    backend_t       backend;            /**< Used backend type */
    std::string     dbconn;             /**< Connection string / databases folder */
    format_t        format;             /**< Output format */
    std::string     input;              /**< A filename from which we read an input */
    std::string     output;             /**< A filename into which we write an output */
    std::string     user;               /**< Current user */
    std::string     baseLocation;       /**< Base path location */
    int             queryLimit;         /**< Limit number of rows fetched at once */
    int             arrayLimit;         /**< Limit amount of printed array elements */

    // THESE ARE USED IN THE UNDERLYING CLASSES HIERARCHY FOR ANYTHING NECESSARY
    std::string     dataset;            /**< Current dataset name */
    std::string     datasetLocation;    /**< Current dataset location */
    std::string     sequence;           /**< Current sequence name */
    std::string     sequenceLocation;   /**< Current sequence location */
    std::string     selection;          /**< Current selection name */

    // NOT YET IMPLEMENTED
    std::string     method;             /**< Current method name */
    std::string     process;            /**< Current process name */

    bool            doom;               /**< For destructor purposes */

public:
    /**
     * Full copy constructor from existing Commons class
     * @note The default constructor Commons() should never exist
     * @param orig previously created Commons class
     */
    Commons(const Commons& orig);
    /**
     * Recommended constructor. All arguments are loaded from gengetopt structure.
     * @note The default constructor Commons() should never exist
     * @param args_info gengetopt structure of command line/configuration file arguments
     */
    Commons(const gengetopt_args_info& args_info);
    /**
     * Destructor
     */
    ~Commons();
    /**
     *
     * @param format
     * @unimplemented
     */
    void read(const std::string& format="");

    // some functions that may be useful
    /**
     * Get current connector
     * @return connector
     */
    Connection* getConnection();
    /**
     * Get current logger
     * @return logger
     */
    Logger* getLogger();

    /**
     * Get name of current dataset
     * @return dataset name
     */
    std::string getDataset();
    /**
     * Get name of current sequence
     * @return sequence name
     */
    std::string getSequence();
    /**
     * Get name of current selection
     * @return selection name
     */
    std::string getSelection();
    /**
     * Get base location path
     * @return base location
     */
    std::string getBaseLocation();
    /**
     * Get dataset location path
     * @return dataset location
     */
    std::string getDatasetLocation();
    /**
     * Get sequence location path
     * @return sequence location
     */
    std::string getSequenceLocation();
    /**
     * Get data location path
     * @return data location
     */
    std::string getDataLocation();
    /**
     * Get user name
     * @return user name
     */
    std::string getUser();

    /**
     * Checks commons object for validity (connection etc.)
     * @return success
     */
    bool checkCommonsObject();
    /**
     * This is to check whether a file exists or not
     * @param filename
     * @return success
     */
    static bool fileExists(const std::string& filename);

private:

    /**
     * Maps string to format_t
     * @param format string format
     * @return format_t value
     */
    format_t mapFormat(const std::string& format);

};

} // namespace vtapi

#endif	/* VTAPI_COMMONS_H */

