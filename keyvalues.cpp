/*
 * File:   keyvalues.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:52
 */

#include "vtapi.h"
#include "postgresql/libpqtypes.h"
#include <cstdlib>
#include <iostream>

using namespace std;



KeyValues::KeyValues(const Commons& orig) 
          : Commons(orig), select(NULL), insert(NULL), position(-1) {
}

KeyValues::KeyValues(const KeyValues& orig) 
          : Commons(orig), select(orig.select), insert(orig.insert), position(orig.position) {
}

KeyValues::~KeyValues() {
    destruct(select);
    destruct(insert);
}

KeyValues* KeyValues::next() {
    // tridu Select, tu si naplni kazda podtrida sama
    // naplnena podtrida Select spusti execute ZDE kdyz position == -1
    if (!select) error(501, "There is no select class");

    if (position = -1) {
        if (select->res) PQclear(select->res);
        select->execute();
    }

    // TODO: zatim to skonci po konci resultsetu, ale melo by zjistit, jestli je
    // to na konci nebo neni a spachat kdyztak dalsi dotaz (limit, offset)
    if (select->res && (position < PQntuples(select->res) - 1)) {
        position++;
        return this;
    }

    return NULL;
}


void KeyValues::print() {
    if (select && select->res && position > -1) this->printRes(select->res, position);
    else warning(502, "There is nothing to print (see other errors)");
}

void KeyValues::printAll() {
    if (select && select->res) this->printRes(select->res);
    else warning(503, "There is nothing to print (see other errors)");
}

/**
 * Get string value specified by column key
 * @param key column key
 * @return string value
 */
String KeyValues::getString(String key) {
    PGtext value;

    // FIXME Tomas: a neni lepsi zjistit (PQftype), co to vlastne je a pak pouzit spravnou funkci???
    // jak jinak by to ty libpqtypes zjistily... ?

    // Several data types are other representation of string, so we must catch all of them
    if (PQgetf(select->res, position, "#text", key.c_str(), &value)) {}
    else if (PQgetf(select->res, position, "#name", key.c_str(), &value)) {}
    else if (PQgetf(select->res, position, "#varchar", key.c_str(), &value)) {}
    else if (PQgetf(select->res, position, "#bytea", key.c_str(), &value)) {}
    else if (PQgetf(select->res, position, "#bpchar", key.c_str(), &value)) {}
    else {
        cerr << PQgeterror() << endl;
    }

    if (value == NULL) {
        value = (PGtext) "";
    }
    
    return (String) value;
}

/**
 * Get string value spevified by index of column
 * @param pos index of column
 * @return string value
 */
String KeyValues::getString(int pos) {
    PGtext value = (PGtext) "";

    if (PQgetf(select->res, position, "%text", pos, &value)) {}
    else if (PQgetf(select->res, position, "%name", pos, &value)) {}
    else if (PQgetf(select->res, position, "%varchar", pos, &value)) {}
    else if (PQgetf(select->res, position, "%regtype", pos, &value)) {}
    else if (PQgetf(select->res, position, "%bytea", pos, &value)) {}
    else {
        cerr << PQgeterror() << endl;
    }

    if (value == NULL) {
        value = (PGtext) "";
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

    if (! PQgetf(select->res, position, "#int4", key.c_str(), &value)) {
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

    if (! PQgetf(select->res, position, "%int4", pos, &value)) {
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

    if (! PQgetf(select->res, position, "#int4[]", key.c_str(), &tmp)) {
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

    if (! PQgetf(select->res, position, "%int4[]", pos, &tmp)) {
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

    if (! PQgetf(select->res, position, "%int4[]", pos, &tmp)) {
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
 * Get vector of integer values specified by column key
 * @param pos index of column
 * @return  array of integer values
 */
std::vector<int> KeyValues::getIntV(String key) {
    PGarray tmp;
    PGint4 value;
    std::vector<int> values;

    if (! PQgetf(select->res, position, "#int4[]", key.c_str(), &tmp)) {
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

    if (! PQgetf(select->res, position, "#float4", key.c_str(), &value)) {
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

    if (! PQgetf(select->res, position, "%float4", pos, &value)) {
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

    if (! PQgetf(select->res, position, "#float4[]", key.c_str(), &tmp)) {
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

    if (! PQgetf(select->res, position, "%float4[]", pos, &tmp)) {
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
// TODO: mozna by se dalo premyslet o PQsetvalue

// FIXME Tomas: to nize ma byt pokus o (sebe)vrazdu?

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
    error("TODO");
}



// ************************************************************************** //
String TKey::print() {
    String ret = "TKey type=" + type + ", key=" + key + ", from=" + from + "\n";
    std::cout << ret;
    return (ret);
}
