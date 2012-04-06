/*
 * File:   keyvalues.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:52
 */

#if defined(WIN32) || defined(WIN64)
#include <Winsock2.h>
#else
#include <netinet/in.h>
#endif

#include "vtapi.h"

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string.h>

// ************************************************************************** //
String TKey::print() {
    String ret = "TKey type=" + type + ", key=" + key + ", size=" + toString(size) + ", from=" + from;
    std::cout << ret << std::endl;
    return (ret);
}


// FIXME: tohle kdyz dam sem, tak je to v haji - proc?
// template <class T>
// String TKeyValue<T>::print() {};
// ************************************************************************** //

KeyValues::KeyValues(const Commons& orig) 
          : Commons(orig), select(NULL), pos(-1), insert(NULL), update(NULL) {
    thisClass = "KeyValues(Commons&)";
}

KeyValues::KeyValues(const KeyValues& orig, const String& selection)
          : Commons(orig), select(NULL), pos(-1), insert(NULL), update(NULL) {
    thisClass = "KeyValues(KeyValues&)";
    this->selection = selection;
}


// TODO: decide if this behavior (warnings raised when not executed) is OK for most of us
KeyValues::~KeyValues() {
    // whether should be something inserted
    if (insert) {
        if (!insert->executed) warning(313, "There should be something inserted: \n" + insert->getQuery());
        destruct(insert);
    }

    // whether should be something updated
    if (update) {
        if (!update->executed) warning(314, "There should be something updated: \n" + insert->getQuery());
        destruct(update);
    }

    destruct (select);

    this->beDoomed();
}

KeyValues* KeyValues::next() {
    // whether should be something inserted
    if (insert) {
        if (!insert->executed) insert->execute();  // FIXME: here should be the store fun instead
        destruct(insert);
    }

    // whether should be something updated
    if (update) {
        if (!update->executed) update->execute();  // FIXME: here should be the store fun instead
        destruct(update);
    }

    // check the Select, each subclass is responsible of
    if (!select) error(301, "There is no select class");

    // select is executed here when position == -1
    if (pos == -1) {
        if (select->res) {
            PQclear(select->res);
            select->res = NULL;
        }
        select->execute();
    }

    // check if end of resultset has been reached
    if (select->res) {
        int rows = PQntuples(select->res);
        if (rows <= 0) return NULL;
        else if (pos < rows) pos++;      // continue
        else if (pos >= select->limit) {     // fetch new resultset
            if (select->executeNext()) {
                pos = 0;
                return this;
            }
        }
        else return NULL;   // end of result        
    }

    return NULL;
}

TKey KeyValues::getKey(int col) {
    TKey column;
    if (!select || !select->res) return column;
    if (col >= PQnfields(select->res)) return column;

    column.key = PQfname(select->res, col);
    column.type = typemap->toTypname(PQftype(select->res, col));
    column.size = 1;
    return column;
}

std::vector<TKey>* KeyValues::getKeys() {
    std::vector<TKey>* vtk = new std::vector<TKey>;
    if (!select || !select->res) return NULL;

    int cols = PQnfields(select->res);
    for (int c = 0; c < cols; c++) {
        String name = PQfname(select->res, c);
        String type = typemap->toTypname(PQftype(select->res, c));
        TKey tk (type, name, -1);
        vtk->push_back(tk);
    }

    return vtk;
}


// =============== GETTERS (Select) ============================================
//TODO: optimalize getters, keys and metadata are sometimes retrieved twice

/**
 * Generic getter - fetches any value from resultset and returns it as string
 * @param col column index
 * @return String representation of field value
 */
