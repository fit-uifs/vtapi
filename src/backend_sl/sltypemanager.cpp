
#include <common/vtapi_global.h>
#include <common/vtapi_misc.h>
#include <common/vtapi_serialize.h>
#include <backends/vtapi_typemanager.h>


#if VTAPI_HAVE_SQLITE

using std::string;
using std::stringstream;
using std::vector;
using std::pair;

using namespace vtapi;

SLTypeManager::SLTypeManager(fmap_t *fmap, Connection *connection, Logger *logger, string& schema)
: TypeManager(fmap, connection, logger, schema) {
    thisClass = "SLTypeManager";
}

SLTypeManager::~SLTypeManager() {
}




#endif