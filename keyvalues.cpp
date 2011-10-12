/*
 * File:   keyvalues.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:52
 */

#include "VTApi.h"
#include "postgresql/libpqtypes.h"
#include <cstdlib>
#include <iostream>

using namespace std;



KeyValues::KeyValues(const Commons& orig) : Commons(orig),
        position(-1), res(NULL), parent(NULL) {
}

KeyValues::KeyValues(const KeyValues& orig) : Commons(orig),
        position(-1), res(NULL), parent(NULL) {
        // FIXME: tohle taky ne: position(orig.position), res(orig.res), parent(orig.parent) {
}

KeyValues::~KeyValues() {
}



KeyValues* KeyValues::next() {
    // TODO: zatim to skonci po konci resultsetu, ale melo by zjistit, jestli je
    // to na konci nebo neni a spachat kdyztak dalsi dotaz (limit, offset)
    if (res && position < PQntuples(res)) position++;

}



long KeyValues::getRowActual() {

}

long KeyValues::getRowNumber() {

}



/**
 * Get string value specified by column key
 * @param key column key
 * @return string value
 */
String KeyValues::getString(String key) {
    PGtext value;

    // Several data types are other representation of string, so we must catch all of them
    if (PQgetf(res, position, "#text", key.c_str(), &value)) {}
    else if (PQgetf(res, position, "#name", key.c_str(), &value)) {}
    else if (PQgetf(res, position, "#varchar", key.c_str(), &value)) {}
    else if (PQgetf(res, position, "#varchar", key.c_str(), &value)) {}
    else {
        cerr << PQgeterror() << endl;
    }
    
    return (String) value;
}

/**
 * Get string value spevified by index of column
 * @param pos index of column
 * @return string value
 */
String KeyValues::getString(int pos) {
    PGtext value;

    if (PQgetf(res, position, "%text", pos, &value)) {}
    else if (PQgetf(res, position, "%name", pos, &value)) {}
    else if (PQgetf(res, position, "%varchar", pos, &value)) {}
    else if (PQgetf(res, position, "%varchar", pos, &value)) {}
    else {
        cerr << PQgeterror() << endl;
    }

    return (String) value;
}



/**
 * Get integer value specified by column key
 * @param key column key
 * @return integer value
 */
int KeyValues::getInt(String key) {
    PGint4 value;

    if (! PQgetf(res, position, "#int4", key.c_str(), &value)) {
        cerr << PQgeterror() << endl;
//        value = 0; // maybe some logging or exception for logging?
    }
    
    return (int) value;
}

/**
 * Get integer value specified by index of column
 * @param pos index of column
 * @return integer value
 */
int KeyValues::getInt(int pos) {
    PGint4 value;

    if (! PQgetf(res, position, "%int4", pos, &value)) {
        cerr << PQgeterror() << endl;
//        value = 0; // maybe some logging or exception for logging?
    }
    
    return (int) value;
}

/**
 * Get array of integer values specified by column key
 * @param key column key
 * @param size size of array of integer values
 * @return array of integer values
 */
int* KeyValues::getIntA(String key, size_t& size) {
    PGarray tmp;
    int* values;

    if (! PQgetf(res, 0, "#int4[]", key.c_str(), &tmp)) {
        cerr << "Error" << PQgeterror() << endl;
    }

    size = PQntuples(tmp.res);
    values = new int(size);
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &values[i])) {
            cerr << PQgeterror() << endl;
        }
    }
    PQclear(tmp.res);

    return values;
}

/**
 * Get array of integer values specified by index of column
 * @param pos index of column
 * @param size size of array of integer values
 * @return array of integer values
 */
int* KeyValues::getIntA(int pos, size_t& size) {
    PGarray tmp;
    int* values;

    if (! PQgetf(res, 0, "%int4[]", pos, &tmp)) {
        cerr << "Error" << PQgeterror() << endl;
    }

    size = PQntuples(tmp.res);
    values = new int(size);
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &values[i])) {
            cerr << PQgeterror() << endl;
        }
    }
    PQclear(tmp.res);

    return values;
}

