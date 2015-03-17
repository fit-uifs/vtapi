
#include <common/vtapi_global.h>
#include <common/vtapi_misc.h>
#include <common/vtapi_serialize.h>
#include <backends/vtapi_resultset.h>

#if HAVE_POSTGRESQL

// postgres data transfer format: 0=text, 1=binary
#define PG_FORMAT           1

using std::string;
using std::stringstream;
using std::vector;
using std::pair;

using namespace vtapi;

/***********************************************************************************************************************************/

PGResultSet::PGResultSet(const PGBackendBase &base, VTAPI_DBTYPES_MAP *dbtypes) :
    ResultSet(dbtypes),
    PGBackendBase(base)
{
    thisClass = "PGResultSet";
}

PGResultSet::~PGResultSet() {
    clear();
}

void PGResultSet::newResult(void *res) {
    clear();
    this->res = res;
}

int PGResultSet::countRows() {
    return pg.PQntuples((PGresult *) this->res);
}
int PGResultSet::countCols() {
    return pg.PQnfields((PGresult *) this->res);
}
bool PGResultSet::isOk() {
    return (this->res) ? VT_OK : VT_FAIL;
}
void PGResultSet::clear() {
    if (this->res) {
        pg.PQclear((PGresult *) this->res);
        this->res = NULL;
    }
}


TKey PGResultSet::getKey(int col) {
    PGresult *pgres = (PGresult *) this->res;
    string name = pg.PQfname(pgres, col);
    string type = getKeyType(col);

    return TKey(type, name, 0);
}

TKeys* PGResultSet::getKeys() {
    PGresult *pgres = (PGresult *) this->res;
    TKeys* keys     = new TKeys;

    int cols = pg.PQnfields(pgres);
    for (int col = 0; col < cols; col++) {
        keys->push_back(getKey(col));
    }
    return keys;
}

string PGResultSet::getKeyType(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    string type;
    if (this->dbtypes) {
        VTAPI_DBTYPES_MAP::iterator it = this->dbtypes->find(pg.PQftype(pgres, col));
        if (it != this->dbtypes->end()) {
            type = (*it).second.name;
        }
    }
    
    return type;
}

int PGResultSet::getKeyIndex(const string& key) {
    return pg.PQfnumber((PGresult *) this->res, key.c_str());
}

// =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===================
char PGResultSet::getChar(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    PGchar value    = '\0';
    int ret         = pqt.PQgetf(pgres, this->pos, "%char", col, &value);
    if (ret == 0) {
        logger->warning(304, "Value is not an char", thisClass+"::getChar()");
        return '\0';
    }
    else return value;
}

char *PGResultSet::getCharA(const int col, int& size) {
    PGresult *pgres = (PGresult *) this->res;
    PGarray tmp;
    if (! pqt.PQgetf(pgres, this->pos, "%char[]", col, &tmp)) {
        logger->warning(304, "Value is not an array of chars", thisClass+"::getCharA()");
        size = -1;
        return NULL;
    }

    size = pg.PQntuples(tmp.res);
    char* values = new char [size];
    for (int i = 0; i < size; i++) {
        if (! pqt.PQgetf(tmp.res, i, "%char", 0, &values[i])) {
            logger->warning(304, "Unexpected value in char array", thisClass+"::getCharA()");
            size = -1;
            pg.PQclear(tmp.res);
            vt_destruct(values);
            return NULL;
        }
    }
    pg.PQclear(tmp.res);

    return values;
}

string PGResultSet::getString(const int col) {
    PGresult *pgres             = (PGresult *) this->res;
    int oid                     = pg.PQftype(pgres, col);
    VTAPI_DBTYPE dbtype        = (*this->dbtypes)[oid].type;
    char category = VTAPI_DBTYPE_GETCATEGORY(dbtype);
    string value;

    // reference types (regtype, regclass..)
    if (category == TYPE_STRING || category == TYPE_ENUM) {
        value = pg.PQgetvalue(pgres, this->pos, col);
    }
    // reference types (regtype, regclass..)
    // TODO: dodelat v typemanager
    else if(category == TYPE_REFTYPE) {
        
    }
    
    return value;
}

// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================

