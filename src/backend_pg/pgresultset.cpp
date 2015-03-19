
#include <common/vtapi_global.h>
#include <common/vtapi_serialize.h>
#include <backends/vtapi_resultset.h>

#if HAVE_POSTGRESQL

using std::string;
using std::ostringstream;
using std::vector;
using std::pair;

using namespace vtapi;

#define PGRES ((PGresult *)this->res)


PGResultSet::PGResultSet(const PGBackendBase &base, DBTYPES_MAP *dbtypes) :
    ResultSet(dbtypes),
    PGBackendBase(base)
{
    thisClass = "PGResultSet";
}

PGResultSet::~PGResultSet()
{
    clear();
}

void PGResultSet::newResult(void *res)
{
    clear();
    this->res = res;
}

int PGResultSet::countRows()
{
    return PGRES ? pg.PQntuples(PGRES) : -1;
}
int PGResultSet::countCols()
{
    return PGRES ? pg.PQnfields(PGRES) : -1;
}
bool PGResultSet::isOk()
{
    return PGRES ? VT_OK : VT_FAIL;
}
void PGResultSet::clear()
{
    if (PGRES) {
        pg.PQclear(PGRES);
        this->res = NULL;
    }
}

TKey PGResultSet::getKey(int col)
{
    string name = pg.PQfname(PGRES, col);
    string type = getKeyType(col);

    return TKey(type, name, 0);
}

TKeys* PGResultSet::getKeys()
{
    TKeys* keys     = new TKeys;

    int cols = pg.PQnfields(PGRES);
    for (int col = 0; col < cols; col++) {
        keys->push_back(getKey(col));
    }
    return keys;
}

string PGResultSet::getKeyType(const int col)
{
    string type;
    if (this->dbtypes) {
        DBTYPES_MAP::iterator it = this->dbtypes->find(pg.PQftype(PGRES, col));
        if (it != this->dbtypes->end()) {
            type = (*it).second.name;
        }
    }
    
    return type;
}

short PGResultSet::getKeyTypeLength(const int col, const short def)
{
    short length = def;
    if (this->dbtypes) {
        DBTYPES_MAP_IT it = this->dbtypes->find(pg.PQftype(PGRES, col));
        if (it != this->dbtypes->end()) {
            length = DBTYPE_GETLENGTH((*it).second.type);
        }
    }
    
    return length;
}

int PGResultSet::getKeyIndex(const string& key)
{
    return PGRES ? pg.PQfnumber(PGRES, key.c_str()) : -1;
}

// =============== SINGLE VALUES / ARRAYS / VECTORS TEMPLATES ===================
    template<typename TDB, typename TOUT>
TOUT PGResultSet::getSingleValue(const int col, const char *def)
{
    TDB value = {0};
    if (!pqt.PQgetf(PGRES, this->pos, def, col, &value)) {
        logger->warning(304, string("Value is not a ") + def, thisClass + "::getSingleValue()");
    }

    return (TOUT)value;
}

template<typename TDB, typename TOUT>
TOUT *PGResultSet::getArray(const int col, int& size, const char *def)
{
    TOUT *values = NULL;
    PGarray tmp = {0};

    do {
        char defArr[128];
        sprintf(defArr, "%s[]", def);
        if (!pqt.PQgetf(PGRES, this->pos, defArr, col, &tmp)) {
            logger->warning(304, string("Value is not an array of ") + def, thisClass + "::getArray()");
            break;
        }

        size = pg.PQntuples(tmp.res);
        if (size == 0) break;
        
        values = new TOUT [size];
        if (!values) break;

        for (int i = 0; i < size; i++) {
            TDB value = {0};
            if (!pqt.PQgetf(tmp.res, i, def, 0, &value)) {
                logger->warning(304, string("Unexpected value in array of ") + def, thisClass + "::getArray()");
                vt_destructall(values);
                break;
            }
            else {
                values[i] = (TOUT)value;
            }
        }
    } while(0);

    if (!values) size = 0;
    if (tmp.res) pg.PQclear(tmp.res);

    return values;
}

