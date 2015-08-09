
#include <sstream>
#include <vtapi/common/global.h>
#include <vtapi/common/serialize.h>
#include "sl_resultset.h"

#define SLRES ((SLres *)_res)

using namespace std;

namespace vtapi {


SLResultSet::SLResultSet()
    : ResultSet(NULL)
{
}

SLResultSet::~SLResultSet()
{
    clear();
}

void SLResultSet::newResult(void *res)
{
    clear();
    _res = res;
}

int SLResultSet::countRows()
{
    return SLRES->rows;
}

int SLResultSet::countCols()
{
    return SLRES->cols;
}

bool SLResultSet::isOk()
{
    return SLRES ? true : false;
}

void SLResultSet::clear()
{
    if (_res) {
        sqlite3_free_table(SLRES->res);
        delete (SLRES);
        _res = NULL;
    }
}

TKey SLResultSet::getKey(int col)
{
    return TKey("", SLRES->res[(col * _pos) + col], 0, "");
}

TKeys* SLResultSet::getKeys()
{
    TKeys* keys = new TKeys;

    for (int col = 0; col < SLRES->cols; col++) {
        keys->push_back(getKey(col));
    }
    return keys;
}

string SLResultSet::getKeyType(const int col)
{
    char        *val_c  = SLRES->res[col];
    return (val_c) ? string(val_c) : "";
}

int SLResultSet::getKeyIndex(const string& key)
{
    for (int col = 0; col < SLRES->cols; col++) {
        if (string(SLRES->res[col]).compare(key) == 0) {
            return col;
        }
    }
    return -1;
}

//// =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===================

char SLResultSet::getChar(const int col)
{
    size_t      sl_res_i    = (SLRES->cols)*(_pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > SLRES->cols * (SLRES->rows + 1)) return '\0';
    else return *(SLRES->res[sl_res_i]);
}

string SLResultSet::getString(const int col)
{
    size_t      sl_res_i    = (SLRES->cols)*(_pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > SLRES->cols * (SLRES->rows + 1)) return "";
    else return SLRES->res[sl_res_i];
}

//// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================

bool SLResultSet::getBool(const int col)
{
    return getInt(col) > 0;
}

int SLResultSet::getInt(const int col)
{
    size_t      sl_res_i    = (SLRES->cols)*(_pos + 1) + col;
    int         ret         = 0;
    if (sl_res_i < 0 || sl_res_i > SLRES->cols * (SLRES->rows + 1)) return 0;
    else {
        stringstream(SLRES->res[sl_res_i]) >> ret;
        return ret;
    }
}

long long SLResultSet::getInt8(const int col)
{
    size_t      sl_res_i    = (SLRES->cols)*(_pos + 1) + col;
    long        ret         = 0;
    if (sl_res_i < 0 || sl_res_i > SLRES->cols * (SLRES->rows + 1)) return 0;
    else {
        stringstream(SLRES->res[sl_res_i]) >> ret;
        return ret;
    }
}

int* SLResultSet::getIntA(const int col, int& size)
{
    size_t      sl_res_i    = (SLRES->cols)*(_pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > SLRES->cols * (SLRES->rows + 1)) return NULL;
    else {
        return deserializeA<int>(SLRES->res[sl_res_i], size);
    }
}

vector<int>* SLResultSet::getIntV(const int col)
{
    size_t      sl_res_i    = (SLRES->cols)*(_pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > SLRES->cols * (SLRES->rows + 1)) return NULL;
    else {
        return deserializeV<int>(SLRES->res[sl_res_i]);
    }
}

long long* SLResultSet::getInt8A(const int col, int& size)
{
    size_t sl_res_i = (SLRES->cols)*(_pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > SLRES->cols * (SLRES->rows + 1)) return NULL;
    else {
        return deserializeA<long long>(SLRES->res[sl_res_i], size);
    }
}

vector<long long>* SLResultSet::getInt8V(const int col)
{
    size_t sl_res_i = (SLRES->cols)*(_pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > SLRES->cols * (SLRES->rows + 1)) return NULL;
    else {
        return deserializeV<long long>(SLRES->res[sl_res_i]);
    }
}

//// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================

float SLResultSet::getFloat(const int col)
{
    size_t      sl_res_i    = (SLRES->cols)*(_pos + 1) + col;
    char        *val_c      = SLRES->res[sl_res_i];
    float       ret         = 0;
    if (val_c && col >= 0)  stringstream (val_c) >> ret;
    return ret;
}

double SLResultSet::getFloat8(const int col)
{
    size_t      sl_res_i    = (SLRES->cols)*(_pos + 1) + col;
    char        *val_c      = SLRES->res[sl_res_i];
    double      ret         = 0;
    if (val_c && col >= 0)  stringstream (val_c) >> ret;
    return ret;
}

float* SLResultSet::getFloatA(const int col, int& size)
{
    size_t      sl_res_i    = (SLRES->cols)*(_pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > SLRES->cols * (SLRES->rows + 1)) return NULL;
    else {
        return deserializeA<float>(SLRES->res[sl_res_i], size);
    }
}

vector<float>* SLResultSet::getFloatV(const int col)
{
    size_t      sl_res_i    = (SLRES->cols)*(_pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > SLRES->cols * (SLRES->rows + 1)) return NULL;
    else {
        return deserializeV<float>(SLRES->res[sl_res_i]);
    }
}

double* SLResultSet::getFloat8A(const int col, int& size)
{
    size_t sl_res_i = (SLRES->cols)*(_pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > SLRES->cols * (SLRES->rows + 1)) return NULL;
    else {
        return deserializeA<double>(SLRES->res[sl_res_i], size);
    }
}

vector<double>* SLResultSet::getFloat8V(const int col)
{
    size_t sl_res_i = (SLRES->cols)*(_pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > SLRES->cols * (SLRES->rows + 1)) return NULL;
    else {
        return deserializeV<double>(SLRES->res[sl_res_i]);
    }
}

cv::Mat *SLResultSet::getCvMat(const int col)
{
    cv::Mat *mat = NULL;
    //    PGresult *mres;
    //    PGarray step_arr;
    //    int type, rows, cols, dims, step_size, data_len;
    //    char *data_loc;
    //    int *step;
    //    void *data;
    //
    //    // get CvMat header structure
    //    if (! PQgetf(select->res, _pos, "%cvmat", col, &mres)) {
    //        warning(324, "Value is not a correct cvmat type");
    //        return NULL;
    //    }
    //    // parse CvMat header fields
    //    if (! PQgetf(mres, 0, "%int4 %int4 %int4[] %int4 %int4 %name",
    //        0, &type, 1, &dims, 2, &step_arr, 3, &rows, 4, &cols, 5, &data_loc)) {
    //        warning(325, "Incorrect cvmat type");
    //        PQclear(mres);
    //        return NULL;
    //    }
    //    // sometimes data type returns with apostrophes ('type')
    //    if (data_loc && data_loc[0] == '\'') {
    //        int len = strlen(data_loc);
    //        if (data_loc[len-1] == '\'') data_loc[len-1] = '\0';
    //        data_loc++;
    //    }
    //    // construct step[] array
    //    step_size = PQntuples(step_arr.res);
    //    step = new int [step_size];
    //    for (int i = 0; i < step_size; i++) {
    //        if (! PQgetf(step_arr.res, i, "%int4", 0, &step[i])) {
    //            warning(310, "Unexpected value in int array");
    //            vt_destruct(step);
    //            PQclear(step_arr.res);
    //            PQclear(mres);
    //            return NULL;
    //        }
    //    }
    //    PQclear(step_arr.res);
    //
    //    // get matrix data from specified column
    //    int dataloc_col = PQfnumber(select->res, data_loc);
    //    int data_oid;
    //    if (dataloc_col < 0) {
    //        warning(325, "Invalid column for CvMat user data");
    //        data = NULL;
    //    }
    //    else data_oid = typeManager->getElemOID(PQftype(select->res, dataloc_col));
    //
    //    // could be char, short, int, float, double
    //    if (data_oid == typeManager->toOid("char")) {
    //        //TODO: maybe fix alignment (every row to 4B) ?
    //        data = getCharA(dataloc_col, data_len);
    //    }
    //    else if (data_oid == typeManager->toOid("float4") ||
    //            data_oid == typeManager->toOid("real")) {
    //        data = getFloatA(dataloc_col, data_len);
    //    }
    //    else {
    //        warning(326, "Unexpected type of CvMat data");
    //        data = NULL;
    //    }
    //    // create CvMat header and set user data
    //    if (dims > 0 && data && step) {
    //        mat = cvCreateMatHeader(rows, cols, type);
    //        cvSetData(mat, data, step[dims-1]);
    //    }
    //    vt_destruct(step);
    //    PQclear(mres);

    return mat;
}

// =============== GETTERS - GEOMETRIC TYPES ===============================

Point SLResultSet::getPoint(const int col)
{
    Point point = { 0.0, 0.0 };
    //    memset(&point, 0, sizeof(Point));
    //    if (! PQgetf(select->res, _pos, "%point", col, &point)) {
    //        logger->warning(314, "Value is not a point");
    //    }
    return point;
}

Point* SLResultSet::getPointA(const int col, int& size)
{
    return NULL;
}

vector<Point>*  SLResultSet::getPointV(const int col)
{
    return NULL;
}

//// =============== GETTERS - TIMESTAMP =========================================

time_t SLResultSet::getTimestamp(const int col)
{
    size_t      sl_res_i    = (SLRES->cols)*(_pos + 1) + col;
    time_t      ret         = 0;
    if (sl_res_i < 0 || sl_res_i > SLRES->cols * (SLRES->rows + 1)) return ret;
    else return toTimestamp(SLRES->res[sl_res_i]);
}

IntervalEvent *SLResultSet::getIntervalEvent(const int col)
{
    return NULL;
}

ProcessState *SLResultSet::getProcessState(const int col)
{
    return NULL;
}

// ========================= GETTERS - OTHER ==================================

void *SLResultSet::getBlob(const int col, int &size)
{
    size = 0;
    return NULL;
}

// =======================UNIVERSAL GETTER=====================================

string SLResultSet::getValue(const int col)
{
    char        *val_c  = SLRES->res[(SLRES->cols)*(_pos + 1) + col];
    string      value   = (val_c && col >= 0) ? val_c : "";

    return value;
}


}
