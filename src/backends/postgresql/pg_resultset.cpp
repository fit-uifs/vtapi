
#include <libpq-fe.h>
#include <vtapi/common/global.h>
#include <vtapi/common/serialize.h>
#include "pg_resultset.h"

#define PGRES ((PGresult *)_res)
#define CHECK_PGRES\
    if (!PGRES) throw RuntimeException("Result set uninitialized, call next()?");

using namespace std;

namespace vtapi {


void PGResultSet::newResult(void *res)
{
    clear();
    _res = res;
}

int PGResultSet::countRows() const
{
    CHECK_PGRES;
    return PQntuples(PGRES);
}

int PGResultSet::countCols() const
{
    CHECK_PGRES;
    return PQnfields(PGRES);
}

bool PGResultSet::isOk() const
{
    return PGRES ? true : false;
}

void PGResultSet::clear()
{
    if (PGRES) {
        PQclear(PGRES);
        _res = NULL;
    }
}

TKey PGResultSet::getKey(int col) const
{
    CHECK_PGRES;
    return TKey(getKeyType(col), PQfname(PGRES, col), string());
}

TKeys PGResultSet::getKeys() const
{
    CHECK_PGRES;
    TKeys keys;

    int cols = PQnfields(PGRES);
    for (int col = 0; col < cols; col++)
        keys.push_back(getKey(col));

    return keys;
}

string PGResultSet::getKeyType(int col) const
{
    CHECK_PGRES;
    return _dbtypes.type(PQftype(PGRES, col))._name;
}

short PGResultSet::getKeyTypeLength(int col, short def) const
{
    CHECK_PGRES;
    return _dbtypes.type(PQftype(PGRES, col))._length;
}

int PGResultSet::getKeyIndex(const string& key) const
{
    CHECK_PGRES;

    int idx = PQfnumber(PGRES, key.c_str());
    if (idx < 0) throw RuntimeException("Failed to find key: " + key);
    return idx;
}

// =============== SINGLE VALUES / VECTORS TEMPLATES ===================

template<typename TDB, typename TOUT>
TOUT PGResultSet::getSingleValue(int col, const char *def) const
{
    CHECK_PGRES;
    TDB value = { 0 };

    if (_pos < 0) {
        throw RuntimeException("Failed to get value: result set position is invalid");
    }
    else if (col < 0) {
        throw RuntimeException("Failed to get value: colum index is invalid");
    }
    else if (!PQgetf(PGRES, _pos, def, col, &value)) {
        string type = getKeyType(col);
        throw RuntimeException(string("Failed to get value: type mismatch ") + def + "!=" + type);
    }

    return static_cast<TOUT>(value);
}

template<typename TDB, typename TOUT>
vector<TOUT> PGResultSet::getVector(int col, const char *def) const
{
    CHECK_PGRES;
    vector<TOUT> values;
    PGarray tmp = { 0 };

    try
    {
        char defArr[64];
        sprintf(defArr, "%s[]", def);

        if (_pos < 0) {
            throw RuntimeException("Failed to get value: result set position is invalid");
        }
        else if (col < 0) {
            throw RuntimeException("Failed to get value: colum index is invalid");
        }
        else if (!PQgetf(PGRES, _pos, defArr, col, &tmp)) {
            string type = getKeyType(col);
            throw RuntimeException(string("Failed to get value: type mismatch ") + def + "!=" + type);
        }

        int size = PQntuples(tmp.res);
        values.resize(size);
        for (int i = 0; i < size; i++) {
            TDB value;
            if (!PQgetf(tmp.res, i, def, 0, &value))
                throw RuntimeException(string("Failed to get value: unexpected value in array of: ") + def);
            else
                values[i] = static_cast<TOUT>(value);
        }
    }
    catch (RuntimeException &e)
    {
        if (tmp.res) PQclear(tmp.res);
        throw;
    }

    return values;
}


// =============== GETTERS  ===============


bool PGResultSet::getBool(int col) const
{
    return getSingleValue<PGbool, bool>(col, "%bool");
}

char PGResultSet::getChar(int col) const
{
    return getSingleValue<PGchar, char>(col, "%char");
}

string PGResultSet::getString(int col) const
{
    CHECK_PGRES;
    string value;

    if (_pos < 0) {
        throw RuntimeException("Failed to get value: result set position is invalid");
    }
    else if (col < 0) {
        throw RuntimeException("Failed to get value: colum index is invalid");
    }
    else {
        char *tmp = PQgetvalue(PGRES, _pos, col);
        if (tmp) value = tmp;
    }

    return value;
}

vector<string> PGResultSet::getStringVector(int col) const
{
    // TODO: string vector
    return vector<string>();
}

int PGResultSet::getInt(int col) const
{
    return static_cast<int>(getInt8(col));
}

vector<int> PGResultSet::getIntVector(int col) const
{
    vector<int> values;

    switch (getKeyTypeLength(col, 4))
    {
    case -1:    // numeric
    {
        int size = 0;
        vector<PGnumeric> vals = getVector<PGnumeric, PGnumeric>(col, "%numeric");
        values.resize(vals.size());
        for (int i = 0; i < size; i++)
            values[i] = std::atoi(vals[i]);
        break;
    }
    case 2:
    {
        values = getVector<PGint2, int>(col, "%int2");
        break;
    }
    case 4:
    {
        values = getVector<PGint4, int>(col, "%int4");
        break;
    }
    case 8:
    {
        values = getVector<PGint8, int>(col, "%int8");
        break;
    }
    default:
    {
        throw RuntimeException("Value has unknown length");
        break;
    }
    }

    return values;
}

long long PGResultSet::getInt8(int col) const
{
    long long value = 0;

    switch (getKeyTypeLength(col, 4))
    {
    case -1:    // numeric
    {
        PGnumeric val = getSingleValue<PGnumeric, PGnumeric>(col, "%numeric");
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
        throw RuntimeException("Value has unknown length");
        break;
    }
    }

    return value;
}

vector<long long> PGResultSet::getInt8Vector(int col) const
{
    vector<long long> values;

    switch (getKeyTypeLength(col, 4))
    {
    case -1: // numeric
    {
        vector<PGnumeric> vals = getVector<PGnumeric, PGnumeric>(col, "%numeric");
        values.resize(vals.size());
        for (int i = 0; i < vals.size(); i++)
            values[i] = std::atoll(vals[i]);
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
        throw RuntimeException("Value has unknown length");
        break;
    }
    }

    return values;
}

float PGResultSet::getFloat(int col) const
{
    return static_cast<float>(getFloat8(col));
}


vector<float> PGResultSet::getFloatVector(int col) const
{
    vector<float> values;

    switch (getKeyTypeLength(col, 4))
    {
    case -1:    // numeric
    {
        vector<PGnumeric> vals = getVector<PGnumeric, PGnumeric>(col, "%numeric");
        values.resize(vals.size());
        for (int i = 0; i < vals.size(); i++)
            values[i] = static_cast<float>(std::atof(vals[i]));
        break;
    }
    case 4:
    {
        values = getVector<PGfloat4, float>(col, "%float4");
        break;
    }
    case 8:
    {
        values = getVector<PGfloat8, float>(col, "%float8");
        break;
    }
    default:
    {
        throw RuntimeException("Value has unknown length");
        break;
    }
    }

    return values;
}

double PGResultSet::getFloat8(int col) const
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
        throw RuntimeException("Value has unknown length");
        break;
    }
    }

    return value;
}