int PGResultSet::getInt(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    int oid = pg.PQftype(pgres, col);
    VTAPI_DBTYPE dbtype = (*this->dbtypes)[oid].type;
    short length = VTAPI_DBTYPE_GETLENGTH(dbtype);
    int value = 0;
    
    // this circumvents libpqtypes and handles all int types (int, oid...)
    if (length < 0) { // conversion if length == -1
        stringstream iss (pg.PQgetvalue(pgres, this->pos, col));
        iss >> value;
    }
     // short 2-byte int
    else if (length == 2) {
//        pqt.PQgetf(pgres, this->pos, "%int2", col, &value);
        endian_swap2(&value, pg.PQgetvalue(pgres, this->pos, col));
    }
    // 4-byte int
    else if (length == 4) {
//        pqt.PQgetf(pgres, this->pos, "%int4", col, &value);
        endian_swap4(&value, pg.PQgetvalue(pgres, this->pos, col));
    }
    else {
        if (length == 8) {
            logger->warning(306, "Use getInt8(...) to retrieve int8 values", thisClass+"::getInt()");
        }
        else {
            logger->warning(306, "Integer value of length "+toString(length)+" is not supported", thisClass+"::getInt()");
        } 
    }
    return value;
}

long PGResultSet::getInt8(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    int oid = pg.PQftype(pgres, col);
    VTAPI_DBTYPE dbtype = (*this->dbtypes)[oid].type;
    short length = VTAPI_DBTYPE_GETLENGTH(dbtype);
    long value = 0;

    // extract long or call getInt if value is integer
    if (length == 8) {
//        pqt.PQgetf(pgres, this->pos, "%int8", col, &value);
        endian_swap8(&value, pg.PQgetvalue(pgres, this->pos, col));
    }
    else if (length < 8) {
        value = (long) getInt(col);
    }
    else {
        logger->warning(306, "Integer value of length "+toString(length)+" is not supported", thisClass+"::getInt8()");
    }
    return value;
}

int* PGResultSet::getIntA(const int col, int& size) {
    PGresult *pgres = (PGresult *) this->res;
    PGarray tmp;
    if (! pqt.PQgetf(pgres, this->pos, "%int4[]", col, &tmp)) {
        logger->warning(307, "Value is not an array of integer", thisClass+"::getIntA()");
        size = -1;
        return NULL;
    }

    size = pg.PQntuples(tmp.res);
    int* values = new int [size];
    for (int i = 0; i < size; i++) {
        if (! pqt.PQgetf(tmp.res, i, "%int4", 0, &values[i])) {
            logger->warning(308, "Unexpected value in integer array", thisClass+"::getIntA()");
            size = -1;
            pg.PQclear(tmp.res);
            vt_destruct(values);
            return NULL;
        }
    }
    pg.PQclear(tmp.res);

    return values;
}

vector<int>* PGResultSet::getIntV(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    PGarray tmp;
    if (! pqt.PQgetf(pgres, this->pos, "%int4[]", col, &tmp)) {
        logger->warning(307, "Value is not an array of integer", thisClass+"::getIntV()");
        return NULL;
    }

    PGint4 value;
    vector<int>* values = new vector<int>;

    int ntuples = pg.PQntuples(tmp.res);
    for (int i = 0; i < ntuples; i++) {
        if (! pqt.PQgetf(tmp.res, i, "%int4", 0, &value)) {
            logger->warning(308, "Unexpected value in integer array", thisClass+"::getIntV()");
            pg.PQclear(tmp.res);
            vt_destruct(values);
            return NULL;
        }
        values->push_back(value);
    }
    pg.PQclear(tmp.res);

    return values;
}