String KeyValues::getValue(const int col) {

    std::stringstream valss; //return stringstream
    TKey colkey = getKey(col); // get column type via its key
    if (colkey.size < 0) return "";
    // get type metadata (its category, length in bytes and possibly array type)
    char typcategory = typemap->getCategory(colkey.type);
    short typlen = typemap->getLength(colkey.type);
     //if array, this shows element type
    int typelemoid = typcategory == 'A' ? typemap->getElemOID(colkey.type) : -1;
    int precision = 4;

    // Call different getters for different categories of types
    switch (typcategory) {
        case 'A': { // array
            //TODO: arrays of other types than int4 and float4
            if (typelemoid == typemap->toOid("int4")) {
                std::vector<int>* arr = this->getIntV(col);
                if (arr) for (int i = 0; i < (*arr).size(); i++) {
                    valss << (*arr)[i];
                    if (arrayLimit && i == arrayLimit) {
                        valss << "...";
                        break;
                    }
                    if (i < (*arr).size()-1) valss << ",";
                }
                destruct (arr);
            }
            else if (typelemoid == typemap->toOid("float4")) {
                std::vector<float>* arr = this->getFloatV(col);
                if (arr) for (int i = 0; i < (*arr).size(); i++) {
                    valss << (*arr)[i];
                    if (arrayLimit && i == arrayLimit) {
                        valss << "...";
                        break;
                    }
                    if (i < (*arr).size()-1) valss << ",";
                }
                destruct (arr);
            }
            else if (typelemoid == typemap->toOid("char")) {
                int arr_size;
                char *arr = this->getCharA(col, arr_size);
                for (int i = 0; i < arr_size; i++) valss << arr[i];
                destruct (arr);
            }
            } break;
        case 'B': { // boolean
            } break;
        case 'C': { // composite
#ifdef __OPENCV_CORE_C_H__
            if (!colkey.type.compare("cvmat")) {
                CvMat *mat = getCvMat(col);
                valss << cvGetElemType(mat);
                cvReleaseMat(&mat);
            }
#endif
            } break;

        case 'D': { // date/time
            struct tm ts = getTimestamp(col);
            valss << std::right << std::setfill('0');
            //TODO: microseconds?
            if (ts.tm_year > 0)
                valss << std::setw(4) << ts.tm_year << '-' << std::setw(2) << ts.tm_mon <<
                  '-' << std::setw(2) << ts.tm_mday << ' ' << std::setw(2) << ts.tm_hour <<
                  ':' << std::setw(2) << ts.tm_min  << ':' << std::setw(2) << ts.tm_sec;
            } break;

        case 'E': { // enum
            valss << getString(col); // 'name' data type is numeric
            } break;
        case 'G': { // geometric
            if (!colkey.type.compare("point")) {
                PGpoint point = getPoint(col);
                valss << point.x << " , " << point.y;
            }
            else if (!colkey.type.compare("box")) {
                PGbox box = getBox(col);
                valss << '(' << box.low.x << " , " << box.low.y << ") , ";
                valss << '(' << box.high.x << " , " << box.high.y << ')';
            }
            else if (!colkey.type.compare("lseg")) {
                PGlseg lseg = getLineSegment(col);
                valss << '(' << lseg.pts[0].x << " , " << lseg.pts[0].y << ") ";
                valss << '(' << lseg.pts[1].x << " , " << lseg.pts[1].y << ')';
            }
            else if (!colkey.type.compare("circle")) {
                PGcircle circle = getCircle(col);
                valss << '(' << circle.center.x << " , " << circle.center.y;
                valss << ") , " << circle.radius;
            }
            else if (!colkey.type.compare("path")) {
                PGpath path = getPath(col);
                for (int i = 0; i < path.npts; i++) {
                    valss << '(' << path.pts[i].x << " , " << path.pts[i].y << ')';
                    if (arrayLimit && i == arrayLimit) {
                        valss << "...";
                        break;
                    }
                    if (i < path.npts-1) valss << " , ";
                }
            }
            else if (!colkey.type.compare("polygon")) {
                PGpolygon polygon = getPolygon(col);
                for (int i = 0; i < polygon.npts; i++) {
                    valss << '(' << polygon.pts[i].x << " , " << polygon.pts[i].y << ')';
                    if (arrayLimit && i == arrayLimit) {
                        valss << "...";
                        break;
                    }
                    if (i < polygon.npts-1) valss << " , ";
                }
            }
            } break;
        case 'I': { // network address
            } break;

        case 'N': { // numeric
            // this detects if type is oid reference (regtype, regclass...)
            // if (typemap->isRefType(colkey.type)) {}
            if (!colkey.type.substr(0,5).compare("float"))
                typlen < 8 ? valss << getFloat(col) : valss << getFloat8(col);
            else
                typlen < 8 ? valss << getInt(col) : valss << getInt8(col);
            } break;
        case 'P': { // pseudo
            } break;

        case 'S': { // string
            if (typlen == 1) valss << getChar(col); // char has length 1
            else return getString(col);
            } break;
        case 'T': { // timespan
            } break;

        case 'U': { // user-defined (cube + postGIS types!!)
            if (!colkey.type.compare("geometry")) { // postGIS geometry type
                GEOSGeometry *geo;
                GEOSWKTWriter *geo_writer;
                char * geo_string;

                if (!(geo = getGeometry(col))) break;
                if (! (geo_writer  =  GEOSWKTWriter_create())) {
                    GEOSGeom_destroy(geo);
                    break;
                }

                //TODO: 2.2 zrejme neumi
                //GEOSWKTWriter_setRoundingPrecision(geo_writer, precision);
                geo_string = GEOSWKTWriter_write(geo_writer, geo);
                valss << geo_string;
                
                GEOSFree(geo_string);
                GEOSGeom_destroy(geo);
            }
            else if (!colkey.type.compare("cube")) { // cube type
                PGcube cube = getCube(col);
                int lim = cube.dim;
                if (arrayLimit && (2 * lim > arrayLimit)) lim = arrayLimit/2;
                if (cube.x && cube.dim > 0) {
                    valss << '(';
                    for (int i = 0; i < lim; i++) {
                        valss << cube.x[i];
                        if (i < lim-1) valss << ',';
                    }
                    valss << ')';
                    if (cube.x[lim]) {
                        valss << ",(";
                        for (int i = lim; i < 2 * lim; i++) {
                            valss << cube.x[i];
                            if (i < lim-1) valss << ',';
                        }
                    }
                    valss << ')';
                }
            }
            } break;
        case 'V': { // bit-string
        } break;
        case 'X': { // unknown
        } break;
        default: { // undefined
        } break;
    }
    return valss.str();    
}

