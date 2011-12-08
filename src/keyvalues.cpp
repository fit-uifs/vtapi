/*
 * File:   keyvalues.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:52
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>

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

// =============== PRINT methods =======================================

/**
 * Prints currently selected row in resultset (Select)
 */
void KeyValues::print() {
    if (!select || !select->res || pos<0) warning(302, "There is nothing to print (see other messages)");
    else {
        std::vector< pair<datatype_t,int> > fInfo = getFieldsInfo(select->res, pos);
        printHeader(select->res, fInfo);
        printRowOnly(select->res, pos, fInfo);
        printFooter(select->res, 1);
    }
}
/**
 * Prints all rows in resultset (Select)
 */
void KeyValues::printAll() {
    if (!select) warning(303, "There is nothing to print (see other messages)");
    else {
        int origpos = this->pos;
        int rows = PQntuples(select->res);
        std::vector< pair<datatype_t,int> > fInfo = getFieldsInfo(select->res);
        printHeader(select->res, fInfo);
        for (int i = 0; i < rows; i++) printRowOnly(select->res, i, fInfo);
        printFooter(select->res);
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
void KeyValues::printHeader(PGresult* res, const std::vector< pair<datatype_t,int> >& fInfo) {
    std::stringstream table, nameln, typeln, border;
    int cols = PQnfields(res);

    if (format == HTML) {
        if (tableOpt.empty()) table << "<table>";
        else table << "<table " << tableOpt << ">";
        if (!caption.empty()) table << "<caption align=\"top\">" << caption << "</caption>";
        table << endl << "<tr align=\"center\">";
    }

    for (int c = 0; c < cols; c++) {
        if (format == STANDARD) {
            nameln << left << setw(fInfo[c].second) << PQfname(res, c);
            typeln << left << setw(fInfo[c].second) << typemap->toTypname(PQftype(res, c));
            border << setfill('-') << setw(fInfo[c].second) << "";
            if (c < cols-1) {
                nameln << '|'; typeln << '|'; border << '+';
            }
        }
        else if (format == CSV) {
            nameln << PQfname(res,c);
            if (c < cols-1) nameln << ',';
        }
        else if (format == HTML) {
            table << "<th>" << PQfname(res, c) << "<br/>";
            table << typemap->toTypname(PQftype(res, c)) << "</th>";
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
void KeyValues::printFooter(PGresult* res, const int count) {
    std::stringstream output;
    if (format == STANDARD) {
        int rows = PQntuples(res);
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
 * @param fInfo
 * //param widths Vector of column widths
 * @todo param fInfo in documentation
 */
void KeyValues::printRowOnly(PGresult* res, const int row,
        const std::vector< pair<datatype_t,int> >& fInfo) {
    std::stringstream output;
    int cols = PQnfields(res);
    //TODO: tohle je, protoze gettery vzdy berou select->res, ne obecny resultset
    this->pos = row;

    if (format == HTML) output << "<tr>";
    for (int c = 0; c < cols; c++) {
        // obecny getter
        String val = getValue(c, fInfo[c].first);
        if (format == STANDARD) {
            output << left << setw(fInfo[c].second) << val;
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
 * Returns data type and desired column width for all fields
 * @param res Input resultset
 * @param row Row number to process (0 = all)
 * @return Vector of column pairs <datatype,width>
 */
std::vector< pair<KeyValues::datatype_t,int> >
KeyValues::getFieldsInfo(PGresult* res, const int row) {
    std::vector< pair<datatype_t,int> > fInfo;
    int plen, flen, tlen, width;
    String typestr;
    String value;
    int rows = PQntuples(res);
    int cols = PQnfields(res);

    // header and first row
    for (int c = 0; c < cols; c++) {
        //TODO: tohle je, protoze gettery vzdy berou select->res, ne obecny resultset
        pos = row < 0 ? 0 : row;
        typestr = toTypname(PQftype(res, c));
        flen = String(PQfname(res, c)).length();
        tlen = typestr.length();
        value = getValue(c, recognizeType(typestr));
        plen = value.length();
        if (plen >= flen && plen >= tlen) width = plen;
        else if (flen >= plen && flen >= tlen) width = flen;
        else width = tlen;
        fInfo.push_back(make_pair(recognizeType(typestr),width));
    }
    // rest of the rows
    if (row < 0) {
        for (int r = 1; r < rows; r++) {
            //TODO: tohle je, protoze gettery vzdy berou select->res, ne obecny resultset
            pos = r;
            for (int c = 0; c < cols; c++) {
                value = getValue(c, fInfo[c].first);
                plen = value.length();
                if (plen > fInfo[c].second) fInfo[c].second = plen;
            }
        }
    }
    return fInfo;
}

//TODO: zrejme docasna funkce, nez manipulaci s typy nejak rozumne integrujem
KeyValues::datatype_t KeyValues::recognizeType(const String& typestr) {
    if (!typestr.compare("int2")) return INT2;
    else if (!typestr.compare("int4")) return INT4;
    else if (!typestr.compare("int8")) return INT8;
    else if (!typestr.compare("oid")) return OID;
    else if (!typestr.compare("inouttype")) return INOUTTYPE;
    else if (!typestr.compare("float4")) return FLOAT4;
    else if (!typestr.compare("float8")) return FLOAT8;
    else if (!typestr.compare("double")) return DOUBLE;
    else if (!typestr.compare("_int2")) return _INT2;
    else if (!typestr.compare("_int4")) return _INT4;
    else if (!typestr.compare("_int8")) return _INT8;
    else if (!typestr.compare("_float4")) return _FLOAT4;
    else if (!typestr.compare("_float8")) return _FLOAT8;
    else if (!typestr.compare("_double")) return _DOUBLE;
    else if (!typestr.compare("numeric")) return NUMERIC;
    else if (!typestr.compare("bpchar")) return BPCHAR;
    else if (!typestr.compare("varchar")) return VARCHAR;
    else if (!typestr.compare("character varying")) return VARCHAR;
    else if (!typestr.compare("name")) return NAME;
    else if (!typestr.compare("text")) return TEXT;
    else if (!typestr.compare("bytea")) return BYTEA;
    else if (!typestr.compare("seqtype")) return SEQTYPE;
    else if (!typestr.compare("regclass")) return REGCLASS;
    else if (!typestr.compare("timestamp")) return TIMESTAMP;
}
// TODO: opet zrejme docasna funkce, obecny getter
String KeyValues::getValue(const int field, const datatype_t typ) {
    std::stringstream valss;
    if (typ>INTBEGIN && typ<INTEND)
        valss << this->getInt(field);
    else if (typ>FLOATBEGIN && typ<FLOATEND)
        valss << this->getFloat(field);
    else if (typ>IARRAYBEGIN && typ<IARRAYEND) {
        std::vector<int> arr = this->getIntV(field);
        for (int i = 0; i < arr.size(); i++) {
            valss << arr[i];
            if (i < arr.size()-1) valss << ",";
        }
    }
    else if (typ>FARRAYBEGIN && typ<FARRAYEND){
        std::vector<float> arr = this->getFloatV(field);
        for (int i = 0; i < arr.size(); i++) {
            valss << arr[i];
            if (i < arr.size()-1) valss << ",";
        }
    }
    else if (typ>STRINGBEGIN && typ<STRINGEND)
        valss << this->getString(field);
    else if (typ == TIMESTAMP) {
       struct tm ts = this->getTimestamp(field);
       valss << right << setfill('0');
       valss << ts.tm_year << '-' << setw(2) << ts.tm_mon << '-' << setw(2) << ts.tm_mday << ' ';
       valss << setw(2) << ts.tm_hour << ':' << setw(2) << ts.tm_min << ':' << setw(2) << ts.tm_sec;
    }
    
    return valss.str();    
}


// =============== GETTERS (Select) ============================================
// =============== GETTERS FOR STRINGS =========================================
String KeyValues::getString(const String& key) {
    return this->getString(PQfnumber(select->res, key.c_str()));
}


// TODO: this getter is experimental
String KeyValues::getString(int position) {
    PGtext value = (PGtext) "";
    
    datatype_t typ = recognizeType(toTypname(PQftype(select->res, position)));
    // Several data types are other representation of string, so we must catch all of them
    switch (typ) {
        case NUMERIC: PQgetf(select->res, pos, "%numeric", position, &value); break;
        case TEXT: PQgetf(select->res, pos, "%text", position, &value); break;
        case NAME: PQgetf(select->res, pos, "%name", position, &value); break;
        case VARCHAR: PQgetf(select->res, pos, "%varchar", position, &value); break;
        case BYTEA: PQgetf(select->res, pos, "%bytea", position, &value); break;
        case BPCHAR: PQgetf(select->res, pos, "%bpchar", position, &value); break;
        case REGCLASS: PQgetf(select->res, pos, "%varchar", position, &value); break;
        case SEQTYPE: PQgetf(select->res, pos, "%varchar", position, &value); break;
        case OID: value = (PGtext) this->toTypname(this->getInt(position)).c_str(); break;
        // FIXME: treti typ?
        case INOUTTYPE: value = (PGtext) ((this->getInt(position)) ? "out" : "in"); break;
        default:
            warning(304,"Type of (" + toString(position) + ") is not a string");
//            this->print();
    }
       
    return value ? String(value) : "";
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
    else if (typname.compare("int8") == 0) { // TODO: make a long type???
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
//        this->print();
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
//        this->print();
    }

    size = PQntuples(tmp.res);
    values = new int(size);
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &values[i])) {
            warning(307, "Unexpected value in integer array");
//            this->print();

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
//        this->print();
    }

    for (int i = 0; i < PQntuples(tmp.res); i++) {
        if (! PQgetf(tmp.res, i, "%int4", 0, &value)) {
            warning(309, "Unexpected value in integer array");
//            this->print();
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
//        this->print();
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
//        this->print();
    }

    size = PQntuples(tmp.res);
    values = new float(size);
    for (int i = 0; i < size; i++) {
        if (! PQgetf(tmp.res, i, "%float4", 0, &values[i])) {
            warning(312, "Unexpected value in float array");
//            this->print();
        }
    }
    PQclear(tmp.res);

    return values;
}

std::vector<float> KeyValues::getFloatV(const String& key) {
    return this->getFloatV(PQfnumber(select->res, key.c_str()));
}

std::vector<float> KeyValues::getFloatV(int position) {
    PGarray tmp;
    PGfloat4 value;
    std::vector<float> values;

    if (! PQgetf(select->res, this->pos, "%float4[]", position, &tmp)) {
        warning(308, "Value is not an array of float");
//        this->print();
    }

    for (int i = 0; i < PQntuples(tmp.res); i++) {
        if (! PQgetf(tmp.res, i, "%float4", 0, &value)) {
            warning(309, "Unexpected value in float array");
//            this->print();
        }
        values.push_back(value);
    }
    PQclear(tmp.res);

    return values;
}

// =============== GETTERS - TIMESTAMP =========================================
struct tm KeyValues::getTimestamp(const String& key) {
    return this->getTimestamp(PQfnumber(select->res, key.c_str()));
}
struct tm KeyValues::getTimestamp(int position) {
    struct tm ts = {0};
    PGtimestamp timestamp;
    if (! PQgetf(select->res, this->pos, "%timestamp", position, &timestamp)) {
        warning(310, "Value is not a timestamp");
//        this->print();
    }
    else {
    ts.tm_year  = timestamp.date.year;
    ts.tm_mon   = timestamp.date.mon;
    ts.tm_mday  = timestamp.date.mday;
    ts.tm_hour  = timestamp.time.hour;
    ts.tm_min   = timestamp.time.min;
    ts.tm_sec   = timestamp.time.sec;
    }

    return ts;
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