template<typename TDB, typename TOUT>
vector<TOUT> *PGResultSet::getVector(const int col, const char *def)
{
    vector<TOUT> *values = NULL;
    PGarray tmp = {0};

    do {
        char defArr[128];
        sprintf(defArr, "%s[]", def);
        if (!pqt.PQgetf(PGRES, this->pos, defArr, col, &tmp)) {
            logger->warning(304, string("Value is not an array of ") + def, thisClass + "::getVector()");
            break;
        }

        int size = pg.PQntuples(tmp.res);
        if (size == 0) break;

        values = new vector<TOUT>;
        if (!values) break;

        for (int i = 0; i < size; i++) {
            TDB value = {0};
            if (!pqt.PQgetf(tmp.res, i, def, 0, &value)) {
                logger->warning(304, string("Unexpected value in array of ") + def, thisClass + "::getVector()");
                vt_destruct(values);
                break;
            } else {
                values->push_back((TOUT)value);
            }
        }
    } while (0);

    if (tmp.res) pg.PQclear(tmp.res);

    return values;
}

// =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===================
char PGResultSet::getChar(const int col)
{
    return getSingleValue<PGchar,char>(col, "%char");
}

string PGResultSet::getString(const int col)
{
    // no conversions with libpqtypes, just get the value
    char *value = pg.PQgetvalue(PGRES, this->pos, col);
    return value ? value : "";
}

// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================

bool PGResultSet::getBool(const int col)
{
    return getSingleValue<PGbool,bool>(col, "%bool");
}

int PGResultSet::getInt(const int col)
{
    return (int)getInt8(col);
}

long long PGResultSet::getInt8(const int col)
{
    long long value = 0;

    switch(getKeyTypeLength(col, 4))
    {
        case -1:    // numeric
        {
            PGnumeric val = getSingleValue<PGnumeric,PGnumeric>(col, "%numeric");
            if (val) value = atoi(val);
            break;
        }
        case 2:
        {
            value = getSingleValue<PGint2, long long>(col, "%int2");
            break;
        }
        case 4:
        {
            value = getSingleValue<PGint4, long long>(col, "%int4");
            break;
        }
        case 8:
        {
            value = getSingleValue<PGint8, long long>(col, "%int8");
            break;
        }
        default:
        {
            logger->warning(306, "Value has unknown length", thisClass + "::getInt()");
            break;
        }
    }
    
    return value;
}

int* PGResultSet::getIntA(const int col, int& size)
{
    int *values = NULL;
    
    switch(getKeyTypeLength(col, 4))
    {
        case -1:    // numeric
        {
            PGnumeric *vals = getArray<PGnumeric,PGnumeric>(col, size, "%numeric");
            if (vals) {
                if (!(values = new int[size])) break;
                for (int i = 0; i < size; i++) {
                    values[i] = atoi(vals[i]);
                }
                vt_destructall(vals);
            }
            break;
        }
        case 2:
        {
            values = getArray<PGint2,int>(col, size, "%int2");
            break;
        }
        case 4:
        {
            values = getArray<PGint4,int>(col, size, "%int4");
            break;
        }
        case 8:
        {
            values = getArray<PGint8, int>(col, size, "%int8");
            break;
        }
        default:
        {
            logger->warning(306, "Value has unknown length", thisClass + "::getIntA()");
            break;
        }
    }
    
    return values;
}