// =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===================
char KeyValues::getChar(const String& key) {
    return this->getChar(PQfnumber(select->res, key.c_str()));
}
char KeyValues::getChar(const int col) {
    PGchar value = '\0';
    int ret = PQgetf(select->res, this->pos, "%char", col, &value);
    if (ret == 0) {
        warning(304, "Value is not an char");
        return '\0';
    }
    else return value;
}

char *KeyValues::getCharA(const String& key, int& size) {
    return this->getCharA(PQfnumber(select->res, key.c_str()), size);
}

char *KeyValues::getCharA(const int col, int& size) {
    PGarray tmp;
    if (! PQgetf(select->res, this->pos, "%char[]", col, &tmp)) {
        warning(304, "Value is not an array of chars");
        size = -1;
        return NULL;
    }

    size = PQntuples(tmp.res);
    char* values = new char [size];
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%char", 0, &values[i])) {
            warning(304, "Unexpected value in char array");
            size = -1;
            PQclear(tmp.res);
            destruct (values);
            return NULL;
        }
    }
    PQclear(tmp.res);

    return values;
}

String KeyValues::getString(const String& key) {
    return this->getString(PQfnumber(select->res, key.c_str()));
}

String KeyValues::getString(const int col) {
    PGtext value = (PGtext) "";
    TKey colkey = getKey(col);
    if (!select->res) return "";

    if (typemap->getCategory(colkey.type) == 'S')  // string types
        value = PQgetvalue(select->res, pos, col);
    else if (typemap->isEnumType(colkey.type))     // enum types
        value = PQgetvalue(select->res, pos, col);
    else if(typemap->isRefType(colkey.type)) {  // reference types (regtype, regclass..)
    }
    else {
        std::stringstream wss;
        wss << "Value of type " << colkey.type << " isn't a string";
        warning(305, wss.str());
    }
    return value ? String(value) : String("");
}

// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================
int KeyValues::getInt(const String& key) {
    return this->getInt(PQfnumber(select->res, key.c_str()));
}

int KeyValues::getInt(const int col) {
    int value = 0;
    short length = typemap->getLength(getKey(col).type);

    if (length < 0) { // conversion if length == -1
        std::stringstream iss (PQgetvalue(select->res, pos, col));
        iss >> value;
    }
    else if (length == 2) // short int
        value = ntohl(*(PGint2 *)PQgetvalue(select->res, pos, col));
    else if (length == 4) // int
        value = ntohl(*(PGint4 *)PQgetvalue(select->res, pos, col));
    else {
        std::stringstream wss;
        if (length == 8) wss << "Use getInt8(col) to fetch int8 values.";
        else wss << "Integer value of length " << length << " is not supported";
        warning(306, wss.str());
    }    
    return value;
}

long KeyValues::getInt8(const String& key) {
    return this->getInt8(PQfnumber(select->res, key.c_str()));
}

long KeyValues::getInt8(const int col) {
    long value = 0;
    short length = typemap->getLength(getKey(col).type);
        // extract long or call getInt if value is integer
    if (length == 8) value = ntohl(*(PGint8 *)PQgetvalue(select->res, pos, col));
    else if (length < 8) value = (long) getInt(col);
    else {
        std::stringstream wss;
        wss << "Integer value of length " << length << " is not supported";
        warning(306, wss.str());
    }
    return value;
}


int* KeyValues::getIntA(const String& key, int& size) {
    return this->getIntA(PQfnumber(select->res, key.c_str()), size);
}
int* KeyValues::getIntA(const int col, int& size) {
    PGarray tmp;
    if (! PQgetf(select->res, this->pos, "%int4[]", col, &tmp)) {
        warning(307, "Value is not an array of integer");
        size = -1;
        return NULL;
    }

    size = PQntuples(tmp.res);
    int* values = new int [size];
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &values[i])) {
            warning(308, "Unexpected value in integer array");
            size = -1;
            PQclear(tmp.res);
            destruct (values);
            return NULL;
        }
    }
    PQclear(tmp.res);

    return values;
}