vector<double> PGResultSet::getFloat8Vector(int col) const
{
    vector<double> values;

    switch (getKeyTypeLength(col, 4))
    {
    case -1:    // numeric
    {
        vector<PGnumeric> vals = getVector<PGnumeric, PGnumeric>(col, "%numeric");
        values.resize(vals.size());
        for (int i = 0; i < vals.size(); i++)
            values[i] = atof(vals[i]);
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
        throw RuntimeException("Value has unknown length");
        break;
    }
    }

    return values;
}

chrono::system_clock::time_point PGResultSet::getTimestamp(int col) const
{
    PGtimestamp pgts = getSingleValue<PGtimestamp, PGtimestamp>(col, "%timestamp");
    std::tm ts = { 0 };
    ts.tm_year  = pgts.date.year - 1900;
    ts.tm_mon   = pgts.date.mon;
    ts.tm_mday  = pgts.date.mday;
    ts.tm_hour  = pgts.time.hour;
    ts.tm_min   = pgts.time.min;
    ts.tm_sec   = pgts.time.sec;
    ts.tm_zone  = "GMT0";

    return chrono::system_clock::from_time_t(timegm(&ts)) + chrono::microseconds(pgts.time.usec);
}

cv::Mat *PGResultSet::getCvMat(int col) const
{
    PGresult *matres    = NULL;
    cv::Mat *mat        = NULL;
    PGarray mat_dims_arr = { 0 };

    try
    {
        // get cvmat structure
        matres = getSingleValue<PGresult*, PGresult*>(col, "%public.cvmat");
        if (matres) {
            // get cvmat members
            PGint4 mat_type = 0;
            PGbytea mat_data_bytea = { 0 };
            if (!PQgetf(matres, 0, "%int4 %int4[] %bytea",
                         0, &mat_type, 1, &mat_dims_arr, 2, &mat_data_bytea))
                throw RuntimeException("Cannot get cvmat header");

            // create dimensions array
            int mat_dims = PQntuples(mat_dims_arr.res);
            if (!mat_dims) throw RuntimeException("CvMat has no dimensions");

            vector<int> mat_dim_sizes(mat_dims);
            for (int i = 0; i < mat_dims; i++) {
                if (!PQgetf(mat_dims_arr.res, i, "%int4", 0, &mat_dim_sizes[i]))
                    throw RuntimeException("Failed to get value: unexpected value in CvMat dimensions");
            }

            // create matrix
            mat = new cv::Mat(mat_dims, mat_dim_sizes.data(), mat_type);
            if (!mat) throw RuntimeException("Failed to create cv::Mat");

            // copy matrix data
            memcpy(mat->data, mat_data_bytea.data, mat_data_bytea.len);

            PQclear(mat_dims_arr.res);
            PQclear(matres);
        }
    }
    catch (RuntimeException &e)
    {
        if (mat_dims_arr.res)
            PQclear(mat_dims_arr.res);
        if (matres)
            PQclear(matres);
        throw;
    }

    return mat;
}