vector<int>* PGResultSet::getIntV(const int col)
{
    vector<int> *values = NULL;
    
    switch(getKeyTypeLength(col, 4))
    {
        case -1:    // numeric
        {
            int size = 0;
            PGnumeric *vals = getArray<PGnumeric,PGnumeric>(col, size, "%numeric");
            if (vals) {
                if (!(values = new vector<int>)) break;
                for (int i = 0; i < size; i++) {
                    values->push_back(atoi(vals[i]));
                }
                vt_destructall(vals);
            }
            break;
        }
        case 2:
        {
            values = getVector<PGint2,int>(col, "%int2");
            break;
        }
        case 4:
        {
            values = getVector<PGint4,int>(col, "%int4");
            break;
        }
        case 8:
        {
            values = getVector<PGint8, int>(col, "%int8");
            break;
        }
        default:
        {
            logger->warning(306, "Value has unknown length", thisClass + "::getIntV()");
            break;
        }
    }
    
    return values;
}

long long* PGResultSet::getInt8A(const int col, int& size) {
    long long *values = NULL;

    switch (getKeyTypeLength(col, 4)) {
        case -1: // numeric
        {
            PGnumeric *vals = getArray<PGnumeric, PGnumeric>(col, size, "%numeric");
            if (vals) {
                if (!(values = new long long[size])) break;
                for (int i = 0; i < size; i++) {
                    values[i] = atoll(vals[i]);
                }
                vt_destructall(vals);
            }
            break;
        }
        case 2:
        {
            values = getArray<PGint2, long long>(col, size, "%int2");
            break;
        }
        case 4:
        {
            values = getArray<PGint4, long long>(col, size, "%int4");
            break;
        }
        case 8:
        {
            values = getArray<PGint8, long long>(col, size, "%int8");
            break;
        }
        default:
        {
            logger->warning(306, "Value has unknown length", thisClass + "::getInt8A()");
            break;
        }
    }

    return values;
}

vector<long long>* PGResultSet::getInt8V(const int col) {
    vector<long long> *values = NULL;

    switch (getKeyTypeLength(col, 4)) {
        case -1: // numeric
        {
            int size = 0;
            PGnumeric *vals = getArray<PGnumeric, PGnumeric>(col, size, "%numeric");
            if (vals) {
                if (!(values = new vector<long long>)) break;
                for (int i = 0; i < size; i++) {
                    values->push_back(atoll(vals[i]));
                }
                vt_destructall(vals);
            }
            break;
        }
        case 2:
        {
            values = getVector<PGint2, long long>(col, "%int2");
            break;
        }
        case 4:
        {
            values = getVector<PGint4, long long>(col, "%int4");
            break;
        }
        case 8:
        {
            values = getVector<PGint8, long long>(col, "%int8");
            break;
        }
        default:
        {
            logger->warning(306, "Value has unknown length", thisClass + "::getInt8V()");
            break;
        }
    }

    return values;
}

// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================
float PGResultSet::getFloat(const int col)
{
    return (float)getFloat8(col);
}

double PGResultSet::getFloat8(const int col)
{
    double value = 0.0;

    switch (getKeyTypeLength(col, 4))
    {
        case -1: // numeric
        {
            PGnumeric val = getSingleValue<PGnumeric, PGnumeric>(col, "%numeric");
            if (val) value = atof(val);
            break;
        }
        case 4:
        {
            value = getSingleValue<PGfloat4, double>(col, "%float4");
            break;
        }
        case 8:
        {
            value = getSingleValue<PGfloat8, double>(col, "%float8");
            break;
        }
        default:
        {
            logger->warning(306, "Value has unknown length", thisClass + "::getFloat8()");
            break;
        }
    }

    return value;
}

float* PGResultSet::getFloatA(const int col, int& size)
{
    float *values = NULL;
    
    switch(getKeyTypeLength(col, 4))
    {
        case -1:    // numeric
        {
            PGnumeric *vals = getArray<PGnumeric,PGnumeric>(col, size, "%numeric");
            if (vals) {
                if (!(values = new float[size])) break;
                for (int i = 0; i < size; i++) {
                    values[i] = (float)atof(vals[i]);
                }
                vt_destructall(vals);
            }
            break;
        }
        case 4:
        {
            values = getArray<PGfloat4,float>(col, size, "%float4");
            break;
        }
        case 8:
        {
            values = getArray<PGfloat8, float>(col, size, "%float8");
            break;
        }
        default:
        {
            logger->warning(306, "Value has unknown length", thisClass + "::getFloatA()");
            break;
        }
    }
    
    return values;
}