vector< vector<int>* >* PGResultSet::getIntVV(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    PGarray tmp;
    if (! pqt.PQgetf(pgres, this->pos, "%int4[]", col, &tmp)) {
        logger->warning(307, "Value is not an array of integer arrays", thisClass+"::getIntVV()");
        return NULL;
    }
    if (tmp.ndims != 2) {
        logger->warning(308, "Array must have exactly 2 dimensions", thisClass+"::getIntVV()");
        return NULL;
    }

    PGint4 value;
    vector< vector<int>* >* arrays = new vector< vector<int>* >;
    vector<int>* arr;

    // array rows
    for (int i = 0; i < tmp.dims[0]; i++) {
        arr = new vector<int>;
        // array columns
        for (int j = 0; j < tmp.dims[1]; j++) {
            if (! pqt.PQgetf(tmp.res, i*tmp.dims[1]+j, "%int4", 0, &value)) {
                logger->warning(308, "Unexpected value in integer array", thisClass+"::getIntVV()");
                pg.PQclear(tmp.res);
                for (int x = 0; x < (*arrays).size(); x++) vt_destruct((*arrays)[x]);
                vt_destruct(arrays);
                return NULL;
            }
            arr->push_back(value);
        }
        arrays->push_back(arr);
    }
    return arrays;
}

// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================
float PGResultSet::getFloat(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    int oid = pg.PQftype(pgres, col);
    VTAPI_DBTYPE dbtype = (*this->dbtypes)[oid].type;
    short length = VTAPI_DBTYPE_GETLENGTH(dbtype);
    float value = 0;
    
    if (length < 0) { // conversion if length == -1
        stringstream iss (pg.PQgetvalue(pgres, this->pos, col));
        iss >> value;
    }
    else if (length == 4) {
//        pqt.PQgetf(pgres, this->pos, "%float4", col, &value);
        endian_swap4(&value, pg.PQgetvalue(pgres, this->pos, col));
    }
    else {
        logger->warning(306, "Float value of length "+toString(length)+" is not supported", thisClass+"::getFloat()");
    }
    return value;
}

double PGResultSet::getFloat8(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    int oid = pg.PQftype(pgres, col);
    VTAPI_DBTYPE dbtype = (*this->dbtypes)[oid].type;
    short length = VTAPI_DBTYPE_GETLENGTH(dbtype);
    double value = 0;

    if (length < 8) {
        value = (double) getFloat(col);
    }
    else  if (length == 8) {
        pqt.PQgetf(pgres, this->pos, "%float8", col, &value);
    }
    else {
        logger->warning(306, "Float value of length "+toString(length)+" is not supported", thisClass+"::getFloat8()");
    }
    return value;
}

float* PGResultSet::getFloatA(const int col, int& size) {
    PGresult *pgres = (PGresult *) this->res;
    PGarray tmp;
    if (! pqt.PQgetf(pgres, this->pos, "%float4[]", col, &tmp)) {
        logger->warning(311, "Value is not an array of float", thisClass+"::getFloatA()");
        size = -1;
        return NULL;
    }

    size = pg.PQntuples(tmp.res);
    float* values = new float [size];
    for (int i = 0; i < size; i++) {
        if (! pqt.PQgetf(tmp.res, i, "%float4", 0, &values[i])) {
            logger->warning(312, "Unexpected value in float array", thisClass+"::getFloatA()");
            size = -1;
            pg.PQclear(tmp.res);
            vt_destruct(values);
            return NULL;
        }
    }
    pg.PQclear(tmp.res);

    return values;
}

vector<float>* PGResultSet::getFloatV(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    PGarray tmp;
    if (! pqt.PQgetf(pgres, this->pos, "%float4[]", col, &tmp)) {
        logger->warning(311, "Value is not an array of float", thisClass+"::getFloatV()");
        return NULL;
    }

    PGfloat4 value;
    vector<float>* values = new vector<float>;
    for (int i = 0; i < pg.PQntuples(tmp.res); i++) {
        if (! pqt.PQgetf(tmp.res, i, "%float4", 0, &value)) {
            logger->warning(312, "Unexpected value in float array", thisClass+"::getFloatV()");
            pg.PQclear(tmp.res);
            vt_destruct(values);
            return NULL;
        }
        values->push_back(value);
    }
    pg.PQclear(tmp.res);

    return values;
}

#if HAVE_OPENCV

