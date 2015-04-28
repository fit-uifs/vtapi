
#include <common/vtapi_global.h>
#include <common/vtapi_misc.h>
#include <common/vtapi_serialize.h>
#include <backends/vtapi_resultset.h>

#if HAVE_SQLITE

using std::string;
using std::stringstream;
using std::vector;
using std::pair;

using namespace vtapi;


SLResultSet::SLResultSet(const SLBackendBase &base) :
    ResultSet(NULL),
    SLBackendBase(base)
{
    thisClass = "SLResultSet";
}

SLResultSet::~SLResultSet() {
    clear();
}

void SLResultSet::newResult(void *res) {
    clear();
    this->res = res;
}

int SLResultSet::countRows() {
    return ((SLres *)this->res)->rows;
}
int SLResultSet::countCols() {
    return ((SLres *)this->res)->cols;
}
bool SLResultSet::isOk() {
    return (this->res) ? true : false;
}
void SLResultSet::clear() {
    if (this->res) {
        SLres *sl_res = (SLres *) this->res;
        sl.sqlite3_free_table(sl_res->res);
        vt_destruct(sl_res)
    }
}

TKey SLResultSet::getKey(int col) {
    SLres *sl_res = (SLres *) this->res;
    return TKey("", sl_res->res[(col*this->pos)+col], 0);
}

TKeys* SLResultSet::getKeys() {
    SLres *sl_res = (SLres *) this->res;
    TKeys* keys = new TKeys;

    for (int col = 0; col < sl_res->cols; col++) {
        keys->push_back(getKey(col));
    }
    return keys;
}

string SLResultSet::getKeyType(const int col) {
    SLres    *sl_res = (SLres *) this->res;
    char        *val_c  = sl_res->res[col];
    return (val_c) ? string(val_c) : "";
}

int SLResultSet::getKeyIndex(const string& key) {
    SLres    *sl_res = (SLres *) this->res;
    for (int col = 0; col < sl_res->cols; col++) {
        if (string(sl_res->res[col]).compare(key) == 0) {
            return col;
        }
    }
    return -1;
}

