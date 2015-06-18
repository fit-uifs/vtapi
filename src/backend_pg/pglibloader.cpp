
#include <common/vtapi_global.h>
#include <common/vtapi_misc.h>
#include <common/vtapi_serialize.h>
#include <backends/vtapi_libloader.h>

#if VTAPI_HAVE_POSTGRESQL

// postgres data transfer format: 0=text, 1=binary
#define PG_FORMAT           1

using std::string;
using std::stringstream;
using std::vector;
using std::pair;

using namespace vtapi;

PGLibLoader::PGLibLoader(Logger *logger)
 : LibLoader (logger) {
    thisClass = "PGLibLoader";
    h_libpqtypes = NULL;
    h_libpq = NULL;
    lt_dlinit();
}

PGLibLoader::~PGLibLoader() {
    unloadLibs();
    lt_dlexit();
};

fmap_t *PGLibLoader::loadLibs() {
    bool    retval  = VT_OK;
    fmap_t  *fmap   = new fmap_t();

    retval &= load_libpq(fmap);
    retval &= load_libpqtypes(fmap);
    if (!retval) vt_destruct(fmap);

    return retval ? fmap : NULL;
};

void PGLibLoader::unloadLibs() {
    unload_libpqtypes();
    unload_libpq();
};

bool PGLibLoader:: isLoaded() {
    return h_libpq && h_libpqtypes ? VT_OK : VT_FAIL;
}

char *PGLibLoader::getLibName() {
    char *name = NULL;
    const lt_dlinfo *libpq_info;

    if (h_libpq) {
        libpq_info = lt_dlgetinfo(h_libpq);
        name = libpq_info->name;
    }
    
    return name;
}

bool PGLibLoader::load_libpqtypes (fmap_t *fmap) {
    bool retval     = VT_OK;
    void *funcPtr   = NULL;
    const string funcStrings[] =
    {"PQgeterror","PQseterror","PQgetErrorField","PQspecPrepare","PQclearSpecs","PQinitTypes",
     "PQregisterSubClasses","PQregisterComposites","PQregisterUserDefinedTypes","PQregisterTypes",
     "PQregisterResult","PQclearTypes","PQparamCreate","PQparamDup","PQparamCount","PQparamReset",
     "PQparamClear","PQputf","PQputvf","PQgetf","PQgetvf","PQexecf","PQexecvf","PQsendf","PQsendvf",
     "PQparamExec","PQparamSendQuery","PQparamExecPrepared","PQparamSendQueryPrepared","PQlocalTZInfo",
     ""
    };

    h_libpqtypes = lt_dlopenext("libpqtypes");

    if (h_libpqtypes) {
        for(int i = 0; !funcStrings[i].empty(); i++) {
            funcPtr = lt_dlsym(h_libpqtypes, funcStrings[i].c_str());
            if (funcPtr) {
                fmap->insert(FMAP_ENTRY("PQT_"+funcStrings[i], funcPtr));
            }
            else {
                logger->warning(555, "Function " + funcStrings[i] + " not loaded.", thisClass+"::load_libpqtypes()");
                retval = VT_FAIL;
                break;
            }
        }
    }
    else {
        logger->warning(556, "Libpqtypes library not found.", thisClass+"::load_libpqtypes()");
        retval = VT_FAIL;
    }

    return retval;
}

bool PGLibLoader::load_libpq (fmap_t *fmap) {
    bool retval     = VT_OK;
    void *funcPtr   = NULL;
    lt_dladvise libpq_advise = NULL;
    const string funcStrings[] =
    {"PQconnectdb","PQstatus","PQerrorMessage","PQfinish","PQclear","PQresultAlloc","PQfreemem","PQresultStatus","PQntuples", "PQcmdStatus",
     "PQcmdTuples","PQfname","PQfnumber","PQftype","PQnfields","PQgetvalue","PQgetisnull","PQgetlength","PQescapeLiteral","PQescapeIdentifier",
    ""
    };

    lt_dladvise_init (&libpq_advise);
    lt_dladvise_ext(&libpq_advise);
    lt_dladvise_global(&libpq_advise);
    h_libpq = lt_dlopenadvise(PG_LIB_PATH "/libpq", libpq_advise);
    lt_dladvise_destroy(&libpq_advise);

    if (h_libpq) {
        for(int i = 0; !funcStrings[i].empty(); i++) {
            funcPtr = lt_dlsym(h_libpq, funcStrings[i].c_str());
            if (funcPtr) {
                fmap->insert(FMAP_ENTRY("PQ_"+funcStrings[i], funcPtr));
            }
            else {
                logger->warning(555, "Function " + funcStrings[i] + " not loaded.", thisClass+"::load_libpq()");
                retval = VT_FAIL;
                break;
            }
        }
    }
    else {
        logger->warning(556, "Libpq library not found.", thisClass+"::load_libpq()");
        retval = VT_FAIL;
    }
    return retval;
}

void PGLibLoader::unload_libpqtypes () {
    if (h_libpqtypes) {
        lt_dlclose(h_libpqtypes);
        h_libpqtypes = NULL;
    }
}

void PGLibLoader::unload_libpq () {
    if (h_libpq) {
        lt_dlclose(h_libpq);
        h_libpq = NULL;
    }
}

#endif