std::vector<int>* KeyValues::getIntV(const String& key) {
    return this->getIntV(PQfnumber(select->res, key.c_str()));
}
std::vector<int>* KeyValues::getIntV(const int col) {
    PGarray tmp;
    if (! PQgetf(select->res, this->pos, "%int4[]", col, &tmp)) {
        warning(307, "Value is not an array of integer");
        return NULL;
    }

    PGint4 value;
    std::vector<int>* values = new std::vector<int>;

    for (int i = 0; i < PQntuples(tmp.res); i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &value)) {
            warning(308, "Unexpected value in integer array");
            PQclear(tmp.res);
            destruct (values);
            return NULL;
        }
        values->push_back(value);
    }
    PQclear(tmp.res);

    return values;
}

// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================
float KeyValues::getFloat(const String& key) {
    return this->getFloat(PQfnumber(select->res, key.c_str()));
}

float KeyValues::getFloat(const int col) {
    float value = 0;
    short length = typemap->getLength(getKey(col).type);
    if (length < 0) { // conversion if length == -1
        std::stringstream iss (PQgetvalue(select->res, pos, col));
        iss >> value;
    }
    else if (length == 4) {
        value = ntohl(*(PGfloat4 *)PQgetvalue(select->res, pos, col));
        std::cout << value << std::endl;
    }
    else {
        std::stringstream wss;
        if (length == 8) wss << "Use getFloat8(col) to fetch float8 values.";
        else wss << "Float value of length " << length << " is not supported";
        warning(309, wss.str());
    }
    return value;
}

double KeyValues::getFloat8(const String& key) {
    return this->getFloat8(PQfnumber(select->res, key.c_str()));
}

double KeyValues::getFloat8(const int col) {
    double value = 0;
    short length = typemap->getLength(getKey(col).type);

    if (length == 8) value = ntohl(*(PGfloat8 *)PQgetvalue(select->res, pos, col));
    else if (length < 8) value = (double) getFloat(col);
    else {
        std::stringstream wss;
        wss << "Float value of length " << length << " is not supported";
        warning(309, wss.str());
    }
    return value;
}

float* KeyValues::getFloatA(const String& key, int& size) {
    int pos = PQfnumber(select->res, String("\"" + key + "\"").c_str());
    if (pos < 0) warning(310, "Column " + toString(key) + " doesn't exist.");
    return this->getFloatA(pos, size);
}
float* KeyValues::getFloatA(const int col, int& size) {
    PGarray tmp;
    if (! PQgetf(select->res, this->pos, "%float4[]", col, &tmp)) {
        warning(311, "Value is not an array of float");
        size = -1;
        return NULL;
    }

    size = PQntuples(tmp.res);
    float* values = new float [size];
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%float4", 0, &values[i])) {
            warning(312, "Unexpected value in float array");
            size = -1;
            PQclear(tmp.res);
            destruct (values);
            return NULL;
        }
    }
    PQclear(tmp.res);

    return values;
}

std::vector<float>* KeyValues::getFloatV(const String& key) {
    return this->getFloatV(PQfnumber(select->res, key.c_str()));
}
std::vector<float>* KeyValues::getFloatV(const int col) {
    PGarray tmp;
    if (! PQgetf(select->res, this->pos, "%float4[]", col, &tmp)) {
        warning(311, "Value is not an array of float");
        return NULL;
    }

    PGfloat4 value;
    std::vector<float>* values = new std::vector<float>;
    for (int i = 0; i < PQntuples(tmp.res); i++) {
        if (! PQgetf(tmp.res, i, "%float4", 0, &value)) {
            warning(312, "Unexpected value in float array");
            PQclear(tmp.res);
            destruct (values);
            return NULL;
        }
        values->push_back(value);
    }
    PQclear(tmp.res);

    return values;
}
// =============== GETTERS - OpenCV MATRICES ===============================
#ifdef __OPENCV_CORE_C_H__

CvMat *KeyValues::getCvMat(const String& key) {
    return this->getCvMat(PQfnumber(select->res, key.c_str()));
}

