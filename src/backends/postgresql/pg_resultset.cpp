
#include "pg_resultset.h"
#include "pg_types.h"
#include <vtapi/common/global.h>
#include <vtapi/common/serialize.h>
#include <libpqtypes.h>


#define CHECK_PGRES\
    if (!_res) throw RuntimeException("Result set uninitialized, call next()?");

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
    return PQntuples(static_cast<const PGresult *>(_res));
}

int PGResultSet::countCols() const
{
    CHECK_PGRES;
    return PQnfields(static_cast<const PGresult *>(_res));
}

bool PGResultSet::isOk() const
{
    return _res ? true : false;
}

void PGResultSet::clear()
{
    if (_res) {
        PQclear(static_cast<PGresult *>(_res));
        _res = NULL;
    }
}

TKey PGResultSet::getKey(int col) const
{
    CHECK_PGRES;
    return TKey(getKeyType(col), PQfname(static_cast<const PGresult *>(_res), col), string());
}

TKeys PGResultSet::getKeys() const
{
    CHECK_PGRES;
    TKeys keys;

    int cols = PQnfields(static_cast<const PGresult *>(_res));
    for (int col = 0; col < cols; col++)
        keys.push_back(getKey(col));

    return keys;
}

string PGResultSet::getKeyType(int col) const
{
    CHECK_PGRES;
    return _dbtypes.type(PQftype(static_cast<const PGresult *>(_res), col))._name;
}

short PGResultSet::getKeyTypeLength(int col, short def) const
{
    CHECK_PGRES;
    return _dbtypes.type(PQftype(static_cast<const PGresult *>(_res), col))._length;
}

int PGResultSet::getKeyIndex(const string& key) const
{
    CHECK_PGRES;

    int idx = PQfnumber(static_cast<const PGresult *>(_res), key.c_str());
    if (idx < 0) throw RuntimeException("Failed to find key: " + key);
    return idx;
}

// =============== TEMPLATE HELPERS  ===============

template <typename T>
T PGResultSet::getIntegerSingle(int col) const
{
    switch (getKeyTypeLength(col, 4))
    {
    case -1:
        return GetterSingle<PGnumeric,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%numeric");
    case 2:
        return GetterSingle<PGint2,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%int2");
    case 4:
        return GetterSingle<PGint4,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%int4");
    case 8:
        return GetterSingle<PGint8,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%int8");
    default:
        throw RuntimeException("Integer value has unknown length");
    }

    return T();
}

template <typename T>
vector<T> PGResultSet::getIntegerVector(int col) const
{
    switch (getKeyTypeLength(col, 4))
    {
    case -1:
        return GetterVector<PGnumeric,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%numeric", "%numeric[]");
    case 2:
        return GetterVector<PGint2,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%int2", "%int2[]");
    case 4:
        return GetterVector<PGint4,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%int4", "%int4[]");
    case 8:
        return GetterVector<PGint8,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%int8", "%int8[]");
    default:
        throw RuntimeException("Integer value has unknown length");
        break;
    }

    return vector<T>();
}

template <typename T>
T PGResultSet::getFloatSingle(int col) const
{
    switch (getKeyTypeLength(col, 4))
    {
    case -1:
        return GetterSingle<PGnumeric,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%numeric");
    case 4:
        return GetterSingle<PGfloat4,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%float4");
    case 8:
        return GetterSingle<PGfloat8,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%float8");
    default:
        throw RuntimeException("Float value has unknown length");
    }

    return T();
}

template <typename T>
vector<T> PGResultSet::getFloatVector(int col) const
{
    switch (getKeyTypeLength(col, 4))
    {
    case -1:
        return GetterVector<PGnumeric,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%numeric", "%numeric[]");
    case 4:
        return GetterVector<PGfloat4,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%float4", "%float4[]");
    case 8:
        return GetterVector<PGfloat8,T>::get(static_cast<const PGresult *>(_res), _pos, col, "%float8", "%float8[]");
    default:
        throw RuntimeException("Float value has unknown length");
        break;
    }

    return vector<T>();
}

// =============== GETTERS  ===============


bool PGResultSet::getBool(int col) const
{
    return GetterSingle<PGbool,bool>::get(static_cast<const PGresult *>(_res), _pos, col, "%bool");
}

char PGResultSet::getChar(int col) const
{
    return GetterSingle<PGchar,char>::get(static_cast<const PGresult *>(_res), _pos, col, "%char");
}

string PGResultSet::getString(int col) const
{
    return GetterSingle<PGtext,string>::get(static_cast<const PGresult *>(_res), _pos, col, "%text");
}

vector<string> PGResultSet::getStringVector(int col) const
{
    return GetterVector<PGtext,string>::get(static_cast<const PGresult *>(_res), _pos, col, "%text", "%text[]");
}

int PGResultSet::getInt(int col) const
{
    return getIntegerSingle<int>(col);
}

vector<int> PGResultSet::getIntVector(int col) const
{
    return getIntegerVector<int>(col);
}

long long PGResultSet::getInt8(int col) const
{
    return getIntegerSingle<long long>(col);
}

vector<long long> PGResultSet::getInt8Vector(int col) const
{
    return getIntegerVector<long long>(col);
}

float PGResultSet::getFloat(int col) const
{
    return getFloatSingle<float>(col);
}

vector<float> PGResultSet::getFloatVector(int col) const
{
    return getFloatVector<float>(col);
}

double PGResultSet::getFloat8(int col) const
{
    return getFloatSingle<double>(col);
}

vector<double> PGResultSet::getFloat8Vector(int col) const
{
    return getFloatVector<double>(col);
}

chrono::system_clock::time_point PGResultSet::getTimestamp(int col) const
{
    return GetterSingle<PGtimestamp,chrono::system_clock::time_point>::get(static_cast<const PGresult *>(_res), _pos, col, "%timestamp");
}

cv::Mat PGResultSet::getCvMat(int col) const
{
    return GetterSingle<PGresult*,cv::Mat>::get(static_cast<const PGresult *>(_res), _pos, col, "%public.cvmat");
}

Point PGResultSet::getPoint(int col) const
{
    return GetterSingle<PGpoint,Point>::get(static_cast<const PGresult *>(_res), _pos, col, "%point");
}

vector<Point> PGResultSet::getPointVector(int col) const
{
    return GetterVector<PGpoint,Point>::get(static_cast<const PGresult *>(_res), _pos, col, "%point", "%point[]");
}

IntervalEvent PGResultSet::getIntervalEvent(int col) const
{
    return GetterSingle<PGresult*,IntervalEvent>::get(static_cast<const PGresult *>(_res), _pos, col, "%public.vtevent");
}

ProcessState PGResultSet::getProcessState(int col) const
{
    return GetterSingle<PGresult*,ProcessState>::get(static_cast<const PGresult *>(_res), _pos, col, "%public.pstate");
}

vector<char> PGResultSet::getBlob(int col) const
{
    return GetterSingle< PGbytea,vector<char> >::get(static_cast<const PGresult *>(_res), _pos, col, "%bytea");
}

string PGResultSet::getValue(int col) const
{
    CHECK_PGRES;
    string ret;

    Oid oid = PQftype(static_cast<const PGresult *>(_res), col);
    const DatabaseTypes::TypeDefinition & type = _dbtypes.type(oid);

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
        ret = toString(getCvMat(col));
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
