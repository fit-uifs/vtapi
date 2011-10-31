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

    // FIXME Tomas: a neni lepsi zjistit (PQftype), co to vlastne je a pak pouzit spravnou funkci???
    // jak jinak by to ty libpqtypes zjistily... ?

    // Several data types are other representation of string, so we must catch all of them
    if (PQgetf(select->res, pos, "#text", key.c_str(), &value)) {}
    else if (PQgetf(select->res, pos, "#name", key.c_str(), &value)) {}
    else if (PQgetf(select->res, pos, "#varchar", key.c_str(), &value)) {}
    else if (PQgetf(select->res, pos, "#bytea", key.c_str(), &value)) {}
    else if (PQgetf(select->res, pos, "#bpchar", key.c_str(), &value)) {}
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

    if (PQgetf(select->res, pos, "%text", pos, &value)) {}
    else if (PQgetf(select->res, pos, "%name", pos, &value)) {}
    else if (PQgetf(select->res, pos, "%varchar", pos, &value)) {}
    else if (PQgetf(select->res, pos, "%regtype", pos, &value)) {}
    else if (PQgetf(select->res, pos, "%bytea", pos, &value)) {}
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

    if (! PQgetf(select->res, pos, "#int4", key.c_str(), &value)) {
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

    if (! PQgetf(select->res, pos, "%int4", pos, &value)) {
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
int* KeyValues::getIntA(String key, int& size) {
    PGarray tmp;
    int* values;

    if (! PQgetf(select->res, pos, "#int4[]", key.c_str(), &tmp)) {
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
int* KeyValues::getIntA(int pos, int& size) {
    PGarray tmp;
    int* values;

    if (! PQgetf(select->res, pos, "%int4[]", pos, &tmp)) {
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

    if (! PQgetf(select->res, pos, "%int4[]", pos, &tmp)) {
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

    if (! PQgetf(select->res, pos, "#int4[]", key.c_str(), &tmp)) {
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

    if (! PQgetf(select->res, pos, "#float4", key.c_str(), &value)) {
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

    if (! PQgetf(select->res, pos, "%float4", pos, &value)) {
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
float* KeyValues::getFloatA(String key, int& size) {
    PGarray tmp;
    float* values;

    if (! PQgetf(select->res, pos, "#float4[]", key.c_str(), &tmp)) {
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
float* KeyValues::getFloatA(int pos, int& size) {
    PGarray tmp;
    float* values;

    if (! PQgetf(select->res, pos, "%float4[]", pos, &tmp)) {
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
// FIXME Tomas: to nize (smazano) ma byt pokus o (sebe)vrazdu?