CvMat *KeyValues::getCvMat(const int col) {
    CvMat *mat = NULL;
    PGresult *mres;
    PGarray step_arr;
    int type, rows, cols, dims, step_size, data_len;
    char *data_loc;
    int *step;
    void *data;

    // get CvMat header structure
    if (! PQgetf(select->res, this->pos, "%cvmat", col, &mres)) {
        warning(324, "Value is not a correct cvmat type");
        return NULL;
    }

    // parse CvMat header fields
    if (! PQgetf(mres, 0, "%int4 %int4 %int4[] %int4 %int4 %name",
        0, &type, 1, &dims, 2, &step_arr, 3, &rows, 4, &cols, 5, &data_loc)) {
        warning(325, "Incorrect cvmat type");
        PQclear(mres);
        return NULL;
    }
    // sometimes data type returns with apostrophes ('type')
    if (data_loc && data_loc[0] == '\'') {
        int len = strlen(data_loc);
        if (data_loc[len-1] == '\'') data_loc[len-1] = '\0';
        data_loc++;
    }

    // construct step[] array
    step_size = PQntuples(step_arr.res);
    step = new int [step_size];
    for (int i = 0; i < step_size; i++) {
        if (! PQgetf(step_arr.res, i, "%int4", 0, &step[i])) {
            warning(310, "Unexpected value in int array");
            destruct (step);
            PQclear(step_arr.res);
            PQclear(mres);
            return NULL;
        }
    }
    PQclear(step_arr.res);

    // get matrix data from specified column
    int dataloc_col = PQfnumber(select->res, data_loc);
    int data_oid;
    if (dataloc_col < 0) {
        warning(325, "Invalid column for CvMat user data");
        data = NULL;
    }
    else data_oid = typemap->getElemOID(PQftype(select->res, dataloc_col));

    // could be char, short, int, float, double
    if (data_oid == typemap->toOid("char")) {
        //TODO: maybe fix alignment (every row to 4B) ?
        data = getCharA(dataloc_col, data_len);
    }
    else if (data_oid == typemap->toOid("float4") ||
            data_oid == typemap->toOid("real")) {
        data = getFloatA(dataloc_col, data_len);
    }
    else {
        warning(326, "Unexpected type of CvMat data");
        data = NULL;
    }

    // create CvMat header and set user data
    if (dims > 0 && data && step) {
        mat = cvCreateMatHeader(rows, cols, type);
        cvSetData(mat, data, step[dims-1]);
    }

    destruct (step);
    PQclear(mres);

    return mat;
}

CvMatND *KeyValues::getCvMatND(const String& key) {
    return this->getCvMatND(PQfnumber(select->res, key.c_str()));
}

CvMatND *KeyValues::getCvMatND(const int col) {
    CvMatND *mat = NULL;
    PGresult *mres;
    PGarray step_arr;
    int type, rows, cols, dims, step_size, data_len;
    char *data_loc;
    int *step, *sizes;
    void *data;

    // get CvMat header structure
    if (! PQgetf(select->res, this->pos, "%cvmat", col, &mres)) {
        warning(324, "Value is not a correct cvmat type");
        return NULL;
    }

    // parse CvMat header fields
    if (! PQgetf(mres, 0, "%int4 %int4 %int4[] %int4 %int4 %name",
        0, &type, 1, &dims, 2, &step_arr, 3, &rows, 4, &cols, 5, &data_loc)) {
        warning(325, "Incorrect cvmat type");
        PQclear(mres);
        return NULL;
    }
    // sometimes data type returns with apostrophes ('type')
    if (data_loc && data_loc[0] == '\'') {
        int len = strlen(data_loc);
        if (data_loc[len-1] == '\'') data_loc[len-1] = '\0';
        data_loc++;
    }

    // construct step[] array
    step_size = PQntuples(step_arr.res);
    step = new int [step_size];
    sizes = new int [step_size];
    for (int i = 0; i < step_size; i++) {
        if (! PQgetf(step_arr.res, i, "%int4", 0, &step[i])) {
            warning(310, "Unexpected value in int array");
            destruct (step);
            destruct (sizes);
            PQclear(step_arr.res);
            PQclear(mres);
            return NULL;
        }
    }
    PQclear(step_arr.res);

    // get matrix data from specified column
    int dataloc_col = PQfnumber(select->res, data_loc);
    int data_oid = -1;
    if (dataloc_col < 0) {
        warning(325, "Invalid column for CvMat user data");
        data = NULL;
    }
    else data_oid = typemap->getElemOID(PQftype(select->res, dataloc_col));

    // could be char, short, int, float, double
    if (data_oid == typemap->toOid("char")) {
        //TODO: maybe fix alignment (every row to 4B) ?
        //TODO: not sure if sizes are counted correctly
        data = getCharA(dataloc_col, data_len);
        for (int i = 0; i < step_size; i++)
            sizes[i] = data_len / step[i];
    }
    else if (data_oid == typemap->toOid("float4") ||
            data_oid == typemap->toOid("real")) {
        //TODO: not sure if sizes are counted correctly
        data = getFloatA(dataloc_col, data_len);
        for (int i = 0; i < step_size; i++)
            sizes[i] = (data_len * sizeof(float)) / step[i];
    }
    else {
        warning(326, "Unexpected type of CvMat data");
        data = NULL;
    }

    // create CvMatND header and set user data
    if (dims > 0 && data && sizes && step) {
        mat = cvCreateMatNDHeader(dims, sizes, type);
        cvSetData(mat, data, step[dims-1]);
    }

    destruct (step);
    PQclear(mres);

    return mat;
}
#endif
// =============== GETTERS - TIMESTAMP =========================================
struct tm KeyValues::getTimestamp(const String& key) {
    return this->getTimestamp(PQfnumber(select->res, key.c_str()));
}
struct tm KeyValues::getTimestamp(const int col) {
    struct tm ts = {0};
    String dtype = getKey(col).type;