cv::Mat *PGResultSet::getCvMat(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    PGresult *matres    = NULL;
    cv::Mat *mat        = NULL;
    PGint4 mat_type = 0;
    int mat_dims = 0, *mat_dim_sizes = NULL;
    PGarray mat_dims_arr   = {0};
    PGbytea mat_data_bytea  = {0};
    
    do {
        // get cvmat structure
        if (! pqt.PQgetf(pgres, this->pos, "%public.cvmat", col, &matres)) {
            logger->warning(324, "Value is not a correct cvmat type", thisClass+"::getCvMat()");
            break;
        }
        // get data from cvmat structure
        if (! pqt.PQgetf(matres, 0, "%int4 %int4[] %bytea",
            0, &mat_type, 1, &mat_dims_arr, 2, &mat_data_bytea)) {
            logger->warning(324, "Cannot get cvmat header", thisClass+"::getCvMat()");
            break;
        }
        
        // create dimensions array
        mat_dims = pg.PQntuples(mat_dims_arr.res);
        mat_dim_sizes = new int[mat_dims];
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
    if (mat_dim_sizes) delete[] mat_dim_sizes;
    if (matres) pg.PQclear(matres);

    return mat;
}
#endif

    // =============== GETTERS - GEOMETRIC TYPES ===============================
#if HAVE_POSTGRESQL
PGpoint PGResultSet::getPoint(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    PGpoint point = {0.0, 0.0};
    if (! pqt.PQgetf(pgres, this->pos, "%point", col, &point)) {
        logger->warning(314, "Value is not a point", thisClass+"::getPoint()");
    }
    return point;
}
vector<PGpoint>*  PGResultSet::getPointV(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    PGarray tmp;
    if (! pqt.PQgetf(pgres, this->pos, "%point[]", col, &tmp)) {
        logger->warning(324, "Value is not an array of points", thisClass+"::getPointV()");
        return NULL;
    }

    PGpoint value;
    vector<PGpoint>* values = new vector<PGpoint>;

    for (int i = 0; i < PQntuples(tmp.res); i++) {
        if (! pqt.PQgetf(tmp.res, i, "%point", 0, &value)) {
            logger->warning(325, "Unexpected value in point array", thisClass+"::getPointV()");
            pg.PQclear(tmp.res);
            vt_destruct(values);
            return NULL;
        }
        values->push_back(value);
    }
    pg.PQclear(tmp.res);

    return values;
}
#endif

#if HAVE_POSTGIS
GEOSGeometry* PGResultSet::getGeometry(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    GEOSGeometry *geo = NULL;
    if (! pqt.PQgetf(pgres, this->pos, "%geometry", col, &geo)) {
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

time_t PGResultSet::getTimestamp(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    struct tm ts = {0};
    string dtype = getKey(col).type;

    if (!dtype.compare("time")) {
        PGtime timestamp;
        pqt.PQgetf(pgres, this->pos, "%time", col, &timestamp);
        ts.tm_hour  = timestamp.hour;
        ts.tm_min   = timestamp.min;
        ts.tm_sec   = timestamp.sec;
    }
    else if (!dtype.compare("timestamp")) {
        PGtimestamp timestamp;
        pqt.PQgetf(pgres, this->pos, "%timestamp", col, &timestamp);
        ts.tm_year  = timestamp.date.year;
        ts.tm_mon   = timestamp.date.mon;
        ts.tm_mday  = timestamp.date.mday;
        ts.tm_hour  = timestamp.time.hour;
        ts.tm_min   = timestamp.time.min;
        ts.tm_sec   = timestamp.time.sec;
    }
    else {
        logger->warning(313, "Data type "+dtype+" not yet supported", thisClass+"::getTimestamp()");
    }
    return mktime(&ts);
}

int PGResultSet::getIntOid(const int col) {
    PGresult *pgres = (PGresult *) this->res;
    PGint4 value;

    pqt.PQgetf(pgres, this->pos, "%oid", col, &value);
    return (int) value;
}

IntervalEvent *PGResultSet::getIntervalEvent(const int col) {
    PGresult *pgres     = (PGresult *) this->res;
    PGresult *evres     = NULL;
    IntervalEvent *event= NULL;
    PGint4 ev_group_id = 0, ev_class_id = 0;
    PGbool ev_is_root = false;
    PGbox ev_region = {0};
    PGfloat8 ev_score = 0.0;
    PGbytea ev_data = {0};
    
    do {
        // get event structure
        if (! pqt.PQgetf(pgres, this->pos, "%public.vtevent", col, &evres)) {
            logger->warning(324, "Value is not a correct vtevent type", thisClass+"::getIntervalEvent()");
            break;
        }
        // get data from event structure
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

string PGResultSet::getValue(const int col, const int arrayLimit) {
    stringstream valss;                         //return stringstream
    PGresult *pgres = (PGresult *) this->res;


    do {
        if (!this->dbtypes || !pgres || pg.PQgetisnull(pgres, this->pos, col)) break;

        int oid = pg.PQftype(pgres, col);
        VTAPI_DBTYPE_DEFINITION_T &def = (*this->dbtypes)[oid];
        short length = VTAPI_DBTYPE_GETLENGTH(def.type);
        
        if (VTAPI_DBTYPE_HASFLAG(def.type, TYPE_FLAG_ARRAY)) {
            //TODO: arrays of other types
            if (def.name.find("int") != string::npos) {
                // array of 4-byte ints
                if (length == 4) {
                    vector<int>* arr = this->getIntV(col);
                    if (arr) for (int i = 0; i < (*arr).size(); i++) {
                            valss << (*arr)[i];
                            if (arrayLimit && i == arrayLimit) {
                                valss << "...";
                                break;
                            }
                            if (i < (*arr).size() - 1) valss << ",";
                        }
                    vt_destruct(arr);
                }
            }
            else if (def.name.find("float") != string::npos) {
                // array of 4-byte floats
                if (length == 4) {
                    vector<float>* arr = this->getFloatV(col);
                    if (arr) for (int i = 0; i < (*arr).size(); i++) {
                            valss << (*arr)[i];
                            if (arrayLimit && i == arrayLimit) {
                                valss << "...";
                                break;
                            }
                            if (i < (*arr).size() - 1) valss << ",";
                        }
                    vt_destruct(arr);
                }
            }
            else if (def.name.find("char") != string::npos) {
                // array of 1-byte chars
                if (length == 1) {
                    int arr_size;
                    char *arr = this->getCharA(col, arr_size);
                    for (int i = 0; i < arr_size; i++) valss << arr[i];
                    vt_destruct(arr);
                }
            }
                // array of PGpoint
            else if (def.name.find("point") != string::npos) {
                vector<PGpoint>* arr = this->getPointV(col);
                if (arr) for (int i = 0; i < (*arr).size(); i++) {
                        valss << "(" << (*arr)[i].x << "," << (*arr)[i].y << ")";
                        if (arrayLimit && i == arrayLimit) {
                            valss << "...";
                            break;
                        }
                        if (i < (*arr).size() - 1) valss << ",";
                    }
                vt_destruct(arr);
            }
        }
        else {
            switch(VTAPI_DBTYPE_GETCATEGORY(def.type))
            {
                case TYPE_BOOLEAN:
                { // NOT IMPLEMENTED
                }
                break;

                case TYPE_COMPOSITE:
                {
#if HAVE_OPENCV
                    // OpenCV cvMat type
                    if (!def.name.compare("cvmat")) {
                        cv::Mat *mat = getCvMat(col);
                        if (mat) {
                            valss << mat->type();
                            delete mat;
                        }
                    }
#endif
                }
                break;

                case TYPE_DATE:
                {
                    time_t ts = getTimestamp(col);
                    valss << toString(ts);
                    //            valss << std::right << std::setfill('0');
                    //            if (ts.tm_year > 0)
                    //                valss << std::setw(4) << ts.tm_year << '-' << std::setw(2) << ts.tm_mon <<
                    //                  '-' << std::setw(2) << ts.tm_mday << ' ' << std::setw(2) << ts.tm_hour <<
                    //                  ':' << std::setw(2) << ts.tm_min  << ':' << std::setw(2) << ts.tm_sec;
                }
                break;

                case TYPE_ENUM:
                {
                    valss << getString(col);
                }
                break;

                case TYPE_GEOMETRIC:
                {
                    //            // PostGIS point type
                    //            if (!keytype.compare("point")) {
                    //                PGpoint point = getPoint(col);
                    //                valss << point.x << " , " << point.y;
                    //            }
                    //            // PostGIS box type
                    //            else if (!keytype.compare("box")) {
                    //                PGbox box = getBox(col);
                    //                valss << '(' << box.low.x << " , " << box.low.y << ") , ";
                    //                valss << '(' << box.high.x << " , " << box.high.y << ')';
                    //            }
                    //            // PostGIS line-segment type
                    //            else if (!keytype.compare("lseg")) {
                    //                PGlseg lseg = getLineSegment(col);
                    //                valss << '(' << lseg.pts[0].x << " , " << lseg.pts[0].y << ") ";
                    //                valss << '(' << lseg.pts[1].x << " , " << lseg.pts[1].y << ')';
                    //            }
                    //            // PostGIS circle type
                    //            else if (!keytype.compare("circle")) {
                    //                PGcircle circle = getCircle(col);
                    //                valss << '(' << circle.center.x << " , " << circle.center.y;
                    //                valss << ") , " << circle.radius;
                    //            }
                    //            // PostGIS path type
                    //            else if (!keytype.compare("path")) {
                    //                PGpath path = getPath(col);
                    //                for (int i = 0; i < path.npts; i++) {
                    //                    valss << '(' << path.pts[i].x << " , " << path.pts[i].y << ')';
                    //                    if (arrayLimit && i == arrayLimit) {
                    //                        valss << "...";
                    //                        break;
                    //                    }
                    //                    if (i < path.npts-1) valss << " , ";
                    //                }
                    //            }
                    //            // PostGIS polygon type
                    //            else if (!keytype.compare("polygon")) {
                    //                PGpolygon polygon = getPolygon(col);
                    //                for (int i = 0; i < polygon.npts; i++) {
                    //                    valss << '(' << polygon.pts[i].x << " , " << polygon.pts[i].y << ')';
                    //                    if (arrayLimit && i == arrayLimit) {
                    //                        valss << "...";
                    //                        break;
                    //                    }
                    //                    if (i < polygon.npts-1) valss << " , ";
                    //                }
                    //            }
                }
                break;

                case TYPE_NUMERIC:
                {
                    // recognize 4 or 8-bytes integers and floats
                    if (!def.name.substr(0, 5).compare("float")) {
                        length < 8 ? valss << getFloat(col) : valss << getFloat8(col);
                    }
                    else {
                        length < 8 ? valss << getInt(col) : valss << getInt8(col);
                    }
                }
                break;

                case TYPE_STRING:
                {
                    if (length == 1) valss << getChar(col); // char has length 1
                    else return getString(col);
                }
                break;
                case TYPE_UD_SEQTYPE:
                {
                }
                break;
                case TYPE_UD_INOUTTYPE:
                {
                }
                break;
                case TYPE_UD_EVENT:
                {
                }
                break;
                case TYPE_UD_CVMAT:
                {
#if HAVE_OPENCV
                    
#endif
                }
                break;
#if HAVE_POSTGIS
                    // PostGIS generic geometry type
                    if (!keytype.compare("geometry")) {
                        GEOSGeometry *geo;
                        GEOSWKTWriter *geo_writer;
                        char * geo_string;

                        if (!(geo = getGeometry(col))) break;
                        if (!(geo_writer = GEOSWKTWriter_create())) {
                            GEOSGeom_destroy(geo);
                            break;
                        }

                        //TODO: GEOS 2.2 conflict, if resolved uncomment int precision
                        //GEOSWKTWriter_setRoundingPrecision(geo_writer, precision);
                        geo_string = GEOSWKTWriter_write(geo_writer, geo);
                        valss << geo_string;

                        GEOSFree(geo_string);
                        GEOSGeom_destroy(geo);
                    }
#endif
                case TYPE_REFTYPE:
                {
                    // TODO: Petr if (keytype.equals("regclass") == 0) // pg_namespace.oid > 99 (pg_catalog, pg_toast)
                }
                break;

                default:
                {
                    valss << "???";
                }
                break;
            }
        }
        
    } while(0);

    return valss.str();
}



pair< TKeys*, vector<int>* > PGResultSet::getKeysWidths(const int row, bool get_widths, const int arrayLimit) {
    PGresult *pgres     = (PGresult *) this->res;
    vector<int> *widths = get_widths ? new vector<int>() : NULL;
    TKeys *keys         = getKeys();
    int rows            = pg.PQntuples(pgres);
    int cols            = pg.PQnfields(pgres);
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

