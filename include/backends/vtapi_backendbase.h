#pragma once

#include "vtapi_backendinterfaces.h"
#include "../common/vtapi_logger.h"

namespace vtapi {


class BackendBase
{
public:
    BackendBase(Logger *logger)
    {
        this->logger = logger;
        bLibsLoaded = false;
    }
    virtual ~BackendBase()
    {
        if (bLibsLoaded) {
            base_unload_libs();
            lt_dlexit();
        }
    }
    
    bool base_init()
    {
        return (bLibsLoaded = (lt_dlinit() == 0) && base_load_libs());
    }
    bool base_is_valid()
    {
        return bLibsLoaded;
    }
    
protected:
    virtual bool base_load_libs() { return true; };
    virtual void base_unload_libs() {};
    
protected:
    std::string thisClass;  /**< class name */
    Logger *logger;         /**< logger object for output messaging */
    
private:
    bool bLibsLoaded;
};

#if HAVE_POSTGRESQL

class PGBackendBase : public BackendBase
{
public:
    PGBackendBase(const PGBackendBase &base);
    PGBackendBase(Logger *logger);
    virtual ~PGBackendBase();

protected:
    bool base_load_libs();
    void base_unload_libs();
    
protected:
    POSTGRES_INTERFACE pg;
    PQTYPES_INTERFACE pqt;
    
private:
    lt_dlhandle hLibpqtypes;
    lt_dlhandle hLibpq;
};

#endif

#if HAVE_SQLITE

typedef struct {
    std::string database;
} SLparam;

typedef struct {
    int rows;
    int cols;
    char **res;
} SLres;

class SLBackendBase : public BackendBase {
public:
    SLBackendBase(const SLBackendBase &base);
    SLBackendBase(Logger *logger);
    virtual ~SLBackendBase();

    bool base_init();

protected:
    bool base_load_libs();
    void base_unload_libs();

protected:
    SQLITE_INTERFACE sl;

private:
    lt_dlhandle hLibsqlite;

};

#endif

} // namespace