    if (!dtype.compare("time")) {
        PGtime timestamp;
        PQgetf(select->res, this->pos, "%time", col, &timestamp);
        ts.tm_hour  = timestamp.hour;
        ts.tm_min   = timestamp.min;
        ts.tm_sec   = timestamp.sec; 
    }
    else if (!dtype.compare("timestamp")) {
        PGtimestamp timestamp;
        PQgetf(select->res, this->pos, "%timestamp", col, &timestamp);
        ts.tm_year  = timestamp.date.year;
        ts.tm_mon   = timestamp.date.mon;
        ts.tm_mday  = timestamp.date.mday;
        ts.tm_hour  = timestamp.time.hour;
        ts.tm_min   = timestamp.time.min;
        ts.tm_sec   = timestamp.time.sec;
    }
    else {
        std::stringstream wss;
        wss << "Data type " << dtype << " not yet supported";
        warning(313, wss.str());
    }
    return ts;
}

// =============== GETTERS - GEOMETRIC TYPES ===============================
PGpoint KeyValues::getPoint(const String& key) {
    return this->getPoint(PQfnumber(select->res, key.c_str()));
}
PGpoint KeyValues::getPoint(const int col) {
    PGpoint point;
    memset(&point, 0, sizeof(PGpoint));
    if (! PQgetf(select->res, this->pos, "%point", col, &point)) {
        warning(314, "Value is not a point");
    }
    return point;
}
PGlseg KeyValues::getLineSegment(const String& key) {
    return this->getLineSegment(PQfnumber(select->res, key.c_str()));
}
PGlseg KeyValues::getLineSegment(const int col){
    PGlseg lseg;
    memset(&lseg, 0, sizeof(PGlseg));
    if (! PQgetf(select->res, this->pos, "%lseg", col, &lseg)) {
        warning(315, "Value is not a line segment");
    }
    return lseg;
}

PGbox KeyValues::getBox(const String& key){
    return this->getBox(PQfnumber(select->res, key.c_str()));
}
PGbox KeyValues::getBox(const int col){
    PGbox box;
    memset(&box, 0, sizeof(PGbox));
    if (! PQgetf(select->res, this->pos, "%box", col, &box)) {
        warning(316, "Value is not a box");
    }
    return box;
}

PGcircle KeyValues::getCircle(const String& key){
    return this->getCircle(PQfnumber(select->res, key.c_str()));

}
PGcircle KeyValues::getCircle(const int col){
    PGcircle circle;
    memset(&circle, 0, sizeof(PGcircle));
    if (! PQgetf(select->res, this->pos, "%circle", col, &circle)) {
        warning(317, "Value is not a circle");
    }
    return circle;
}

PGpolygon KeyValues::getPolygon(const String& key){
    return this->getPolygon(PQfnumber(select->res, key.c_str()));

}
PGpolygon KeyValues::getPolygon(const int col){
    PGpolygon polygon;
    memset(&polygon, 0, sizeof(PGpolygon));
    if (! PQgetf(select->res, this->pos, "%polygon", col, &polygon)) {
        warning(318, "Value is not a polygon");
    }
    return polygon;
}

PGpath KeyValues::getPath(const String& key){
    return this->getPath(PQfnumber(select->res, key.c_str()));

}
PGpath KeyValues::getPath(const int col){
    PGpath path;
    memset(&path, 0, sizeof(PGpath));
    if (! PQgetf(select->res, this->pos, "%path", col, &path)) {
        warning(319, "Value is not a path");
    }
    return path;
}

PGcube KeyValues::getCube(const String& key) {
    return this->getCube(PQfnumber(select->res, key.c_str()));
}
PGcube KeyValues::getCube(const int col) {
    PGcube cube;
    memset(&cube, 0, sizeof(PGcube));
    if (! PQgetf(select->res, this->pos, "%cube", col, &cube)) {
        warning(320, "Value is not a cube");
    }
    return cube;
}