vector<float>* PGResultSet::getFloatV(const int col)
{
    vector<float> *values = NULL;
    
    switch(getKeyTypeLength(col, 4))
    {
        case -1:    // numeric
        {
            int size = 0;
            PGnumeric *vals = getArray<PGnumeric,PGnumeric>(col, size, "%numeric");
            if (vals) {
                if (!(values = new vector<float>)) break;
                for (int i = 0; i < size; i++) {
                    values->push_back((float) atof(vals[i]));
                }
                vt_destructall(vals);
            }
            break;
        }
        case 4:
        {
            values = getVector<PGfloat4,float>(col, "%float4");
            break;
        }
        case 8:
        {
            values = getVector<PGfloat8, float>(col, "%float8");
            break;
        }
        default:
        {
            logger->warning(306, "Value has unknown length", thisClass + "::getFloatV()");
            break;
        }
    }
    
    return values;
}

double* PGResultSet::getFloat8A(const int col, int& size)
{
    double *values = NULL;
    
    switch(getKeyTypeLength(col, 4))
    {
        case -1:    // numeric
        {
            PGnumeric *vals = getArray<PGnumeric,PGnumeric>(col, size, "%numeric");
            if (vals) {
                if (!(values = new double[size])) break;
                for (int i = 0; i < size; i++) {
                    values[i] = atof(vals[i]);
                }
                vt_destructall(vals);
            }
            break;
        }
        case 4:
        {
            values = getArray<PGfloat4, double>(col, size, "%float4");
            break;
        }
        case 8:
        {
            values = getArray<PGfloat8, double>(col, size, "%float8");
            break;
        }
        default:
        {
            logger->warning(306, "Value has unknown length", thisClass + "::getFloat8A()");
            break;
        }
    }
    
    return values;
}

vector<double>* PGResultSet::getFloat8V(const int col)
{
    vector<double> *values = NULL;
    
    switch(getKeyTypeLength(col, 4))
    {
        case -1:    // numeric
        {
            int size = 0;
            PGnumeric *vals = getArray<PGnumeric,PGnumeric>(col, size, "%numeric");
            if (vals) {
                if (!(values = new vector<double>)) break;
                for (int i = 0; i < size; i++) {
                    values->push_back(atof(vals[i]));
                }
                vt_destructall(vals);
            }
            break;
        }
        case 4:
        {
            values = getVector<PGfloat4, double>(col, "%float4");
            break;
        }
        case 8:
        {
            values = getVector<PGfloat8, double>(col, "%float8");
            break;
        }
        default:
        {
            logger->warning(306, "Value has unknown length", thisClass + "::getFloat8V()");
            break;
        }
    }
    
    return values;
}

#if HAVE_OPENCV

