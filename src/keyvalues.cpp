/**
 * @file    commons.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of KeyValues class
 */


#include <vtapi_global.h>
#include <common/vtapi_tkey.h>
#include <data/vtapi_keyvalues.h>

using namespace vtapi;


//================================ KEYVALUES ===================================


KeyValues::KeyValues(const Commons& orig)
          : Commons(orig), select(NULL), insert(NULL), update(NULL) {
    thisClass = "KeyValues(Commons&)";
}

KeyValues::KeyValues(const KeyValues& orig, const string& selection)
          : Commons(orig), select(NULL), insert(NULL), update(NULL) {
    thisClass = "KeyValues(KeyValues&)";
    this->selection = selection;
}

KeyValues::~KeyValues() {
    // whether should be something inserted
    if (insert) {
        if (!insert->executed) {
            logger->warning(313, "There should be something inserted: \n" + insert->getQuery(), thisClass+"~KeyValues()");
        }
        destruct(insert);
    }

    // whether should be something updated
    if (update) {
        if (!update->executed) {
            logger->warning(314, "There should be something updated: \n" + update->getQuery(), thisClass+"~KeyValues()");
        }
        destruct(update);
    }

    destruct (select);
}

KeyValues* KeyValues::next() {
    int rowCount    = 0;

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
    if (!select) logger->error(301, "There is no select class", thisClass+"next()");

    // select is executed here when position == -1
    if (select->resultSet->getPosition() == -1) {                
        if (!select->execute()) return NULL;
    }

    // check if end of resultset has been reached
    rowCount = select->resultSet->countRows();
    if (rowCount <= 0) return NULL;
    else if (select->resultSet->getPosition() < rowCount) {
        select->resultSet->step();
        if (select->resultSet->getPosition() == rowCount) { // this was last row, reset
            select->resultSet->setPosition(-1);
            return NULL;
        }
    }
    else if (select->resultSet->getPosition() >= select->getLimit()) {     // fetch new resultset
        if (select->executeNext()) {
            select->resultSet->setPosition(0);
        }
    }
    else return NULL;   // end of result
    
    return this;
}

TKey KeyValues::getKey(int col) {
    return select->resultSet->getKey(col);
}

TKeys* KeyValues::getKeys() {
    return select->resultSet->getKeys();
}


// =============== GETTERS (Select) ============================================
//TODO: optimalize getters, keys and metadata are sometimes retrieved twice

/**
 * Generic getter - fetches any value from resultset and returns it as string
 * @param col column index
 * @param arrayLimit limits size of elements of returned array
 * @return string representation of field value
 */
string KeyValues::getValue(const int col, const int arrayLimit) {

    return select->resultSet->getValue(col, arrayLimit);
}

// =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===================
char KeyValues::getChar(const string& key) {
    return select->resultSet->getChar(key);
}
char KeyValues::getChar(const int col) {
    return select->resultSet->getChar(col);
}
char *KeyValues::getCharA(const string& key, int& size) {
    return select->resultSet->getCharA(key, size);
}
char *KeyValues::getCharA(const int col, int& size) {
    return select->resultSet->getCharA(col, size);
}
string KeyValues::getString(const string& key) {
    return select->resultSet->getString(key);
}
string KeyValues::getString(const int col) {
    return select->resultSet->getString(col);
}

// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================
int KeyValues::getInt(const string& key) {
    return select->resultSet->getInt(key);
}
int KeyValues::getInt(const int col) {
    return select->resultSet->getInt(col);
}
long KeyValues::getInt8(const string& key) {
    return select->resultSet->getInt8(key);
}
long KeyValues::getInt8(const int col) {
    return select->resultSet->getInt8(col);
}
int* KeyValues::getIntA(const string& key, int& size) {
    return select->resultSet->getIntA(key, size);
}
int* KeyValues::getIntA(const int col, int& size) {
    return select->resultSet->getIntA(col, size);
}
vector<int>* KeyValues::getIntV(const string& key) {
    return select->resultSet->getIntV(key);
}
vector<int>* KeyValues::getIntV(const int col) {
    return select->resultSet->getIntV(col);
}
vector< vector<int>* >* KeyValues::getIntVV(const int col) {
    return select->resultSet->getIntVV(col);
}
vector< vector<int>* >* KeyValues::getIntVV(const string& key) {
    return select->resultSet->getIntVV(key);
}

// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================
float KeyValues::getFloat(const string& key) {
    return select->resultSet->getFloat(key);
}
float KeyValues::getFloat(const int col) {
    return select->resultSet->getFloat(col);
}
double KeyValues::getFloat8(const string& key) {
    return select->resultSet->getFloat8(key);
}
double KeyValues::getFloat8(const int col) {
    return select->resultSet->getFloat8(col);
}
float* KeyValues::getFloatA(const string& key, int& size) {
    return select->resultSet->getFloatA(key, size);
}
float* KeyValues::getFloatA(const int col, int& size) {
    return select->resultSet->getFloatA(col, size);
}
vector<float>* KeyValues::getFloatV(const string& key) {
    return select->resultSet->getFloatV(key);
}
vector<float>* KeyValues::getFloatV(const int col) {
    return select->resultSet->getFloatV(col);
}

// =============== GETTERS - OpenCV MATRICES ===============================
#ifdef __OPENCV_CORE_C_H__XXX

CvMat *KeyValues::getCvMat(const string& key) {
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
    else data_oid = typeManager->getElemOID(PQftype(select->res, dataloc_col));

    // could be char, short, int, float, double
    if (data_oid == typeManager->toOid("char")) {
        //TODO: maybe fix alignment (every row to 4B) ?
        data = getCharA(dataloc_col, data_len);
    }
    else if (data_oid == typeManager->toOid("float4") ||
            data_oid == typeManager->toOid("real")) {
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

CvMatND *KeyValues::getCvMatND(const string& key) {
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
    else data_oid = typeManager->getElemOID(PQftype(select->res, dataloc_col));

    // could be char, short, int, float, double
    if (data_oid == typeManager->toOid("char")) {
        //TODO: maybe fix alignment (every row to 4B) ?
        //TODO: not sure if sizes are counted correctly
        data = getCharA(dataloc_col, data_len);
        for (int i = 0; i < step_size; i++)
            sizes[i] = data_len / step[i];
    }
    else if (data_oid == typeManager->toOid("float4") ||
            data_oid == typeManager->toOid("real")) {
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
time_t KeyValues::getTimestamp(const string& key) {
    return select->resultSet->getTimestamp(key);
}
time_t KeyValues::getTimestamp(const int col) {
    return select->resultSet->getTimestamp(col);
}

// =============== GETTERS - GEOMETRIC TYPES ===============================
#ifdef POSTGIS
PGpoint KeyValues::getPoint(const string& key) {
    return this->getPoint(PQfnumber(select->res, key.c_str()));
}
PGpoint KeyValues::getPoint(const int col) {
    PGpoint point;
    memset(&point, 0, sizeof(PGpoint));
    if (! PQgetf(select->res, this->pos, "%point", col, &point)) {
        logger->warning(314, "Value is not a point");
    }
    return point;
}
PGlseg KeyValues::getLineSegment(const string& key) {
    return this->getLineSegment(PQfnumber(select->res, key.c_str()));
}
PGlseg KeyValues::getLineSegment(const int col){
    PGlseg lseg;
    memset(&lseg, 0, sizeof(PGlseg));
    if (! PQgetf(select->res, this->pos, "%lseg", col, &lseg)) {
        logger->warning(315, "Value is not a line segment");
    }
    return lseg;
}

PGbox KeyValues::getBox(const string& key){
    return this->getBox(PQfnumber(select->res, key.c_str()));
}
PGbox KeyValues::getBox(const int col){
    PGbox box;
    memset(&box, 0, sizeof(PGbox));
    if (! PQgetf(select->res, this->pos, "%box", col, &box)) {
        logger->warning(316, "Value is not a box");
    }
    return box;
}

PGcircle KeyValues::getCircle(const string& key){
    return this->getCircle(PQfnumber(select->res, key.c_str()));

}
PGcircle KeyValues::getCircle(const int col){
    PGcircle circle;
    memset(&circle, 0, sizeof(PGcircle));
    if (! PQgetf(select->res, this->pos, "%circle", col, &circle)) {
        logger->warning(317, "Value is not a circle");
    }
    return circle;
}

PGpolygon KeyValues::getPolygon(const string& key){
    return this->getPolygon(PQfnumber(select->res, key.c_str()));

}
PGpolygon KeyValues::getPolygon(const int col){
    PGpolygon polygon;
    memset(&polygon, 0, sizeof(PGpolygon));
    if (! PQgetf(select->res, this->pos, "%polygon", col, &polygon)) {
        logger->warning(318, "Value is not a polygon");
    }
    return polygon;
}

PGpath KeyValues::getPath(const string& key){
    return this->getPath(PQfnumber(select->res, key.c_str()));

}
PGpath KeyValues::getPath(const int col){
    PGpath path;
    memset(&path, 0, sizeof(PGpath));
    if (! PQgetf(select->res, this->pos, "%path", col, &path)) {
        logger->warning(319, "Value is not a path");
    }
    return path;
}

PGcube KeyValues::getCube(const string& key) {
    return this->getCube(PQfnumber(select->res, key.c_str()));
}
PGcube KeyValues::getCube(const int col) {
    PGcube cube;
    memset(&cube, 0, sizeof(PGcube));
    if (! PQgetf(select->res, this->pos, "%cube", col, &cube)) {
        logger->warning(320, "Value is not a cube");
    }
    return cube;
}

GEOSGeometry *KeyValues::getGeometry(const string& key) {
    return this->getGeometry(PQfnumber(select->res, key.c_str()));
}
GEOSGeometry *KeyValues::getGeometry(const int col) {
    GEOSGeometry *geo;
    if (! PQgetf(select->res, this->pos, "%geometry", col, &geo)) {
        logger->warning(321, "Value is not a geometry type");
    }
    return geo;
}

GEOSGeometry *KeyValues::getLineString(const string& key) {
    return this->getLineString(PQfnumber(select->res, key.c_str()));
}
GEOSGeometry *KeyValues::getLineString(const int col) {
    GEOSGeometry *ls;
    if (! PQgetf(select->res, this->pos, "%geometry", col, &ls)) {
        logger->warning(322, "Value is not a geometry type");
    }
    else if (ls && GEOSGeomTypeId(ls) != GEOS_LINESTRING) {
        logger->warning(323, "Value is not a linestring");
    }
    return ls;
}

vector<PGpoint>*  KeyValues::getPointV(const string& key) {
    return this->getPointV(PQfnumber(select->res, key.c_str()));
}
vector<PGpoint>*  KeyValues::getPointV(const int col) {
    PGarray tmp;
    if (! PQgetf(select->res, this->pos, "%point[]", col, &tmp)) {
        logger->warning(324, "Value is not an array of points");
        return NULL;
    }

    PGpoint value;
    vector<PGpoint>* values = new vector<PGpoint>;

    for (int i = 0; i < PQntuples(tmp.res); i++) {
        if (! PQgetf(tmp.res, i, "%point", 0, &value)) {
            logger->warning(325, "Unexpected value in point array");
            PQclear(tmp.res);
            destruct (values);
            return NULL;
        }
        values->push_back(value);
    }
    PQclear(tmp.res);

    return values;
}
#endif

// =============== GETTERS - OTHER =============================================
int KeyValues::getIntOid(const string& key) {
    return select->resultSet->getIntOid(key);
}
int KeyValues::getIntOid(const int col) {
    return select->resultSet->getIntOid(col);
}

// =============== PRINT methods =======================================

bool KeyValues::print() {
    if (!select || !select->resultSet->isOk() || select->resultSet->getPosition() < 0) {
        logger->warning(302, "There is nothing to print (see other messages)", thisClass+"::print()");
        return VT_FAIL;
    }
    else {
        int origpos = select->resultSet->getPosition();
        int get_widths = (format == STANDARD);
        pair< TKeys*, vector<int>* > fInfo = select->resultSet->getKeysWidths(select->resultSet->getPosition(), get_widths, arrayLimit);
        if (fInfo.first && (format != STANDARD || fInfo.second)) {
            printHeader(fInfo);
            printRowOnly(select->resultSet->getPosition(), fInfo.second);
            printFooter(1);
        }
        else cout << "(empty)" << endl;
        destruct(fInfo.first); destruct(fInfo.second);
        select->resultSet->setPosition(origpos);
        return VT_OK;
    }
}

bool KeyValues::printAll() {
    if (!select || !select->resultSet->isOk()) {
        logger->warning(303, "There is nothing to print (see other messages)", thisClass+"::printAll()");
        return VT_FAIL;
    }
    else {
        int origpos = select->resultSet->getPosition();
        int get_widths = (format == STANDARD);
        pair< TKeys*, vector<int>* > fInfo = select->resultSet->getKeysWidths(-1, get_widths, arrayLimit);
        if (fInfo.first && (format != STANDARD || fInfo.second)) {
            printHeader(fInfo);
            for (int r = 0; r < select->resultSet->countRows(); r++) printRowOnly(r, fInfo.second);
            printFooter(select->resultSet->countRows());
        }
        else cout << "(empty)" << endl;
        destruct(fInfo.first); destruct(fInfo.second);
        select->resultSet->setPosition(origpos);
        return VT_OK;
    }
}

// =============== PRINT support methods =======================================

bool KeyValues::printHeader(const pair< TKeys*, vector<int>* > fInfo) {
    bool retval = VT_OK;
    stringstream table, nameln, typeln, border;
    int cols = select->resultSet->countCols();

    if (!fInfo.first || !fInfo.second || cols != fInfo.first->size() || cols != fInfo.second->size()) {
        return VT_FAIL;
    }

    if (format == HTML) {
        if (tableOpt.empty()) table << "<table>";
        else table << "<table " << tableOpt << ">";
        if (!caption.empty()) table << "<caption align=\"top\">" << caption << "</caption>";
        table << endl << "<tr align=\"center\">";
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
    table << "</tr>" << endl; nameln << endl; typeln << endl; border << endl;
    if (format == STANDARD) cout << nameln.str() << typeln.str() << border.str();
    else if (format == CSV) cout << nameln.str();
    else if (format == HTML) cout << table.str();

    return VT_OK;
}

bool KeyValues::printFooter(const int count) {
    stringstream output;
    if (format == STANDARD) {
        int rows = select->resultSet->countRows();
        if (count > 0) output << "(" << count << " of " << rows << " rows)" << endl;
        else output << "(" << rows << " rows)" << endl;
    }
    else if (format == HTML) output << "</table>" << endl;
    cout << output.str();

    return VT_OK;
}

bool KeyValues::printRowOnly(const int row, const vector<int>* widths) {
    bool retval = VT_OK;
    stringstream output;
    int cols = select->resultSet->countCols();

    // don't forget to save original value of pos beforehand!
    select->resultSet->setPosition(row);
    if (format == HTML) output << "<tr>";
    for (int c = 0; c < cols; c++) {
        // general getter
        string val = getValue(c, arrayLimit);
        if (format == STANDARD) {
            output << std::left << std::setw((*widths)[c]) << val;
            if (c < cols-1) output << '|';
        }
        else if (format == CSV) {
            if (val.find(',') != string::npos) output << "\"" << val << "\"";
            else output << val;
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


// =============== SETTERS (Update) ============================================
// TODO: mozna by se dalo premyslet o PQsetvalue

bool KeyValues::preSet() {
    // TODO: tohle by se v budoucnu melo dat za pomoci system_catalog
    logger->warning(3010, "Set inherited from KeyValues at class " + thisClass, thisClass+"::preSet()");

    destruct(update);
    update = new Update(*this);

    return update ? VT_OK : VT_FAIL;
}

// TODO: how to change binary data???
bool KeyValues::setString(const string& key, const string& value) {
    // call preset on the derived class
    if (!update) this->preSet();

    /* 
    if (select) {
        char* tempc = const_cast<char*>(value.c_str()); // stupid C conversions
        PQsetvalue(select->res, pos, PQfnumber(select->res, key.c_str()), tempc, value.length());
    } */

    return update->setString(key, value);
}

bool KeyValues::setInt(const string& key, int value) {
    // call preset on the derived class
    if (!update) this->preSet();
    return update->setInt(key, value);
}

bool KeyValues::setIntA(const string& key, int* values, int size){
    // call preset on the derived class
    if (!update) this->preSet();
    return update->setIntA(key, values, size);
}

bool KeyValues::setFloat(const string& key, float value){
    // call preset on the derived class
    if (!update) this->preSet();
    return update->setFloat(key, value);
}

bool KeyValues::setFloatA(const string& key, float* values, int size){
    // call preset on the derived class
    if (!update) this->preSet();
    return update->setFloatA(key, values, size);
}

bool KeyValues::setExecute() {
    if (update) return this->update->execute();
    else return VT_FAIL;
}


// =================== ADDERS (Insert) =========================================
bool KeyValues::addExecute() {
    if (this->insert) return this->insert->execute();
    else return VT_FAIL;
}

// =================== CHECKERS (Storage) ======================================
bool KeyValues::checkStorage() {
    logger->warning(3018, "Check might fail at class " + thisClass, thisClass+"::checkStorage()");

    if (!sequence.empty() && fileExists(getDataLocation())) return VT_OK;
    else if (!dataset.empty() && fileExists(getDataLocation())) return VT_OK;

    return VT_FAIL;
}


