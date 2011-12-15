/*
 * File:   keyvalues.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:52
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>
#if defined(WIN32) || defined(WIN64)
#include <Winsock2.h>
#else
#include <netinet/in.h>
#endif
#include "postgresql/libpqtypes.h"
#include "vtapi.h"


using namespace std;

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

    if (select->res && (pos < (PQntuples(select->res) - 1))) {
        pos++;
        return this;
    }
    // TODO: zatim to skonci po konci resultsetu, ale melo by zjistit, jestli je
    // to na konci nebo neni a spachat kdyztak dalsi dotaz (limit, offset)

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


// =============== PRINT methods =======================================

/**
 * Prints currently selected row in resultset (Select)
 */
void KeyValues::print() {
    if (!select || !select->res || pos<0) warning(302, "There is nothing to print (see other messages)");
    else {
        int origpos = this->pos;
        pair< vector<TKey>,vector<int> > fInfo = getFieldsInfo(pos);
        printHeader(&fInfo);
        printRowOnly(pos, &fInfo.second);
        printFooter(1);
        this->pos = origpos;
    }
}
/**
 * Prints all rows in resultset (Select)
 */
void KeyValues::printAll() {
    if (!select || !select->res || pos<0) warning(303, "There is nothing to print (see other messages)");
    else {
        int origpos = this->pos;
        pair< vector<TKey>,vector<int> > fInfo = getFieldsInfo();
        printHeader(&fInfo);
        for (int r = 0; r < PQntuples(select->res); r++) printRowOnly(r, &fInfo.second);
        printFooter();
        this->pos = origpos;
    }
}

// =============== PRINT support methods =======================================
/**
 * Prints header - field name and data type
 * @param res Input resultset
 * @param fInfo
 * // param widths Vector of column widths
 * @todo param fInfo in documentation
 */
void KeyValues::printHeader(const pair< vector<TKey>,vector<int> >* fInfo) {
    std::stringstream table, nameln, typeln, border;
    int cols = PQnfields(select->res);

    if (!fInfo || fInfo->first.size()!=cols || fInfo->second.size()!=cols) {
        //TODO: error printing
        return;
    }
    if (format == HTML) {
        if (tableOpt.empty()) table << "<table>";
        else table << "<table " << tableOpt << ">";
        if (!caption.empty()) table << "<caption align=\"top\">" << caption << "</caption>";
        table << endl << "<tr align=\"center\">";
    }

    for (int c = 0; c < cols; c++) {
        if (format == STANDARD) {
            nameln << left << setw(fInfo->second[c]) << fInfo->first[c].key;
            typeln << left << setw(fInfo->second[c]) << fInfo->first[c].type;
            border << setfill('-') << setw(fInfo->second[c]) << "";
            if (c < cols-1) {
                nameln << '|'; typeln << '|'; border << '+';
            }
        }
        else if (format == CSV) {
            nameln << fInfo->first[c].key;
            if (c < cols-1) nameln << ',';
        }
        else if (format == HTML) {
            table << "<th>" << fInfo->first[c].key << "<br/>";
            table << fInfo->first[c].type << "</th>";
        }
    }
    table << "</tr>" << endl; nameln << endl; typeln << endl; border << endl;
    if (format == STANDARD) cout << nameln.str() << typeln.str() << border.str();
    else if (format == CSV) cout << nameln.str();
    else if (format == HTML)cout << table.str();
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
        if (count > 0) output << "(" << count << " of " << rows << " rows)" << endl;
        else output << "(" << rows << " rows)" << endl;
    }
    else if (format == HTML) output << "</table>" << endl;
    cout << output.str();
}
/**
 * Prints values in single row
 * @param res Input resultset
 * @param row Row number
 * @param widths Vector of column widths
 */
void KeyValues::printRowOnly(const int row, const vector<int>* widths) {
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
            output << left << setw((*widths)[c]) << val;
            if (c < cols-1) output << '|';
        }
        else if (format == CSV) {
            output << val;
            if (c < cols-1) output << ',';
        }
        else if (format == HTML) {
            output << "<td>" << val << "</td>";
        }
    }
    if (format == HTML) output << "</tr>";
    output << endl;
    cout << output.str();
}
/**
 * Returns data types, field names and desired column widths for all fields
 * @param res Input resultset
 * @param row Row number to process (0 = all)
 * @return Pair of vectors <TKey,widths> (TKey includes data type and field name)
 */