//// =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===================
char SLResultSet::getChar(const int col) {
    SLres    *sl_res     = (SLres *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return '\0';
    else return *(sl_res->res[sl_res_i]);
}

string SLResultSet::getString(const int col) {
    SLres    *sl_res     = (SLres *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return "";
    else return sl_res->res[sl_res_i];
}

//// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================

bool SLResultSet::getBool(const int col) {
    return getInt(col) > 0;
}

int SLResultSet::getInt(const int col) {
    SLres    *sl_res     = (SLres *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    int         ret         = 0;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return 0;
    else {
        stringstream(sl_res->res[sl_res_i]) >> ret;
        return ret;
    }
}

long long SLResultSet::getInt8(const int col) {
    SLres    *sl_res     = (SLres *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    long        ret         = 0;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return 0;
    else {
        stringstream(sl_res->res[sl_res_i]) >> ret;
        return ret;
    }
}

int* SLResultSet::getIntA(const int col, int& size) {
    SLres    *sl_res     = (SLres *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return NULL;
    else {
        return deserializeA<int>(sl_res->res[sl_res_i], size);
    }
}

vector<int>* SLResultSet::getIntV(const int col) {
    SLres    *sl_res     = (SLres *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return NULL;
    else {
        return deserializeV<int>(sl_res->res[sl_res_i]);
    }
}

long long* SLResultSet::getInt8A(const int col, int& size) {
    SLres *sl_res = (SLres *) this->res;
    size_t sl_res_i = (sl_res->cols)*(this->pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols * (sl_res->rows + 1)) return NULL;
    else {
        return deserializeA<long long>(sl_res->res[sl_res_i], size);
    }
}

vector<long long>* SLResultSet::getInt8V(const int col) {
    SLres *sl_res = (SLres *) this->res;
    size_t sl_res_i = (sl_res->cols)*(this->pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols * (sl_res->rows + 1)) return NULL;
    else {
        return deserializeV<long long>(sl_res->res[sl_res_i]);
    }
}

//// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================

float SLResultSet::getFloat(const int col) {
    SLres    *sl_res     = (SLres *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    char        *val_c      = sl_res->res[sl_res_i];
    float       ret         = 0;
    if (val_c && col >= 0)  stringstream (val_c) >> ret;
    return ret;
}

double SLResultSet::getFloat8(const int col) {
    SLres    *sl_res     = (SLres *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    char        *val_c      = sl_res->res[sl_res_i];
    double      ret         = 0;
    if (val_c && col >= 0)  stringstream (val_c) >> ret;
    return ret;
}

float* SLResultSet::getFloatA(const int col, int& size) {
    SLres    *sl_res     = (SLres *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return NULL;
    else {
        return deserializeA<float>(sl_res->res[sl_res_i], size);
    }
}

vector<float>* SLResultSet::getFloatV(const int col) {
    SLres    *sl_res     = (SLres *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return NULL;
    else {
        return deserializeV<float>(sl_res->res[sl_res_i]);
    }
}

double* SLResultSet::getFloat8A(const int col, int& size) {
    SLres *sl_res = (SLres *) this->res;
    size_t sl_res_i = (sl_res->cols)*(this->pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols * (sl_res->rows + 1)) return NULL;
    else {
        return deserializeA<double>(sl_res->res[sl_res_i], size);
    }
}

vector<double>* SLResultSet::getFloat8V(const int col) {
    SLres *sl_res = (SLres *) this->res;
    size_t sl_res_i = (sl_res->cols)*(this->pos + 1) + col;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols * (sl_res->rows + 1)) return NULL;
    else {
        return deserializeV<double>(sl_res->res[sl_res_i]);
    }
}

#if HAVE_OPENCV

cv::Mat *SLResultSet::getCvMat(const int col) {
    cv::Mat *mat = NULL;
//    PGresult *mres;
//    PGarray step_arr;
//    int type, rows, cols, dims, step_size, data_len;
//    char *data_loc;
//    int *step;
//    void *data;
//
//    // get CvMat header structure
//    if (! PQgetf(select->res, this->pos, "%cvmat", col, &mres)) {
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
#endif

    // =============== GETTERS - GEOMETRIC TYPES ===============================
#if HAVE_POSTGRESQL
PGpoint SLResultSet::getPoint(const int col) {
    PGpoint point = { 0.0, 0.0 };
//    memset(&point, 0, sizeof(PGpoint));
//    if (! PQgetf(select->res, this->pos, "%point", col, &point)) {
//        logger->warning(314, "Value is not a point");
//    }
    return point;
}

PGpoint* SLResultSet::getPointA(const int col, int& size) {
    return NULL;
}
vector<PGpoint>*  SLResultSet::getPointV(const int col) {
    return NULL;
}
#endif

#if HAVE_POSTGIS
GEOSGeometry* SLResultSet::getGeometry(const int col) {
    return NULL;
}
GEOSGeometry* SLResultSet::getLineString(const int col) {
    return NULL;
}
#endif

//// =============== GETTERS - TIMESTAMP =========================================

time_t SLResultSet::getTimestamp(const int col) {
    SLres    *sl_res     = (SLres *) this->res;
    size_t      sl_res_i    = (sl_res->cols)*(this->pos+1)+col;
    time_t      ret         = 0;
    if (sl_res_i < 0 || sl_res_i > sl_res->cols*(sl_res->rows+1)) return ret;
    else return toTimestamp(sl_res->res[sl_res_i]);
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

void *SLResultSet::getBlob(const int col, int &size) {
    size = 0;
    return NULL;
}

// =======================UNIVERSAL GETTER=====================================

string SLResultSet::getValue(const int col, const int arrayLimit) {
    SLres    *sl_res = (SLres *) this->res;
    char        *val_c  = sl_res->res[(sl_res->cols)*(this->pos+1)+col];
    string      value   = (val_c && col >= 0) ? val_c : "";
    size_t      comPos  = 0;

    if (arrayLimit > 0) {
        for (int lim = 0; lim < arrayLimit; lim++) {
            comPos = value.find(',', comPos);
            if (comPos == string::npos) break;
        }
        value = value.substr(0, comPos);
    }
    return value;
}



pair< TKeys*, vector<int>* > SLResultSet::getKeysWidths(const int row, bool get_widths, const int arrayLimit) {
    SLres *sl_res    = (SLres *) this->res;
    vector<int> *widths = get_widths ? new vector<int>() : NULL;
    TKeys *keys         = getKeys();

    if (!get_widths && keys) return std::make_pair(keys, widths);
    else if (!widths || !keys || sl_res->cols != keys->size() || sl_res->cols == 0 || sl_res->rows == 0) {
        vt_destruct(widths);
        vt_destruct(keys);
        return std::make_pair((TKeys*)NULL, (vector<int>*)NULL);
    }

    for (int col = 0; col < sl_res->cols; col++) {
        int max_width = 0;
        int col_width = 0;
        size_t comPos = 0;
        for (int row = 0; row < sl_res->rows+1; row++) {
            char *val_c     = sl_res->res[(sl_res->cols)*row+col];
            string value    = val_c ? val_c : "";
            if (arrayLimit > 0) {
                for (int lim = 0; lim < arrayLimit; lim++) {
                    comPos = value.find(',', comPos);
                    if (comPos == string::npos) break;
                    comPos++;
                }
                col_width = value.substr(0, comPos).length();
            }
            else {
                col_width = value.length();
            }
            comPos      = 0;
            max_width   = max_width > col_width ? max_width : col_width;
        }
        widths->push_back(max_width);
    }

    if (widths->size() != keys->size()) {
        vt_destruct(widths);
        vt_destruct(keys);
        return std::make_pair((TKeys*)NULL, (vector<int>*)NULL);
    }
    else return std::make_pair(keys, widths);
}





#endif // HAVE_POSTGRESQL

