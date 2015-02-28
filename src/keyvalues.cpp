/**
 * @file
 * @brief   Methods of KeyValues class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <common/vtapi_global.h>
#include <common/vtapi_tkey.h>
#include <data/vtapi_keyvalues.h>

using std::string;
using std::stringstream;
using std::vector;
using std::pair;
using std::cout;
using std::cerr;
using std::endl;

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
        vt_destruct(insert);
    }

    // whether should be something updated
    if (update) {
        if (!update->executed) {
            logger->warning(314, "There should be something updated: \n" + update->getQuery(), thisClass+"~KeyValues()");
        }
        vt_destruct(update);
    }

    vt_destruct(select);
}

KeyValues* KeyValues::next() {
    int rowCount    = 0;

    // whether should be something inserted
    if (insert) {
        if (!insert->executed) insert->execute();  // FIXME: here should be the store fun instead
        vt_destruct(insert);
    }

    // whether should be something updated
    if (update) {
        if (!update->executed) update->execute();  // FIXME: here should be the store fun instead
        vt_destruct(update);
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
#if HAVE_OPENCV

cv::Mat *KeyValues::getCvMat(const string& key) {
    return select->resultSet->getCvMat(key);
}

cv::Mat *KeyValues::getCvMat(const int col) {
    return select->resultSet->getCvMat(col);
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
#if HAVE_POSTGRESQL
PGpoint KeyValues::getPoint(const string& key) {
    return select->resultSet->getPoint(key);
}
PGpoint KeyValues::getPoint(const int col) {
    return select->resultSet->getPoint(col);
}
vector<PGpoint>*  KeyValues::getPointV(const string& key) {
    return select->resultSet->getPointV(key);
}
vector<PGpoint>*  KeyValues::getPointV(const int col) {
    return select->resultSet->getPointV(col);
}
#endif

//PGlseg KeyValues::getLineSegment(const string& key) {
//    return this->getLineSegment(PQfnumber(select->res, key.c_str()));
//}
//PGlseg KeyValues::getLineSegment(const int col){
//    PGlseg lseg;
//    memset(&lseg, 0, sizeof(PGlseg));
//    if (! PQgetf(select->res, this->pos, "%lseg", col, &lseg)) {
//        logger->warning(315, "Value is not a line segment");
//    }
//    return lseg;
//}
//
//PGbox KeyValues::getBox(const string& key){
//    return this->getBox(PQfnumber(select->res, key.c_str()));
//}
//PGbox KeyValues::getBox(const int col){
//    PGbox box;
//    memset(&box, 0, sizeof(PGbox));
//    if (! PQgetf(select->res, this->pos, "%box", col, &box)) {
//        logger->warning(316, "Value is not a box");
//    }
//    return box;
//}
//
//PGcircle KeyValues::getCircle(const string& key){
//    return this->getCircle(PQfnumber(select->res, key.c_str()));
//
//}
//PGcircle KeyValues::getCircle(const int col){
//    PGcircle circle;
//    memset(&circle, 0, sizeof(PGcircle));
//    if (! PQgetf(select->res, this->pos, "%circle", col, &circle)) {
//        logger->warning(317, "Value is not a circle");
//    }
//    return circle;
//}
//
//PGpolygon KeyValues::getPolygon(const string& key){
//    return this->getPolygon(PQfnumber(select->res, key.c_str()));
//
//}
//PGpolygon KeyValues::getPolygon(const int col){
//    PGpolygon polygon;
//    memset(&polygon, 0, sizeof(PGpolygon));
//    if (! PQgetf(select->res, this->pos, "%polygon", col, &polygon)) {
//        logger->warning(318, "Value is not a polygon");
//    }
//    return polygon;
//}
//PGpath KeyValues::getPath(const string& key){
//    return this->getPath(PQfnumber(select->res, key.c_str()));
//
//}
//PGpath KeyValues::getPath(const int col){
//    PGpath path;
//    memset(&path, 0, sizeof(PGpath));
//    if (! PQgetf(select->res, this->pos, "%path", col, &path)) {
//        logger->warning(319, "Value is not a path");
//    }
//    return path;
//}

#if HAVE_POSTGIS
GEOSGeometry *KeyValues::getGeometry(const string& key) {
    return select->resultSet->getGeometry(key);
}
GEOSGeometry *KeyValues::getGeometry(const int col) {
    return select->resultSet->getGeometry(col);
}

GEOSGeometry *KeyValues::getLineString(const string& key) {
    return select->resultSet->getLineString(key);
}
GEOSGeometry *KeyValues::getLineString(const int col) {
    return select->resultSet->getLineString(col);
}
#endif

IntervalEvent *KeyValues::getIntervalEvent(const std::string& key) {
    return select->resultSet->getIntervalEvent(key);
}

IntervalEvent *KeyValues::getIntervalEvent(const int col) {
    return select->resultSet->getIntervalEvent(col);
}
    
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
        vt_destruct(fInfo.first); vt_destruct(fInfo.second);
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
        vt_destruct(fInfo.first); vt_destruct(fInfo.second);
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
    
    return retval;
}


// =============== SETTERS (Update) ============================================
// TODO: mozna by se dalo premyslet o PQsetvalue

bool KeyValues::preSet() {
    // TODO: tohle by se v budoucnu melo dat za pomoci system_catalog
    logger->warning(3010, "Set inherited from KeyValues at class " + thisClass, thisClass+"::preSet()");

    vt_destruct(update);
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
bool KeyValues::addString(const std::string& key, const std::string& value) {
    return this->insert ? this->insert->keyString(key, value) : VT_FAIL;
}
bool KeyValues::addInt(const std::string& key, int value) {
    return this->insert ? this->insert->keyInt(key, value) : VT_FAIL;
}
bool KeyValues::addIntA(const std::string& key, int* value, int size) {
    return this->insert ? this->insert->keyIntA(key, value, size) : VT_FAIL;
}
bool KeyValues::addFloat(const std::string& key, float value) {
    return this->insert ? this->insert->keyFloat(key, value) : VT_FAIL;
}
bool KeyValues::addFloatA(const std::string& key, float* value, int size) {
    return this->insert ? this->insert->keyFloatA(key, value, size) : VT_FAIL;
}
#if HAVE_OPENCV
bool KeyValues::addCvMat(const std::string& key, cv::Mat& value) {
    return this->insert ? this->insert->keyCvMat(key, value) : VT_FAIL;
}
#endif
    
bool KeyValues::addIntervalEvent(const std::string& key, IntervalEvent& value) {
    return this->insert ? this->insert->keyIntervalEvent(key, value) : VT_FAIL;
}

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