pair< vector<TKey>,vector<int> > KeyValues::getFieldsInfo(const int row) {
    int plen, flen, tlen, width;
    vector<int> widths;
    vector<TKey> keys;
    vector<TKey>* keysptr = getKeys();
    int rows = PQntuples(select->res);
    int cols = PQnfields(select->res);

    if (keysptr) {
        keys = *keysptr;
        destruct(keysptr);
    }
    else {
        //TODO: error getting keys
        return make_pair(keys, widths);
    }
    if (cols != keys.size()) {
        //TODO: error getting keys
        keys.clear();
        return make_pair(keys, widths);
    }
    // header and first row
    for (int c = 0; c < cols; c++) {
        // don't forget to save original value of pos beforehand!
        this->pos = row < 0 ? 0 : row;
        flen = keys[c].key.length();  // field name length
        tlen = keys[c].type.length(); // data type string length
        plen = getValue(c).length();  // field value string length
        if (plen >= flen && plen >= tlen) width = plen;
        else if (flen >= plen && flen >= tlen) width = flen;
        else width = tlen;
        widths.push_back(width);
    }
    // rest of the rows
    if (row < 0) {
        for (int r = 1; r < rows; r++) {
            // very ugly manipulation with pos, it needs to be reset after print
            this->pos = r;
            for (int c = 0; c < cols; c++) {
                plen = getValue(c).length();
                if (plen > widths[c]) widths[c] = plen;
            }
        }
    }
    return make_pair(keys, widths);
}

String KeyValues::getValue(const int col) {

    stringstream valss;
    TKey colkey = getKey(col);
    if (colkey.size < 0) return "";
    char typcategory = typemap->getCategory(colkey.type);
    short typlen = typemap->getLength(colkey.type);
    int typelemoid = typemap->getElemOID(colkey.type);

    //TODO: dodelat
    switch (typcategory) {
        case 'A': { // array
            //TODO: dodelat array
            if (!typemap->toTypname(typelemoid).compare("int4")) {
                std::vector<int>* arr = this->getIntV(col);
                if (arr) for (int i = 0; i < (*arr).size(); i++) {
                    valss << (*arr)[i];
                    if (i < (*arr).size()-1) valss << ",";
                }
                destruct (arr);
            }
            else if (!typemap->toTypname(typelemoid).compare("float4")) {
                std::vector<float>* arr = this->getFloatV(col);
                if (arr) for (int i = 0; i < (*arr).size(); i++) {
                    valss << (*arr)[i];
                    if (i < (*arr).size()-1) valss << ",";
                }
                destruct (arr);
            }
            else valss << 'A';
            } break;
        case 'B': { // boolean
            valss << 'B';
            } break;
        case 'C': { // composite
            valss << 'C';
            } break;
        case 'D': { // date/time
            struct tm ts = getTimestamp(col);
            valss << right << setfill('0');
            if (ts.tm_year > 0)
                valss << ts.tm_year << '-' << setw(2) << ts.tm_mon << '-' << setw(2) << ts.tm_mday << ' ';
            valss << setw(2) << ts.tm_hour << ':' << setw(2) << ts.tm_min << ':' << setw(2) << ts.tm_sec;
            } break;
        case 'E': { // enum
            // can get name
            valss << getString(col);
            } break;
        case 'G': { // geometric
            valss << 'G';
            } break;
        case 'I': { // network address
            valss << 'I';
            } break;
        case 'N': { // numeric
            // can be oid reference
            if (typemap->isRefType(colkey.type)) valss << getString(col);
            else if (PQgetlength(select->res, pos, col) > 0) {
                if (!colkey.type.substr(0,5).compare("float")) valss << getFloat(col);
                else valss << getInt(col);
            }
            } break;
        case 'P': { // pseudo
            valss << 'P';
            } break;
        case 'S': { // string
            // char has length 1
            if (typlen == 1) valss << getChar(col);
            else valss << getString(col);
            } break;
        case 'T': { // timespan
            valss << 'T';
            } break;
        case 'U': { // user-defined
            valss << 'U';
            } break;
        case 'V': { // bit-string
            valss << 'V';
            } break;
        case 'X': { // unknown
            valss << "unknown";
            } break;
        default: {
            valss << "undefined";
            } break;
    }
    return valss.str();    
}