GEOSGeometry *KeyValues::getGeometry(const String& key) {
    return this->getGeometry(PQfnumber(select->res, key.c_str()));
}
GEOSGeometry *KeyValues::getGeometry(const int col) {
    GEOSGeometry *geo;
    if (! PQgetf(select->res, this->pos, "%geometry", col, &geo)) {
        warning(321, "Value is not a geometry type");
    }
    return geo;
}

GEOSGeometry *KeyValues::getLineString(const String& key) {
    return this->getLineString(PQfnumber(select->res, key.c_str()));
}
GEOSGeometry *KeyValues::getLineString(const int col) {
    GEOSGeometry *ls;
    if (! PQgetf(select->res, this->pos, "%geometry", col, &ls)) {
        warning(322, "Value is not a geometry type");
    }
    else if (ls && GEOSGeomTypeId(ls) != GEOS_LINESTRING) {
        warning(323, "Value is not a linestring");
    }
    return ls;
}

// =============== GETTERS - OTHER =============================================
int KeyValues::getIntOid(const String& key) {
    PGint4 value;

    PQgetf(select->res, this->pos, "#oid", key.c_str(), &value);

    return (int) value;
}

// =============== SETTERS (Update) ============================================
// TODO: mozna by se dalo premyslet o PQsetvalue

bool KeyValues::preSet() {
    // TODO: tohle by se v budoucnu melo dat za pomoci system_catalog
    warning(3010, "Set unimplemented at class " + thisClass);
    return false;
}

// TODO: how to change binary data???
bool KeyValues::setString(const String& key, const String& value) {
    // call preset on the derived class
    if (!update) this->preSet();

    /* TODO: how to change the res in case of binary data???
    if (select) {
        // TODO: what to call in the case of binary data???
        char* tempc = const_cast<char*>(value.c_str()); // stupid C conversions
        PQsetvalue(select->res, pos, PQfnumber(select->res, key.c_str()), tempc, value.length());
    } */

    update->keyString(key, value);
}

bool KeyValues::setInt(const String& key, int value) {
    // call preset on the derived class
    if (!update) this->preSet();
    update->keyInt(key, value);
}

bool KeyValues::setIntA(const String& key, const int* values, int size){
    // call preset on the derived class
    if (!update) this->preSet();
    update->keyIntA(key, values, size);
}

bool KeyValues::setFloat(const String& key, float value){
    // call preset on the derived class
    if (!update) this->preSet();
    update->keyFloat(key, value);
}

bool KeyValues::setFloatA(const String& key, const float* values, int size){
    // call preset on the derived class
    if (!update) this->preSet();
    update->keyFloatA(key, values, size);
}

bool KeyValues::setExecute() {
    if (this->update) return this->update->execute();
    else return false;
}


// =================== ADDERS (Insert) =========================================
bool KeyValues::addExecute() {
    if (this->insert) return this->insert->execute();
    else return false;
}

// =================== CHECKERS (Storage) ======================================
bool KeyValues::checkStorage() {
    warning(3018, "Check might fail at class " + thisClass);

    if (!sequence.empty() && fileExists(getDataLocation())) return true;
    else if (!dataset.empty() && fileExists(getDataLocation())) return true;

    return false;
}










// =============== PRINT methods =======================================

/**
 * Prints currently selected row in resultset (Select)
 */
void KeyValues::print() {
    if (!select || !select->res || pos < 0)
        warning(302, "There is nothing to print (see other messages)");
    else {
        int origpos = this->pos;
        int get_widths = (format == STANDARD);
        std::pair< std::vector<TKey>*, std::vector<int>* > fInfo = getFieldsInfo(pos, get_widths);
        if (fInfo.first && (format != STANDARD || fInfo.second)) {
            printHeader(fInfo);
            printRowOnly(pos, fInfo.second);
            printFooter(1);
        }
        else std::cout << "(no output)" << std::endl;
        destruct(fInfo.first); destruct(fInfo.second);
        this->pos = origpos;
    }
}
/**
 * Prints all rows in resultset (Select)
 */
void KeyValues::printAll() {
    if (!select || !select->res)
        warning(303, "There is nothing to print (see other messages)");
    else {
        int origpos = this->pos;
        int get_widths = (format == STANDARD);
        std::pair< std::vector<TKey>*, std::vector<int>* > fInfo = getFieldsInfo(-1, get_widths);
        if (fInfo.first && (format != STANDARD || fInfo.second)) {
            printHeader(fInfo);
            for (int r = 0; r < PQntuples(select->res); r++) printRowOnly(r, fInfo.second);
            printFooter();
        }
        else std::cout << "(no output)" << std::endl;
        destruct(fInfo.first); destruct(fInfo.second);
        this->pos = origpos;
    }
}

