/*
 * File:   keyvalues.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:52
 */

#include <cstdlib>
#include <iostream>

#include "postgresql/libpqtypes.h"
#include "vtapi.h"


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

KeyValues::KeyValues(const Commons& orig) 
          : Commons(orig), select(NULL), pos(-1), insert(NULL) {
    thisClass = "KeyValues(Commons&)";
}

KeyValues::KeyValues(const KeyValues& orig)
          : Commons(orig), select(NULL), pos(-1), insert(NULL) {
    thisClass = "KeyValues(KeyValues&)";
}


KeyValues::~KeyValues() {
    destruct (select);
    destruct (insert);

    this->beDoomed();
}

KeyValues* KeyValues::next() {
    // check the Select, each subclass is responsible of
    if (!select) error(301, "There is no select class");

    // it is executed here when position == -1
    if (pos == -1) {
        if (select->res) {
            PQclear(select->res);
            select->res = NULL;
        }
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



// =============== GETTERS (Select) ============================================
// =============== GETTERS FOR STRINGS =========================================
/**
 * Get a string value specified by a column key
 * @param key column key
 * @return string value
 */
String KeyValues::getString(String key) {
    return this->getString(PQfnumber(select->res, key.c_str()));
}

/**
 * Get a string value specified by an index of a column
 * @param position index of the column
 * @return string value
 */
String KeyValues::getString(int position) {
    PGtext value = (PGtext) "";
    
    String typname = this->toTypname(PQftype(select->res, position));

    // Several data types are other representation of string, so we must catch all of them
    if (typname.compare("text") == 0) {
        PQgetf(select->res, pos, "%text", position, &value);
    }
    else if (typname.compare("name") == 0) {
        PQgetf(select->res, pos, "%name", position, &value);
    }
    else if (typname.compare("varchar") == 0) {
        PQgetf(select->res, pos, "%varchar", position, &value);
    }
    else if (typname.compare("character varying") == 0) {
        PQgetf(select->res, pos, "%varchar", position, &value);
    }
    else if (typname.compare("oid") == 0) {
        value = (PGtext) this->toTypname(this->getInt(position)).c_str();
    }
    // FIXME Tomas: tady mozna chybi 3. hodnota?
    else if (typname.compare("inouttype") == 0) {
        value = (PGtext) ((this->getInt(position)) ? "out" : "in");
    }
    else if (typname.compare("bytea") == 0) {
        PQgetf(select->res, pos, "%bytea", position, &value);
    }
    else if (typname.compare("bpchar") == 0) {
        PQgetf(select->res, pos, "%bpchar", position, &value);
    }
    else if (typname.compare("regclass") == 0) {
        PQgetf(select->res, pos, "%varchar", position, &value);
    }
    else {
       warning(304,"Type of (" + toString(position) + ") is not a string");
       this->print();
    }
       
    if (value == NULL) {
        value = (PGtext) "";
    }

    return String(value);
}



// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================
/**
 * Get an integer value specified by a column key
 * @param key column key
 * @return integer value
 */
int KeyValues::getInt(String key) {
    return this->getInt(PQfnumber(select->res, key.c_str()));
}

/**
 * Get an integer value specified by an index of a column
 * @param position index of column
 * @return integer value
 */
int KeyValues::getInt(int position) {
    PGint4 value;
    String typname = this->toTypname(PQftype(select->res, position));

    if (typname.compare("int4") == 0) {
        PQgetf(select->res, this->pos, "%int4", position, &value);
    }
    else if (typname.compare("oid") == 0) {
        PQgetf(select->res, this->pos, "%oid", position, &value);
    }
    else if (typname.compare("inouttype") == 0) {
        PQgetf(select->res, this->pos, "%inouttype", position, &value);
    }
    else {
        warning(305, "Value is not an integer");
        this->print();
    }
    
    return (int) value;
}

/**
 * Get an array of integer values specified by a column key
 * @param key column key
 * @param size size of the array of integer values
 * @return array of integer values
 */
int* KeyValues::getIntA(String key, int& size) {
    return this->getIntA(PQfnumber(select->res, key.c_str()), size);
}

/**
 * Get an array of integer values specified by an index of a column
 * @param pos index of column
 * @param size size of the array of integer values
 * @return array of integer values
 */
int* KeyValues::getIntA(int position, int& size) {
    PGarray tmp;
    int* values;

    if (! PQgetf(select->res, this->pos, "%int4[]", position, &tmp)) {
        warning(306, "Value is not an array of integer");
        this->print();
    }

    size = PQntuples(tmp.res);
    values = new int(size);
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &values[i])) {
            warning(307, "Unexpected value in integer array");
            this->print();

        }
    }
    PQclear(tmp.res);

    return values;
}