// =============== GETTERS (Select) ============================================
// =============== GETTERS FOR CHAR, STRINGS ===================================
char KeyValues::getChar(const String key) {
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
String KeyValues::getString(const String key) {
    return this->getString(PQfnumber(select->res, key.c_str()));
}

String KeyValues::getString(const int col) {
    int ret = 0;
    PGtext value = (PGtext) "";
    TKey colkey = getKey(col);
    if (!select->res) return "";

    if (typemap->getCategory(colkey.type) == 'S')  // string types
        value = PQgetvalue(select->res, pos, col);
    else if (typemap->isEnumType(colkey.type))     // enum types
        value = PQgetvalue(select->res, pos, col);
    else if (typemap->isRefType(colkey.type)) { // reference types (regclass..)
        pair<String,String> reftable = typemap->getRefTable(colkey.type);
        Oid oid = ntohl(*(Oid *)PQgetvalue(select->res, pos, col));
        if (oid > 0) {
            // better without creating KeyValues
            stringstream qss;
            qss << "SELECT " << reftable.second << " FROM " << reftable.first;
            qss << " WHERE oid = " << oid;
            PGresult* rres = PQexec(this->getConnector()->getConn(), qss.str().c_str());
            if (rres) {
                ret = PQgetf(rres, 0, "%name", 0, &value);
                PQclear(rres);
            }
        }
    }
    else {
        stringstream wss;
        wss << "Value of type " << colkey.type << " isn't a string";
        warning(305, wss.str());
        return String("");
    }
    return value ? String(value) : String("");
}

// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================
int KeyValues::getInt(const String key) {
    return this->getInt(PQfnumber(select->res, key.c_str()));
}

int KeyValues::getInt(const int col) {
    int value = 0;
    short length = typemap->getLength(getKey(col).type);
    if (length < 0) {
        stringstream iss (PQgetvalue(select->res, pos, col));
        iss >> value;
    }
    if (length == 2)
        value = ntohl(*(PGint2 *)PQgetvalue(select->res, pos, col));
    else if (length == 4)
        value = ntohl(*(PGint4 *)PQgetvalue(select->res, pos, col));
    else {
        stringstream wss;
        wss << "Integer value of length " << length << " is not supported";
        warning(306, wss.str());
    }
    return value;
}


int* KeyValues::getIntA(const String key, int *size) {
    return this->getIntA(PQfnumber(select->res, key.c_str()), size);
}
//TODO: toto
int* KeyValues::getIntA(const int col, int *size) {
    PGarray tmp;
    if (! PQgetf(select->res, this->pos, "%int4[]", col, &tmp)) {
        warning(306, "Value is not an array of integer");
        *size = -1;
        return NULL;
    }

    *size = PQntuples(tmp.res);
    int* values = new int [*size];
    for (int i = 0; i < *size; i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &values[i])) {
            warning(307, "Unexpected value in integer array");
            *size = -1;
            return NULL;
        }
    }
    PQclear(tmp.res);

    return values;
}

std::vector<int>* KeyValues::getIntV(const String key) {
    return this->getIntV(PQfnumber(select->res, key.c_str()));
}
//TODO: toto
std::vector<int>* KeyValues::getIntV(const int col) {
    PGarray tmp;
    if (! PQgetf(select->res, this->pos, "%int4[]", col, &tmp)) {
        warning(308, "Value is not an array of integer");
        return NULL;
    }

    PGint4 value;
    std::vector<int>* values = new std::vector<int>;

    for (int i = 0; i < PQntuples(tmp.res); i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &value)) {
            warning(309, "Unexpected value in integer array");
            destruct (values);
            return NULL;
        }
        values->push_back(value);
    }
    PQclear(tmp.res);

    return values;
}

// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================
float KeyValues::getFloat(const String key) {
    return this->getFloat(PQfnumber(select->res, key.c_str()));
}
float KeyValues::getFloat(const int col) {
    PGfloat4 value;

    if (! PQgetf(select->res, this->pos, "%float4", col, &value)) {
        warning(310, "Value is not a float");
    }

    return (float) value;
}

float* KeyValues::getFloatA(const String key, int *size) {
    int pos = PQfnumber(select->res, key.c_str());
    if (pos < 0) warning(313, "Column " + toString(key) + " doesn't exist.");
    return this->getFloatA(pos, size);
}
//TODO: toto dodelat
float* KeyValues::getFloatA(const int col, int *size) {
    PGarray tmp;
    if (! PQgetf(select->res, this->pos, "%float4[]", col, &tmp)) {
        warning(311, "Value is not an array of float");
        *size = -1;
        return NULL;
    }

    *size = PQntuples(tmp.res);
    float* values = new float [*size];
    for (int i = 0; i < *size; i++) {
        if (! PQgetf(tmp.res, i, "%float4", 0, &values[i])) {
            warning(312, "Unexpected value in float array");
            *size = -1;
            return NULL;
        }
    }
    PQclear(tmp.res);

    return values;
}

std::vector<float>* KeyValues::getFloatV(const String key) {
    return this->getFloatV(PQfnumber(select->res, key.c_str()));
}
//TODO: toto dodelat
std::vector<float>* KeyValues::getFloatV(const int col) {
    PGarray tmp;
    if (! PQgetf(select->res, this->pos, "%float4[]", col, &tmp)) {
        warning(308, "Value is not an array of float");
        return NULL;
    }

    PGfloat4 value;
    std::vector<float>* values = new std::vector<float>;
    for (int i = 0; i < PQntuples(tmp.res); i++) {
        if (! PQgetf(tmp.res, i, "%float4", 0, &value)) {
            warning(309, "Unexpected value in float array");
            destruct (values);
            return NULL;
        }
        values->push_back(value);
    }
    PQclear(tmp.res);

    return values;
}

// =============== GETTERS - TIMESTAMP =========================================
struct tm KeyValues::getTimestamp(const String key) {
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
        stringstream wss;
        wss << "Data type " << dtype << " not yet supported";
        warning(310, wss.str());
    }
    return ts;
}

// =============== GETTERS - OTHER =============================================
String KeyValues::getName(const String key) {
    PGtext value = (PGtext) "";

    PQgetf(select->res, this->pos, "#name", key.c_str(), &value);

    if (value == NULL) {
        value = (PGtext) "";
    }

    return (String) value;
}

int KeyValues::getIntOid(const String key) {
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