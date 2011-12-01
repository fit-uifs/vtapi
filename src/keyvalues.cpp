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

    if (select->res && (pos < (PQntuples(select->res) - 1))) {
        pos++;
        return this;
    }
    // TODO: zatim to skonci po konci resultsetu, ale melo by zjistit, jestli je
    // to na konci nebo neni a spachat kdyztak dalsi dotaz (limit, offset)

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
String KeyValues::getString(const String& key) {
    return this->getString(PQfnumber(select->res, key.c_str()));
}


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
int KeyValues::getInt(const String& key) {
    return this->getInt(PQfnumber(select->res, key.c_str()));
}

int KeyValues::getInt(int position) {
    PGint4 value;
    String typname = this->toTypname(PQftype(select->res, position));

    if (typname.compare("int4") == 0) {
        PQgetf(select->res, this->pos, "%int4", position, &value);
    }
    if (typname.compare("int8") == 0) { // TODO: make a long type???
        PQgetf(select->res, this->pos, "%int8", position, &value);
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

int* KeyValues::getIntA(const String& key, int& size) {
    return this->getIntA(PQfnumber(select->res, key.c_str()), size);
}

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

std::vector<int> KeyValues::getIntV(const String& key) {
    return this->getIntV(PQfnumber(select->res, key.c_str()));
}

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
float KeyValues::getFloat(const String& key) {
    return this->getFloat(PQfnumber(select->res, key.c_str()));
}

// TODO: double???
float KeyValues::getFloat(int position) {
    PGfloat4 value;

    if (! PQgetf(select->res, this->pos, "%float4", position, &value)) {
        warning(310, "Value is not a float");
        this->print();
    }

    return (float) value;
}

float* KeyValues::getFloatA(const String& key, int& size) {
    int pos = PQfnumber(select->res, key.c_str());
    if (pos < 0) warning(313, "Column " + toString(key) + " doesn't exist.");
    return this->getFloatA(pos, size);
}

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
String KeyValues::getName(const String& key) {
    PGtext value = (PGtext) "";

    PQgetf(select->res, this->pos, "#name", key.c_str(), &value);

    if (value == NULL) {
        value = (PGtext) "";
    }

    return (String) value;
}

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