Point PGResultSet::getPoint(int col) const
{
    PGpoint pt = getSingleValue<PGpoint, PGpoint>(col, "%point");
    return Point(pt.x, pt.y);
}

vector<Point> PGResultSet::getPointVector(int col) const
{
    vector<PGpoint> pts = getVector<PGpoint, PGpoint>(col, "%point");
    // TODO: optimalize point vector
    vector<Point> ret(pts.size());
    for (size_t i = 0; i < pts.size(); i++)
        ret[i] = Point(pts[i].x, pts[i].y);

    return ret;
}

IntervalEvent PGResultSet::getIntervalEvent(int col) const
{
    PGresult *evres = NULL;
    IntervalEvent event;

    try
    {
        // get event structure
        evres = getSingleValue<PGresult*, PGresult*>(col, "%public.vtevent");
        if (evres) {
            // get event members
            PGint4 ev_group_id = 0;
            PGint4 ev_class_id = 0;
            PGbool ev_is_root = false;
            PGfloat8 ev_score = 0.0;
            PGbox ev_region = { 0 };
            PGbytea ev_data = { 0 };
            if (!PQgetf(evres, 0, "%int4 %int4 %bool %box %float8 %bytea",
                         0, &ev_group_id, 1, &ev_class_id, 2, &ev_is_root,
                         3, &ev_region, 4, &ev_score, 5, &ev_data))
                throw RuntimeException("Failed to get value: unexpected value in event header");

            event.group_id = ev_group_id;
            event.class_id = ev_class_id;
            event.is_root = ev_is_root;
            event.score = ev_score;
            event.region = IntervalEvent::Box(ev_region.high.x, ev_region.high.y,
                                              ev_region.low.x, ev_region.low.y);
            if (ev_data.len > 0)
                event.user_data = vector<char>(ev_data.data, ev_data.data + ev_data.len);

            PQclear(evres);
        }
    }
    catch(RuntimeException &e)
    {
        if (evres) PQclear(evres);
        throw;
    }

    return event;
}

