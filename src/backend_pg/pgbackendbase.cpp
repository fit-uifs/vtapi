
#include <common/vtapi_global.h>
#include <backends/vtapi_backendbase.h>


#if HAVE_POSTGRESQL

#define PG_LOAD(F) \
if ((pg.F = (PQ_ ## F)lt_dlsym(hLibpq, #F)) == NULL) { \
    string warning = string("Function ") + #F + " not loaded.";\
    logger->warning(555, warning.c_str(), thisClass + "::base_load_libs()");\
    retval = false;\
    break;\
};

#define PQT_LOAD(F) \
if ((pqt.F = (PQT_ ## F)lt_dlsym(hLibpqtypes, #F)) == NULL) { \
    string warning = string("Function ") + #F + " not loaded.";\
    logger->warning(555, warning.c_str(), thisClass + "::base_load_libs()");\
    retval = false;\
    break;\
}

using namespace std;

namespace vtapi {


PGBackendBase::PGBackendBase(const PGBackendBase &base) :
    BackendBase(base.logger)
{
    thisClass = base.thisClass;
    hLibpq = NULL;
    hLibpqtypes = NULL;
    memcpy(&this->pg, &base.pg, sizeof (this->pg));
    memcpy(&this->pqt, &base.pqt, sizeof (this->pqt));
}

PGBackendBase::PGBackendBase(Logger *logger) :
    BackendBase(logger)
{
    thisClass = "PGBackendBase";
    hLibpq = NULL;
    hLibpqtypes = NULL;
    memset(&pg, 0, sizeof(pg));
    memset(&pqt, 0, sizeof(pqt));
}

PGBackendBase::~PGBackendBase()
{
}

bool PGBackendBase::base_load_libs() {
    bool retval = true;
    lt_dladvise ldadvise = NULL;

    do {       
        // load libraries with global symbol resolving and possible other extensions
        lt_dladvise_init(&ldadvise);
        lt_dladvise_ext(&ldadvise);
        lt_dladvise_global(&ldadvise);

        // load libpq
        hLibpq = lt_dlopenadvise(PG_LIB_PATH "/libpq", ldadvise);
        if (!hLibpq) {
            logger->warning(556, "libpq library not found.", thisClass + "::base_load_libs()");
            retval = false;
            break;
        }
        
        // load libpqtypes
        hLibpqtypes = lt_dlopenadvise("libpqtypes", ldadvise);
        if (!hLibpqtypes) {
            logger->warning(556, "libpqtypes library not found.", thisClass + "::base_load_libs()");
            retval = false;
            break;
        }
        
        // load libpqtypes functions
        PQT_LOAD(PQgeterror);
        PQT_LOAD(PQgeterror);
        PQT_LOAD(PQseterror);
        PQT_LOAD(PQgetErrorField);
        PQT_LOAD(PQspecPrepare);
        PQT_LOAD(PQclearSpecs);
        PQT_LOAD(PQinitTypes);
        PQT_LOAD(PQregisterSubClasses);
        PQT_LOAD(PQregisterComposites);
        PQT_LOAD(PQregisterUserDefinedTypes);
        PQT_LOAD(PQregisterTypes);
        PQT_LOAD(PQregisterResult);
        PQT_LOAD(PQclearTypes);
        PQT_LOAD(PQparamCreate);
        PQT_LOAD(PQparamDup);
        PQT_LOAD(PQparamCount);
        PQT_LOAD(PQparamReset);
        PQT_LOAD(PQparamClear);
        PQT_LOAD(PQputf);
        PQT_LOAD(PQputvf);
        PQT_LOAD(PQgetf);
        PQT_LOAD(PQgetvf);
        PQT_LOAD(PQexecf);
        PQT_LOAD(PQexecvf);
        PQT_LOAD(PQsendf);
        PQT_LOAD(PQsendvf);
        PQT_LOAD(PQparamExec);
        PQT_LOAD(PQparamSendQuery);
        PQT_LOAD(PQparamExecPrepared);
        PQT_LOAD(PQparamSendQueryPrepared);
        PQT_LOAD(PQlocalTZInfo);

        // load libpq functions
        PG_LOAD(PQconnectdb);
        PG_LOAD(PQstatus);
        PG_LOAD(PQerrorMessage);
        PG_LOAD(PQfinish);
        PG_LOAD(PQclear);
        PG_LOAD(PQresultAlloc);
        PG_LOAD(PQfreemem);
        PG_LOAD(PQresultStatus);
        PG_LOAD(PQntuples);
        PG_LOAD(PQcmdStatus);
        PG_LOAD(PQcmdTuples);
        PG_LOAD(PQfname);
        PG_LOAD(PQfnumber);
        PG_LOAD(PQftype);
        PG_LOAD(PQnfields);
        PG_LOAD(PQgetvalue);
        PG_LOAD(PQgetisnull);
        PG_LOAD(PQgetlength);
        PG_LOAD(PQescapeLiteral);
        PG_LOAD(PQescapeIdentifier);
    } while(0);
    
    if (ldadvise) lt_dladvise_destroy(&ldadvise);
    
    return retval;
}

void PGBackendBase::base_unload_libs()
{
    if (hLibpqtypes) {
        lt_dlclose(hLibpqtypes);
        hLibpqtypes = NULL;
    }
    if (hLibpq) {
        lt_dlclose(hLibpq);
        hLibpq = NULL;
    }
    memset(&pg, 0, sizeof (pg));
    memset(&pqt, 0, sizeof (pqt));
}

#endif

}