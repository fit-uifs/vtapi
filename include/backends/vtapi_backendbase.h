#pragma once

#include "../common/vtapi_config.h"
#include "vtapi_backendinterfaces.h"
#include "../common/vtapi_logger.h"

namespace vtapi {

class BackendBase;
class PGBackendBase;
class SLBackendBase;

class BackendBase
{
public:
    explicit BackendBase(Logger *logger)
    : logger(logger), m_libsLoaded(false) {}
    
    virtual ~BackendBase()
    {
        if (m_libsLoaded) {
            base_unload_libs();
            lt_dlexit();
        }
    }
    
    bool base_init()
    {
        return (m_libsLoaded = (lt_dlinit() == 0) && base_load_libs());
    }
    
    bool base_is_valid()
    {
        return m_libsLoaded;
    }
    
protected:
    virtual bool base_load_libs()
    { return true; };
    
    virtual void base_unload_libs() {};
    
protected:
    std::string thisClass;  /**< class name */
    Logger *logger;         /**< logger object for output messaging */
    
private:
    bool m_libsLoaded;
};

#if HAVE_POSTGRESQL

class PGBackendBase : public BackendBase
{
public:
    explicit PGBackendBase(const PGBackendBase &base);
    explicit PGBackendBase(Logger *logger);
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
    explicit SLBackendBase(const SLBackendBase &base);
    explicit SLBackendBase(Logger *logger);
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