ProcessState PGResultSet::getProcessState(int col) const
{
    PGresult *psres = NULL;
    ProcessState pstate;

    try
    {
        // get process state structure
        psres = getSingleValue<PGresult*, PGresult*>(col, "%public.pstate");
        if (psres) {
            // get event members
            PGvarchar ps_status = NULL, ps_curritem = NULL, ps_lasterror = NULL;
            PGfloat4 ps_progress = 0;
            if (! PQgetf(psres, 0, "%public.pstatus %float4 %varchar %varchar",
                         0, &ps_status, 1, &ps_progress,
                         2, &ps_curritem, 3, &ps_lasterror))
                throw RuntimeException("Failed to get value: unexpected value in pstate header");

            if (ps_status)
                pstate.status = pstate.toStatusValue(ps_status);
            if (ps_curritem)
                pstate.current_item = ps_curritem;
            if (ps_lasterror)
                pstate.last_error = ps_lasterror;
            pstate.progress = ps_progress;

            PQclear(psres);
        }
    }
    catch(RuntimeException &e)
    {
        if (psres) PQclear(psres);
        throw;
    }

    return pstate;
}

vector<char> PGResultSet::getBlob(int col) const
{
    PGbytea value = getSingleValue<PGbytea, PGbytea>(col, "%bytea");
    if (value.len > 0)
        return vector<char>(value.data, value.data + value.len);
    else
        return vector<char>();
}

string PGResultSet::getValue(int col) const
{
    CHECK_PGRES;
    string ret;

    const DatabaseTypes::TypeDefinition & type = _dbtypes.type(PQftype(PGRES, col));

    switch (type._category)
    {
    case DatabaseTypes::CATEGORY_STRING:
    {
        if (type._flags & DatabaseTypes::FLAG_ARRAY)
            ret = toString(getStringVector(col));
        else
            ret = getString(col);
        break;
    }
    case DatabaseTypes::CATEGORY_INT:
    {
        if (type._flags & DatabaseTypes::FLAG_ARRAY)
            ret = toString(getInt8Vector(col));
        else
            ret = toString(getInt8(col));
        break;
    }
    case DatabaseTypes::CATEGORY_FLOAT:
    {
        if (type._flags & DatabaseTypes::FLAG_ARRAY)
            ret = toString(getFloat8Vector(col));
        else
            ret = toString(getFloat8(col));
        break;
    }
    case DatabaseTypes::CATEGORY_BOOLEAN:
    {
        ret = toString(getBool(col));
        break;
    }
    case DatabaseTypes::CATEGORY_BLOB:
    {
        ret = toString(getBlob(col));
        break;
    }
    case DatabaseTypes::CATEGORY_TIMESTAMP:
    {
        ret = toString(getTimestamp(col));
        break;
    }
    case DatabaseTypes::CATEGORY_GEO_POINT:
    {
        if (type._flags & DatabaseTypes::FLAG_ARRAY)
            ret = toString(getPointVector(col));
        else
            ret = toString(getPoint(col));
        break;
    }
    case DatabaseTypes::CATEGORY_GEO_LSEG:
    {
        break;
    }
    case DatabaseTypes::CATEGORY_GEO_PATH:
    {
        break;
    }
    case DatabaseTypes::CATEGORY_GEO_BOX:
    {
        break;
    }
    case DatabaseTypes::CATEGORY_GEO_POLYGON:
    {
        break;
    }
    case DatabaseTypes::CATEGORY_GEO_LINE:
    {
        break;
    }
    case DatabaseTypes::CATEGORY_GEO_CIRCLE:
    {
        break;
    }
    case DatabaseTypes::CATEGORY_GEO_GEOMETRY:
    {
        // PostGIS geometry type
        break;
    }
    case DatabaseTypes::CATEGORY_UD_SEQTYPE:
    {
        ret = getString(col);
        break;
    }
    case DatabaseTypes::CATEGORY_UD_INOUTTYPE:
    {
        ret = getString(col);
        break;
    }
    case DatabaseTypes::CATEGORY_UD_PSTATUS:
    {
        ret = getString(col);
        break;
    }
    case DatabaseTypes::CATEGORY_UD_CVMAT:
    {
        cv::Mat * mat = getCvMat(col);
        if (mat) {
            ret = toString(*mat);
            delete mat;
        }
        break;
    }
    case DatabaseTypes::CATEGORY_UD_EVENT:
    {
        ret = toString(getIntervalEvent(col));
        break;
    }
    case DatabaseTypes::CATEGORY_UD_PSTATE:
    {
        ret = toString(getProcessState(col));
        break;
    }
    case DatabaseTypes::CATEGORY_REF_TYPE:
    {
        break;
    }
    case DatabaseTypes::CATEGORY_REF_CLASS:
    {
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}


}
