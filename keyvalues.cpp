/*
 * File:   keyvalues.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:52
 */

#include "vtapi.h"
#include "postgresql/libpqtypes.h"
#include "postgresql/pg_type.h"
#include <cstdlib>
#include <iostream>

using namespace std;




// ************************************************************************** //
String TKey::print() {
    String ret = "TKey type=" + type + ", key=" + key + ", size=" + toString(size) + ", from=" + from + "\n";
    std::cout << ret;
    return (ret);
}


// FIXME: tohle kdyz dam sem, tak je to v haji - proc?
// template <class T>
// String TKeyValue<T>::print() {};
// ************************************************************************** //

// FIXME: proc to nekdy vynecha tento konstruktor???
KeyValues::KeyValues(const Commons& orig) 
          : Commons(orig), select(NULL), pos(-1), insert(NULL) {
}

KeyValues::KeyValues(const KeyValues& orig)
          : Commons(orig), select(NULL), pos(-1), insert(NULL) {
}


KeyValues::~KeyValues() {
    destruct(select);
    destruct(insert);
}

KeyValues* KeyValues::next() {
    // check the Select, each subclass is responsible of
    if (!select) error(301, "There is no select class");

    // it is executed here when position == -1
    if (pos == -1) {
        if (select->res) PQclear(select->res);
        select->execute();
    }

    // whether should be something inserted
    if (insert && !insert->executed) {
        insert->execute();  // FIXME: here should be the store fun instead
    }

    // TODO: zatim to skonci po konci resultsetu, ale melo by zjistit, jestli je
    // to na konci nebo neni a spachat kdyztak dalsi dotaz (limit, offset)
    if (select->res && (pos < (PQntuples(select->res) - 1))) {
        pos++;
        return this;
    }

    return NULL;
}


void KeyValues::print() {
    if (select && select->res && pos > -1) this->printRes(select->res, pos);
    else warning(302, "There is nothing to print (see other messages)");
}

void KeyValues::printAll() {
    if (select && select->res) this->printRes(select->res);
    else warning(303, "There is nothing to print (see other messages)");
}

/**
 * Get string value specified by column key
 * @param key column key
 * @return string value
 */
String KeyValues::getString(String key) {
    PGtext value;

    // Several data types are other representation of string, so we must catch all of them
    switch (PQftype(select->res, PQfnumber(select->res, key.c_str()))) {
        case TEXTOID:
                PQgetf(select->res, this->pos, "#text", key.c_str(), &value);
            break;
        case NAMEOID:
                PQgetf(select->res, this->pos, "#name", key.c_str(), &value);
            break;
        case VARCHAROID:
                PQgetf(select->res, this->pos, "#varchar", key.c_str(), &value);
            break;
        case BYTEAOID:
                PQgetf(select->res, this->pos, "#bytea", key.c_str(), &value);
            break;
        case BPCHAROID:
                PQgetf(select->res, this->pos, "#bpchar", key.c_str(), &value);
            break;
        default:
                error(304, "Value of key \"" + key + "\" is not a string");
            break;
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

    // Several data types are other representation of string, so we must catch all of them
    switch (PQftype(select->res, PQfnumber(select->res, pos))) {
        case TEXTOID:
                PQgetf(select->res, this->pos, "%text", pos, &value);
            break;
        case NAMEOID:
                PQgetf(select->res, this->pos, "%name", pos, &value);
            break;
        case VARCHAROID:
                PQgetf(select->res, this->pos, "%varchar", pos, &value);
            break;
        case BYTEAOID:
                PQgetf(select->res, this->pos, "%bytea", pos, &value);
            break;
        case BPCHAROID:
                PQgetf(select->res, this->pos, "%bpchar", pos, &value);
            break;
        default:
                error(305, "Value of column index #" + pos + " is not a string");
            break;
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

    if (! PQgetf(select->res, this->pos, "#int4", key.c_str(), &value)) {
        error(306, "Value of key \"" + key + "\" is not an integer");
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

    if (! PQgetf(select->res, this->pos, "%int4", pos, &value)) {
        error(307, "Value of column index #" + pos + " is not an integer");
    }
    
    return (int) value;
}

/**
 * Get array of integer values specified by column key
 * @param key column key
 * @param size size of array of integer values
 * @return array of integer values
 */
int* KeyValues::getIntA(String key, int& size) {
    PGarray tmp;
    int* values;

    if (! PQgetf(select->res, this->pos, "#int4[]", key.c_str(), &tmp)) {
        error(308, "Value of key \"" + key + "\" is not an array of integer");
    }

    size = PQntuples(tmp.res);
    values = new int(size);
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &values[i])) {
            error(309, "Value of key \"" + key + "\": unexpected value in integer array");
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
int* KeyValues::getIntA(int pos, int& size) {
    PGarray tmp;
    int* values;

    if (! PQgetf(select->res, this->pos, "%int4[]", pos, &tmp)) {
        error(310, "Value of column index #" + pos + "\" is not an array of integer");
    }

    size = PQntuples(tmp.res);
    values = new int(size);
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &values[i])) {
            error(311, "Value of column index #" + pos + ": unexpected value in integer array");
        }
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

    if (! PQgetf(select->res, this->pos, "#int4[]", key.c_str(), &tmp)) {
        error(312, "Value of key \"" + key + "\" is not an array of integer");
    }

    for (int i = 0; i < PQntuples(tmp.res); i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &value)) {
            error(313, "Value of key \"" + key + "\": unexpected value in integer array");
        }
        values.push_back(value);
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

    if (! PQgetf(select->res, this->pos, "%int4[]", pos, &tmp)) {
        error(314, "Value of column index #" + pos + "\" is not an array of integer");
    }

    for (int i = 0; i < PQntuples(tmp.res); i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &value)) {
            error(315, "Value of column index #" + pos + ": unexpected value in integer array");
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

    if (! PQgetf(select->res, this->pos, "#float4", key.c_str(), &value)) {
        error(316, "Value of key \"" + key + "\" is not a float");
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

    if (! PQgetf(select->res, this->pos, "%float4", pos, &value)) {
        error(317, "Value of column index #" + pos + " is not a float");
    }

    return (float) value;
}

/**
 * Get array of float values specified by column key
 * @param key column key
 * @param size size of array of float values
 * @return array of float values
 */
float* KeyValues::getFloatA(String key, int& size) {
    PGarray tmp;
    float* values;

    if (! PQgetf(select->res, this->pos, "#float4[]", key.c_str(), &tmp)) {
        error(318, "Value of key \"" + key + "\" is not an array of float");
    }

    size = PQntuples(tmp.res);
    values = new float(size);
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%float4", 0, &values[i])) {
            error(319, "Value of key \"" + key + "\": unexpected value in float array");
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
float* KeyValues::getFloatA(int pos, int& size) {
    PGarray tmp;
    float* values;

    if (! PQgetf(select->res, this->pos, "%float4[]", pos, &tmp)) {
        error(320, "Value of column index #" + pos + "\" is not an array of float");
    }

    size = PQntuples(tmp.res);
    values = new float(size);
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%float4", 0, &values[i])) {
            error(321, "Value of column index #" + pos + ": unexpected value in float array");
        }
    }
    PQclear(tmp.res);

    return values;
}


// !!!!!! TODO SETS TODO !!!!!!!
// TODO: mozna by se dalo premyslet o PQsetvalue
// FIXME Tomas: to nize (smazano) ma byt pokus o (sebe)vrazdu?