/**
 * Get vector of integer values specified by index of column
 * @param pos index of column
 * @return  array of integer values
 */
std::vector<int> KeyValues::getIntV(int pos) {
    PGarray tmp;
    PGint4 value;
    std::vector<int> values;

    if (! PQgetf(res, 0, "%int4[]", pos, &tmp)) {
        cerr << "Error" << PQgeterror() << endl;
    }

    for (int i = 0; i < PQntuples(tmp.res); i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &value)) {
            cerr << PQgeterror() << endl;
        }
        values.push_back(value);
    }
    PQclear(tmp.res);

    return values;
}



/**
 * Get float value specified by column key
 * @param key column key
 * @return float value
 */
float KeyValues::getFloat(String key) {
    PGfloat4 value;

    if (! PQgetf(res, position, "#float4", key.c_str(), &value)) {
        cerr << PQgeterror() << endl;
//        value = 0; // maybe some logging or exception for logging?
    }
    
    return (float) value;
}

/**
 * Get float value specified by index of column
 * @param pos index of column
 * @return float value
 */
float KeyValues::getFloat(int pos) {
    PGfloat4 value;

    if (! PQgetf(res, position, "%float4", pos, &value)) {
        cerr << PQgeterror() << endl;
//        value = 0; // maybe some logging or exception for logging?
    }

    return (float) value;
}

/**
 * Get array of float values specified by column key
 * @param key column key
 * @param size size of array of float values
 * @return array of float values
 */
float* KeyValues::getFloatA(String key, size_t& size) {
    PGarray tmp;
    float* values;

    if (! PQgetf(res, 0, "#float4[]", key.c_str(), &tmp)) {
        cerr << "Error" << PQgeterror() << endl;
    }

    size = PQntuples(tmp.res);
    values = new float(size);
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%float4", 0, &values[i])) {
            cerr << PQgeterror() << endl;
        }
    }
    PQclear(tmp.res);

    return values;
}

/**
 * Get array of float values specified by index of column
 * @param pos index of column
 * @param size size of array of float values
 * @return array of float values
 */
float* KeyValues::getFloatA(int pos, size_t& size) {
    PGarray tmp;
    float* values;

    if (! PQgetf(res, 0, "%float4[]", pos, &tmp)) {
        cerr << "Error" << PQgeterror() << endl;
    }

    size = PQntuples(tmp.res);
    values = new float(size);
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%float4", 0, &values[i])) {
            cerr << PQgeterror() << endl;
        }
    }
    PQclear(tmp.res);

    return values;
}


// !!!!!! TODO SETS TODO !!!!!!!
/**
 * Set string value specified by column key
 * @param key column key
 * @param value string value
 * @return On success, a non-zero value is returned. On error, zero is returned.
 */
bool KeyValues::setString(String key, String value) {
//    return PQputf(res, "%text %text", key, value);
}

/**
 * Set integer value specified by column key and string value
 * @param key column key
 * @param value integer value represented by string
 * @return On success, a non-zero value is returned. On error, zero is returned.
 */
bool KeyValues::setInt(String key, String value) {
    PGint4 newValue;
    if (! sscanf(value.c_str(), "%d", &newValue)) {
        return false;
    }
//    return PQputf(key, "%int4", newValue);
}

bool KeyValues::setIntA(String key, int* value, size_t size) {

}

/**
 * Set float value specified by column key and string value
 * @param key column key
 * @param value float value represented by string
 * @return On success, a non-zero value is returned. On error, zero is returned.
 */
bool KeyValues::setFloat(String key, String value) {
    PGfloat4 newValue;
    
    if (! sscanf(value.c_str(), "%f", &newValue)) {
        return false;
    }
//    return PQputf(key, "%float4", newValue);
}

bool KeyValues::setFloatA(String key, float_t value, size_t size) {

}