cv::Mat *PGResultSet::getCvMat(const int col)
{
    PGresult *matres    = NULL;
    cv::Mat *mat        = NULL;
    PGarray mat_dims_arr = {0};
    int *mat_dim_sizes  = NULL;
    
    do {
        // get cvmat structure
        matres = getSingleValue<PGresult*,PGresult*>(col, "%public.cvmat");
        if (!matres) break;
        
        // get cvmat members
        PGint4 mat_type = 0;
        PGbytea mat_data_bytea = {0};
        if (! pqt.PQgetf(matres, 0, "%int4 %int4[] %bytea",
            0, &mat_type, 1, &mat_dims_arr, 2, &mat_data_bytea)) {
            logger->warning(324, "Cannot get cvmat header", thisClass+"::getCvMat()");
            break;
        }
        
        // create dimensions array
        int mat_dims = pg.PQntuples(mat_dims_arr.res);
        if (!mat_dims) break;
        
        mat_dim_sizes = new int[mat_dims];
        if (!mat_dim_sizes) break;
        
        for (int i = 0; i < mat_dims; i++) {
            pqt.PQgetf(mat_dims_arr.res, i, "%int4", 0, &mat_dim_sizes[i]);
        }
        
        // create matrix
        mat = new cv::Mat(mat_dims, mat_dim_sizes, mat_type);
        if (!mat) {
            logger->warning(324, "Failed to create cv::Mat", thisClass+"::getCvMat()");
            break;
        }

        // copy matrix data
        memcpy(mat->data, mat_data_bytea.data, mat_data_bytea.len);
    } while(0);
    
    if (mat_dims_arr.res) pg.PQclear(mat_dims_arr.res);
    vt_destructall(mat_dim_sizes);
    if (matres) pg.PQclear(matres);

    return mat;
}
#endif

    // =============== GETTERS - GEOMETRIC TYPES ===============================
#if HAVE_POSTGRESQL
PGpoint PGResultSet::getPoint(const int col)
{
    return getSingleValue<PGpoint, PGpoint>(col, "%point");
}

PGpoint* PGResultSet::getPointA(const int col, int& size)
{
    return getArray<PGpoint,PGpoint>(col, size, "%point");
}

vector<PGpoint>*  PGResultSet::getPointV(const int col)
{
    return getVector<PGpoint,PGpoint>(col, "%point");
}
#endif

#if HAVE_POSTGIS
GEOSGeometry* PGResultSet::getGeometry(const int col) {
    GEOSGeometry *geo = NULL;
    if (! pqt.PQgetf(PGRES, this->pos, "%geometry", col, &geo)) {
        logger->warning(325, "Value is not a geometry type", thisClass+"::getGeometry()");
    }
    return geo;
}
GEOSGeometry* PGResultSet::getLineString(const int col) {
    GEOSGeometry *ls = getGeometry(col);
    if (!ls || GEOSGeomTypeId(ls) != GEOS_LINESTRING) {
        logger->warning(326, "Value is not a linestring", thisClass+"::getGeometry()");
    }
    return ls;
}
#endif

// =============== GETTERS - TIMESTAMP =========================================

time_t PGResultSet::getTimestamp(const int col)
{
    PGtimestamp pgts = getSingleValue<PGtimestamp,PGtimestamp>(col, "%timestamp");

    struct tm ts = {0};
    ts.tm_year  = pgts.date.year;
    ts.tm_mon   = pgts.date.mon;
    ts.tm_mday  = pgts.date.mday;
    ts.tm_hour  = pgts.time.hour;
    ts.tm_min   = pgts.time.min;
    ts.tm_sec   = pgts.time.sec;
    
    return mktime(&ts);
}

IntervalEvent *PGResultSet::getIntervalEvent(const int col)
{
    PGresult *evres     = NULL;
    IntervalEvent *event= NULL;
    
    do {
        // get event structure
        evres = getSingleValue<PGresult*, PGresult*>(col, "%public.vtevent");
        if (!evres) break;
        
        // get event members
        PGint4 ev_group_id = 0, ev_class_id = 0;
        PGbool ev_is_root = false;
        PGbox ev_region = {0};
        PGfloat8 ev_score = 0.0;
        PGbytea ev_data = {0};
        if (! pqt.PQgetf(evres, 0, "%int4 %int4 %bool %box %float8 %bytea",
            0, &ev_group_id, 1, &ev_class_id, 2, &ev_is_root, 3, &ev_region, 4, &ev_score, 5, &ev_data)) {
            logger->warning(324, "Cannot get vtevent header", thisClass+"::getIntervalEvent()");
            break;
        }
       
        // create event
        event = new IntervalEvent();
        if (!event) {
            logger->warning(324, "Failed to create IntervalEvent", thisClass+"::getIntervalEvent()");
            break;
        }

        event->group_id = ev_group_id;
        event->class_id = ev_class_id;
        event->is_root = ev_is_root;
        memcpy(&event->region, &ev_region, sizeof(ev_region));
        event->score = ev_score;
        event->user_data_size = ev_data.len;
        event->SetUserData(ev_data.data, ev_data.len);
    } while(0);

    if (evres) pg.PQclear(evres);
    
    return event;
}

