
#include <vtapi/common/global.h>
#include <vtapi/common/exception.h>
#include <vtapi/common/serialize.h>
#include "sl_resultset.h"

#define SLRES ((SLres *)_res)

using namespace std;

namespace vtapi {


void SLResultSet::newResult(void *res)
{
    clear();
    _res = res;
}

int SLResultSet::countRows() const
{
    return SLRES->rows;
}

int SLResultSet::countCols() const
{
    return SLRES->cols;
}

bool SLResultSet::isOk() const
{
    return SLRES ? true : false;
}

void SLResultSet::clear()
{
    if (_res) {
        sqlite3_free_table(SLRES->res);
        delete SLRES;
        _res = NULL;
    }
}

TKey SLResultSet::getKey(int col) const
{
    return TKey(string(), SLRES->res[(col * _pos) + col], string());
}

TKeys SLResultSet::getKeys() const
{
    TKeys keys;

    for (int col = 0; col < SLRES->cols; col++)
        keys.push_back(getKey(col));

    return keys;
}

string SLResultSet::getKeyType(const int col) const
{
    char        *val_c  = SLRES->res[col];
    return (val_c) ? string(val_c) : "";
}

int SLResultSet::getKeyIndex(const string& key) const
{
    for (int col = 0; col < SLRES->cols; col++) {
        if (string(SLRES->res[col]).compare(key) == 0) {
            return col;
        }
    }
    return -1;
}


// =============== GETTERS  ===============


string SLResultSet::getValue(int col) const
{
    throw RuntimeException("unimplemented");
    return string();
}

bool SLResultSet::getBool(int col) const
{
    throw RuntimeException("unimplemented");
    return false;
}

char SLResultSet::getChar(int col) const
{
    throw RuntimeException("unimplemented");
    return '\0';
}

string SLResultSet::getString(int col) const
{
    throw RuntimeException("unimplemented");
    return string();
}

vector<string> SLResultSet::getStringVector(int col) const
{
    throw RuntimeException("unimplemented");
    return vector<string>();
}

int SLResultSet::getInt(int col) const
{
    throw RuntimeException("unimplemented");
    return 0;
}

vector<int> SLResultSet::getIntVector(int col) const
{
    throw RuntimeException("unimplemented");
    return vector<int>();
}

long long SLResultSet::getInt8(int col) const
{
    throw RuntimeException("unimplemented");
    return 0;
}


vector<long long> SLResultSet::getInt8Vector(int col) const
{
    throw RuntimeException("unimplemented");
    return vector<long long>();
}

float SLResultSet::getFloat(int col) const
{
    throw RuntimeException("unimplemented");
    return 0;
}

vector<float> SLResultSet::getFloatVector(int col) const
{
    throw RuntimeException("unimplemented");
    return vector<float>();
}

double SLResultSet::getFloat8(int col) const
{
    throw RuntimeException("unimplemented");
    return 0;
}

vector<double> SLResultSet::getFloat8Vector(int col) const
{
    throw RuntimeException("unimplemented");
    return vector<double>();
}

chrono::system_clock::time_point SLResultSet::getTimestamp(int col) const
{
    throw RuntimeException("unimplemented");
    return chrono::system_clock::time_point();
}

cv::Mat SLResultSet::getCvMat(int col) const
{
    throw RuntimeException("unimplemented");
    return cv::Mat();
}

Point SLResultSet::getPoint(int col) const
{
    throw RuntimeException("unimplemented");
    return Point();
}

vector<Point> SLResultSet::getPointVector(int col) const
{
    throw RuntimeException("unimplemented");
    return vector<Point>();
}

IntervalEvent SLResultSet::getIntervalEvent(int col) const
{
    throw RuntimeException("unimplemented");
    return IntervalEvent();
}

EyedeaEdfDescriptor SLResultSet::getEdfDescriptor(int col) const
{
    throw RuntimeException("unimplemented");
    return EyedeaEdfDescriptor();
}

ProcessState SLResultSet::getProcessState(int col) const
{
    throw RuntimeException("unimplemented");
    return ProcessState();
}

vector<char> SLResultSet::getBlob(int col) const
{
    throw RuntimeException("unimplemented");
    return vector<char>();
}


}
