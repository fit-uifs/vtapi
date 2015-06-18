
#include <common/vtapi_global.h>
#include <backends/vtapi_backendbase.h>

#if VTAPI_HAVE_SQLITE

#define SL_LOAD(F) \
if ((sl.F = (SL_ ## F)lt_dlsym(hLibsqlite, #F)) == NULL) { \
    string warning = string("Function ") + #F + " not loaded.";\
    logger->warning(555, warning.c_str(), thisClass + "::base_load_libs()");\
    retval = false;\
    break;\
};

using namespace std;

namespace vtapi {


SLBackendBase::SLBackendBase(const SLBackendBase &base) :
    BackendBase(base.logger)
{
    thisClass = base.thisClass;
    hLibsqlite = NULL;
    memcpy(&this->sl, &base.sl, sizeof (this->sl));
}

SLBackendBase::SLBackendBase(Logger *logger) :
    BackendBase(logger) {
    thisClass = "SLBackendBase";
    hLibsqlite = NULL;
    memset(&sl, 0, sizeof (sl));
}

SLBackendBase::~SLBackendBase() {
}

bool SLBackendBase::base_load_libs() {
    bool retval = true;
    lt_dladvise ldadvise = NULL;

    do {
        // load libraries with global symbol resolving and possible other extensions
        lt_dladvise_init(&ldadvise);
        lt_dladvise_ext(&ldadvise);
        lt_dladvise_global(&ldadvise);

        // load sqlite library
        hLibsqlite = lt_dlopenadvise("libsqlite3", ldadvise);
        if (hLibsqlite) {
            logger->warning(556, "libsqlite3 library not found.", thisClass + "::base_load_libs()");
            retval = false;
            break;
        }

        // load sqlite functions
        SL_LOAD(sqlite3_open_v2);
        SL_LOAD(sqlite3_close);
        SL_LOAD(sqlite3_errmsg);
        SL_LOAD(sqlite3_db_status);
        SL_LOAD(sqlite3_get_table);
        SL_LOAD(sqlite3_db_filename);
        SL_LOAD(sqlite3_exec);
        SL_LOAD(sqlite3_free);
        SL_LOAD(sqlite3_free_table);
    } while (0);

    if (ldadvise) lt_dladvise_destroy(&ldadvise);

    return retval;
}

void SLBackendBase::base_unload_libs() {
    if (hLibsqlite) {
        lt_dlclose(hLibsqlite);
        hLibsqlite = NULL;
    }
    memset(&sl, 0, sizeof(sl));
}


}

#endif