// ========================= GETTERS - OTHER ==================================


void *PGResultSet::getBlob(const int col, int &size)
{
    PGbytea value = getSingleValue<PGbytea, PGbytea>(col, "%bytea");
    size = value.len;
    
    return (void *)value.data;
}

// =======================UNIVERSAL GETTER=====================================


#define GET_AND_SERIALIZE_VALUE(FUNC) \
{\
    ret = toString(FUNC(col));\
}
#define GET_AND_SERIALIZE_VALUE_ALLOC(TYPE,FUNC) \
{\
    TYPE *value = FUNC(col);\
    if (value) {\
        ret = toString(*value);\
        delete value;\
    }\
}
#define GET_AND_SERIALIZE_PTR(TYPE, FUNC) \
{\
    int size = 0;\
    TYPE *value = FUNC(col, size);\
    if (value) {\
        ret = toString(value, size, arrayLimit);\
    }\
}
#define GET_AND_SERIALIZE_ARRAY(TYPE, FUNC) \
{\
    int size = 0;\
    TYPE *values = FUNC(col, size);\
    if (values) {\
        ret = toString(values, size, arrayLimit);\
        vt_destructall(values);\
    }\
}
#define GET_AND_SERIALIZE_VALUE_AND_ARRAY(TYPE, FUNC) \
{\
    if (DBTYPE_HASFLAG(dbtype, DBTYPE_FLAG_ARRAY)) {\
        GET_AND_SERIALIZE_ARRAY(TYPE, FUNC ## A);\
    }\
    else {\
        GET_AND_SERIALIZE_VALUE(FUNC);\
    }\
}

string PGResultSet::getValue(const int col, const int arrayLimit)
{
    string ret;
    
    DBTYPE dbtype = 0;
    if (this->dbtypes) {
        DBTYPES_MAP_IT it = this->dbtypes->find(pg.PQftype(PGRES, col));
        if (it != this->dbtypes->end()) {
            dbtype = (*it).second.type;
        }
    }
        
    switch(DBTYPE_GETCATEGORY(dbtype))
    {
        case DBTYPE_STRING:
        {
            GET_AND_SERIALIZE_VALUE(getString);
            break;
        }
        case DBTYPE_INT:
        {
            GET_AND_SERIALIZE_VALUE_AND_ARRAY(long long, getInt8);
            break;
        }
        case DBTYPE_FLOAT:
        {
            GET_AND_SERIALIZE_VALUE_AND_ARRAY(double, getFloat8);
            break;
        }
        case DBTYPE_BOOLEAN:
        {
            GET_AND_SERIALIZE_VALUE(getBool);
            break;
        }
        case DBTYPE_BLOB:
        {
            GET_AND_SERIALIZE_PTR(void, getBlob);
            break;
        }
        case DBTYPE_TIMESTAMP:
        {
            GET_AND_SERIALIZE_VALUE(getTimestamp);
            break;
        }
        case DBTYPE_GEO_POINT:
        {
#if HAVE_POSTGRESQL
            GET_AND_SERIALIZE_VALUE_AND_ARRAY(PGpoint, getPoint);
#endif
            break;
        }
        case DBTYPE_GEO_LSEG:
        {
            break;
        }
        case DBTYPE_GEO_PATH:
        {
            break;
        }
        case DBTYPE_GEO_BOX:
        {
            break;
        }
        case DBTYPE_GEO_POLYGON:
        {
            break;
        }
        case DBTYPE_GEO_LINE:
        {
            break;
        }
        case DBTYPE_GEO_CIRCLE:
        {
            break;
        }
        case DBTYPE_GEO_GEOMETRY:
        {
#if HAVE_POSTGIS
#endif
            break;
        }
        case DBTYPE_UD_SEQTYPE:
        {
            GET_AND_SERIALIZE_VALUE(getString);
            break;
        }
        case DBTYPE_UD_INOUTTYPE:
        {
            GET_AND_SERIALIZE_VALUE(getString);
            break;
        }
        case DBTYPE_UD_CVMAT:
        {
#if HAVE_OPENCV
            GET_AND_SERIALIZE_VALUE_ALLOC(cv::Mat, getCvMat);
#endif
            break;
        }
        case DBTYPE_UD_EVENT:
        {
            GET_AND_SERIALIZE_VALUE_ALLOC(IntervalEvent, getIntervalEvent);
            break;
        }
        case DBTYPE_REF_TYPE:
        {
            break;
        }
        case DBTYPE_REF_CLASS:
        {
            break;
        }
        default:
        {
            break;
        }
    }
    
    return ret;
    
//#if HAVE_POSTGIS
//                    // PostGIS generic geometry type
//                    if (!keytype.compare("geometry")) {
//                        GEOSGeometry *geo;
//                        GEOSWKTWriter *geo_writer;
//                        char * geo_string;
//
//                        if (!(geo = getGeometry(col))) break;
//                        if (!(geo_writer = GEOSWKTWriter_create())) {
//                            GEOSGeom_destroy(geo);
//                            break;
//                        }
//
//                        //TODO: GEOS 2.2 conflict, if resolved uncomment int precision
//                        //GEOSWKTWriter_setRoundingPrecision(geo_writer, precision);
//                        geo_string = GEOSWKTWriter_write(geo_writer, geo);
//                        valss << geo_string;
//
//                        GEOSFree(geo_string);
//                        GEOSGeom_destroy(geo);
//                    }
//#endif        

}



pair< TKeys*, vector<int>* > PGResultSet::getKeysWidths(const int row, bool get_widths, const int arrayLimit) {
    vector<int> *widths = get_widths ? new vector<int>() : NULL;
    TKeys *keys         = getKeys();
    int rows            = pg.PQntuples(PGRES);
    int cols            = pg.PQnfields(PGRES);
    int plen, flen, tlen, width;

    if (!get_widths && keys) return std::make_pair(keys, widths);
    else if (!widths || !keys || cols != keys->size() || cols == 0 || rows == 0) {
        vt_destruct(widths);
        vt_destruct(keys);
        return std::make_pair((TKeys*)NULL, (vector<int>*)NULL);
    }

    // header and first row
    for (int c = 0; c < cols; c++) {
        // don't forget to save original value of pos beforehand!
        this->pos = row < 0 ? 0 : row;
        flen = (*keys)[c].key.length();  // field name length
        tlen = (*keys)[c].type.length(); // data type string length
        plen = getValue(c).length();     // field value string length
        if (plen >= flen && plen >= tlen) width = plen;
        else if (flen >= plen && flen >= tlen) width = flen;
        else width = tlen;
        widths->push_back(width);
    }

    // rest of the rows
    if (row < 0) {
        for (int r = 1; r < rows; r++) {
            // very ugly manipulation with pos, it needs to be reset after print
            this->pos = r;
            for (int c = 0; c < cols; c++) {
                plen = getValue(c).length();
                if (plen > (*widths)[c]) (*widths)[c] = plen;
            }
        }
    }
    if (widths->size() != keys->size()) {
        vt_destruct(widths);
        vt_destruct(keys);
        return std::make_pair((TKeys*)NULL, (vector<int>*)NULL);
    }
    else return std::make_pair(keys, widths);
}




#endif // HAVE_POSTGRESQL

