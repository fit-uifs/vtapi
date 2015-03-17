
#include <common/vtapi_global.h>
#include <common/vtapi_misc.h>
#include <common/vtapi_serialize.h>
#include <backends/vtapi_libloader.h>

#if HAVE_SQLITE

using std::string;
using std::stringstream;
using std::vector;
using std::pair;

using namespace vtapi;

SLLibLoader::SLLibLoader(Logger *logger)
 : LibLoader (logger) {
    thisClass = "SLLibLoader";
    h_libsqlite = NULL;
    lt_dlinit();
}

SLLibLoader::~SLLibLoader() {
    unloadLibs();
    lt_dlexit();
};

fmap_t *SLLibLoader::loadLibs() {
    fmap_t *fmap = new fmap_t();

    if (load_libsqlite(fmap)) {
        return fmap;
    }
    else {
        vt_destruct(fmap);
        return NULL;
    }
};

void SLLibLoader::unloadLibs() {
    unload_libsqlite();
};

bool SLLibLoader:: isLoaded() {
    return h_libsqlite ? VT_OK : VT_FAIL;
}

char *SLLibLoader::getLibName() {
    char *name = NULL;
    const lt_dlinfo *libsl_info;

    if (h_libsqlite) {
        libsl_info = lt_dlgetinfo(h_libsqlite);
        name = libsl_info->name;
    }

    return name;
}

bool SLLibLoader::load_libsqlite (fmap_t *fmap) {
    bool retval     = VT_OK;
    void *funcPtr   = NULL;
    const string funcStrings[] =
    {"sqlite3_open_v2","sqlite3_close","sqlite3_errmsg","sqlite3_db_status","sqlite3_get_table",
     "sqlite3_db_filename","sqlite3_exec","sqlite3_free","sqlite3_free_table",
     ""
    };

    h_libsqlite = lt_dlopenext("libsqlite3");

    if (h_libsqlite) {
        for(int i = 0; !funcStrings[i].empty(); i++) {
            funcPtr = lt_dlsym(h_libsqlite, funcStrings[i].c_str());
            if (funcPtr) {
                fmap->insert(FMAP_ENTRY("SL_"+funcStrings[i], funcPtr));
            }
            else {
                logger->warning(555, "Function " + funcStrings[i] + " not loaded.", thisClass+"::load_libsqlite()");
                retval = VT_FAIL;
                break;
            }
        }
    }
    else {
        logger->warning(556, "Libsqlite library not found.", thisClass+"::load_libsl()");
        retval = VT_FAIL;
    }

    return retval;
}

void SLLibLoader::unload_libsqlite () {
    if (h_libsqlite) {
        lt_dlclose(h_libsqlite);
        h_libsqlite = NULL;
    }
}

#endif