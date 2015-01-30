/* 
 * File:   vtapi_libloader.h
 * Author: vojca
 *
 * Created on May 4, 2013, 10:34 PM
 */

#ifndef VTAPI_LIBLOADER_H
#define	VTAPI_LIBLOADER_H

#include "vtapi_backendlibs.h"
#include "../common/vtapi_logger.h"

namespace vtapi {


/**
 * @brief Class encapsulates runtime loading and unloading of dynamic libraries
 *
 * All functions loaded into function map using loadLibs() can be accessed via
 * CALL_PQ, CALL_PQT and CALL_SL macros.
 */
class LibLoader {
protected:
    Logger          *logger;        /**< logger object for output messaging */
    std::string     thisClass;      /**< class name */

public:

    /**
     * Constructor
     * @param logger logger object
     */
    LibLoader(Logger *logger) {
        this->logger = logger;
    };
    /**
     * Virtual destructor
     */
    virtual ~LibLoader() {};

    /**
     * Loads dynamic libraries and addresses of some of their functions into
     * the function address map indexed by function name
     * @return function address map
     */
    virtual fmap_t *loadLibs() = 0;
    /**
     * Unloads all dynamic libraries
     */
    virtual void unloadLibs() = 0;
    /**
     * Checks if dynamic libraries are currently loaded
     * @return success
     */
    virtual bool isLoaded() = 0;
    /**
     * Gets library name
     * @return library name
     */
    virtual char *getLibName() = 0;
};

#if HAVE_POSTGRESQL
class PGLibLoader : public LibLoader {
private:

    lt_dlhandle     h_libpqtypes;
    lt_dlhandle     h_libpq;
    
public:

    PGLibLoader(Logger *logger);
    ~PGLibLoader();

    fmap_t *loadLibs();
    void unloadLibs();
    bool isLoaded();
    char *getLibName();

private:

    bool load_libpqtypes(fmap_t *);
    bool load_libpq(fmap_t *);
    void unload_libpqtypes();
    void unload_libpq();

};
#endif

#if HAVE_SQLITE
class SLLibLoader : public LibLoader {
private:

    lt_dlhandle h_libsqlite;

public:

    SLLibLoader(Logger *logger);
    ~SLLibLoader();

    fmap_t *loadLibs();
    void unloadLibs();
    bool isLoaded();
    char *getLibName();
    
private:

    bool load_libsqlite(fmap_t *);
    void unload_libsqlite();
};
#endif

} // namespace vtapi

#endif	