/**
 * Get a vector of integer values specified by a column key
 * @param key column key
 * @return  array of integer values
 */
std::vector<int> KeyValues::getIntV(String key) {
    return this->getIntV(PQfnumber(select->res, key.c_str()));
}

/**
 * Get a vector of integer values specified by an index of a column
 * @param position index of column
 * @return  array of integer values
 */
std::vector<int> KeyValues::getIntV(int position) {
    PGarray tmp;
    PGint4 value;
    std::vector<int> values;

    if (! PQgetf(select->res, this->pos, "%int4[]", position, &tmp)) {
        warning(308, "Value is not an array of integer");
        this->print();
    }

    for (int i = 0; i < PQntuples(tmp.res); i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &value)) {
            warning(309, "Unexpected value in integer array");
            this->print();
        }
        values.push_back(value);
    }
    PQclear(tmp.res);

    return values;
}



// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================
/**
 * Get a float value specified by a column key
 * @param key column key
 * @return float value
 */
float KeyValues::getFloat(String key) {
    return this->getFloat(PQfnumber(select->res, key.c_str()));
}

/**
 * Get a float value specified by an index of a column
 * @param position index of column
 * @return float value
 */
float KeyValues::getFloat(int position) {
    PGfloat4 value;

    if (! PQgetf(select->res, this->pos, "%float4", position, &value)) {
        warning(310, "Value is not a float");
        this->print();
    }

    return (float) value;
}

/**
 * Get an array of float values specified by a column key
 * @param key column key
 * @param size size of the array of float values
 * @return array of float values
 */
float* KeyValues::getFloatA(String key, int& size) {
    return this->getFloatA(PQfnumber(select->res, key.c_str()), size);
}

/**
 * Get array of float values specified by index of column
 * @param position index of column
 * @param size size of the array of float values
 * @return array of float values
 */
float* KeyValues::getFloatA(int position, int& size) {
    PGarray tmp;
    float* values;

    if (! PQgetf(select->res, this->pos, "%float4[]", position, &tmp)) {
        warning(311, "Value is not an array of float");
        this->print();
    }

    size = PQntuples(tmp.res);
    values = new float(size);
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%float4", 0, &values[i])) {
            warning(312, "Unexpected value in float array");
            this->print();
        }
    }
    PQclear(tmp.res);

    return values;
}



// =============== GETTERS - OTHER =============================================
/**
 * Get a string value from a name data type specified by a column key
 * @param key column key
 * @return string value
 */
String KeyValues::getName(String key) {
    PGtext value = (PGtext) "";

    PQgetf(select->res, this->pos, "#name", key.c_str(), &value);

    if (value == NULL) {
        value = (PGtext) "";
    }

    return (String) value;
}

/**
 * Get an integer with an OID value specified by a column key
 * @param key column key
 * @return integer with the OID value
 */
int KeyValues::getOid(String key) {
    PGint4 value;

    PQgetf(select->res, this->pos, "#oid", key.c_str(), &value);

    return (int) value;
}




// =============== SETTERS (Update) ============================================
// !!!!!! TODO SETS TODO !!!!!!!
// TODO: mozna by se dalo premyslet o PQsetvalue
// FIXME Tomas: to nize (smazano) ma byt pokus o (sebe)vrazdu?