// =============== PRINT support methods =======================================
/**
 * Prints header - field name and data type
 * @param res Input resultset
 * @param fInfo Column types and widths
 */
void KeyValues::printHeader(const std::pair< std::vector<TKey>*, std::vector<int>* > fInfo) {
    std::stringstream table, nameln, typeln, border;
    int cols = PQnfields(select->res);

    if (format == HTML) {
        if (tableOpt.empty()) table << "<table>";
        else table << "<table " << tableOpt << ">";
        if (!caption.empty()) table << "<caption align=\"top\">" << caption << "</caption>";
        table << std::endl << "<tr align=\"center\">";
    }
    for (int c = 0; c < cols; c++) {
        if (format == STANDARD) {
            nameln << std::left << std::setw((*fInfo.second)[c]) << (*fInfo.first)[c].key;
            typeln << std::left << std::setw((*fInfo.second)[c]) << (*fInfo.first)[c].type;
            border << std::setfill('-') << std::setw((*fInfo.second)[c]) << "";
            if (c < cols-1) {
                nameln << '|'; typeln << '|'; border << '+';
            }
        }
        else if (format == CSV) {
            nameln << (*fInfo.first)[c].key;
            if (c < cols-1) nameln << ',';
        }
        else if (format == HTML) {
            table << "<th>" << (*fInfo.first)[c].key << "<br/>";
            table << (*fInfo.first)[c].type << "</th>";
        }
    }
    table << "</tr>" << std::endl; nameln << std::endl; typeln << std::endl; border << std::endl;
    if (format == STANDARD) std::cout << nameln.str() << typeln.str() << border.str();
    else if (format == CSV) std::cout << nameln.str();
    else if (format == HTML) std::cout << table.str();
}
/**
 * Prints footer - number of rows printed
 * @param res Input resultset
 * @param count Number of rows printed (0 = all)
 */
void KeyValues::printFooter(const int count) {
    std::stringstream output;
    if (format == STANDARD) {
        int rows = PQntuples(select->res);
        if (count > 0) output << "(" << count << " of " << rows << " rows)" << std::endl;
        else output << "(" << rows << " rows)" << std::endl;
    }
    else if (format == HTML) output << "</table>" << std::endl;
    std::cout << output.str();
}
/**
 * Prints values in single row
 * @param res Input resultset
 * @param row Row number
 * @param widths Vector of column widths
 */
void KeyValues::printRowOnly(const int row, const std::vector<int>* widths) {
    std::stringstream output;
    int cols = PQnfields(select->res);

    //for (int i = 0; i < cols-(widths->size()); i++) widths->push_back(0);
    // don't forget to save original value of pos beforehand!
    this->pos = row;
    if (format == HTML) output << "<tr>";
    for (int c = 0; c < cols; c++) {
        // obecny getter
        String val = getValue(c);
        if (format == STANDARD) {
            output << std::left << std::setw((*widths)[c]) << val;
            if (c < cols-1) output << '|';
        }
        else if (format == CSV) {
            if (val.find(',') != String::npos) output << "\"" << val << "\"";
            //TODO: escape quotation marks ""
            else output << val;
            if (c < cols-1) output << ',';
        }
        else if (format == HTML) {
            output << "<td>" << val << "</td>";
        }
    }
    if (format == HTML) output << "</tr>";
    output << std::endl;
    std::cout << output.str();
}
/**
 * Returns data types, field names and desired column widths for all fields
 * @param res Input resultset
 * @param row Row number to process (0 = all)
 * @return Pair of vectors <TKey,widths> (TKey includes data type and field name)
 */
std::pair< std::vector<TKey>*, std::vector<int>* > KeyValues::getFieldsInfo(const int row, int get_widths) {
    int plen, flen, tlen, width;
    std::vector<int> *widths = get_widths ? new std::vector<int>() : NULL;
    std::vector<TKey> *keys = getKeys();
    int rows = PQntuples(select->res);
    int cols = PQnfields(select->res);

    if (!get_widths && keys) return std::make_pair(keys, widths);
    else if (!widths || !keys || cols!=keys->size() || cols == 0 || rows == 0) {
        destruct(widths);
        destruct(keys);
        return std::make_pair((std::vector<TKey>*)NULL, (std::vector<int>*)NULL);
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
        destruct(widths);
        destruct(keys);
        return std::make_pair((std::vector<TKey>*)NULL, (std::vector<int>*)NULL);
    }
    else return std::make_pair(keys, widths);
}
