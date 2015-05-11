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
using std::list;
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

KeyValues::~KeyValues()
{
    for (list<Insert *>::iterator it = store.begin(); it != store.end(); it++) {
        if (!(*it)->executed) {
            logger->warning(313, "INSERT operation was not executed:\n" + (*it)->getQuery(), thisClass + "~KeyValues()");
        }
        delete (*it);
    }
    
    // whether should be something inserted
    if (insert) {
        if (!insert->executed) {
            logger->warning(313, "INSERT operation was not executed:\n" + insert->getQuery(), thisClass + "~KeyValues()");
        }
        vt_destruct(insert);
    }

    // whether should be something updated
    if (update) {
        if (!update->executed) {
            logger->warning(314, "UPDATE operation was not executed:\n" + update->getQuery(), thisClass+"~KeyValues()");
        }
        vt_destruct(update);
    }

    vt_destruct(select);
}

KeyValues* KeyValues::next()
{
    int rowCount = 0;

    // whether should be something inserted
    if (insert) {
        addExecute();
    }

    // whether should be something updated
    if (update) {
        setExecute();
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

int KeyValues::count()
{
    if (!select) logger->error(301, "There is no select class", thisClass + "::count()");

    int cnt = -1;
    
    std::string query   = select->queryBuilder->getCountQuery();
    void *param         = select->queryBuilder->getQueryParam();
    void *paramDup      = select->queryBuilder->duplicateQueryParam(param);
    ResultSet *res      = BackendFactory::createResultSet(backend, *backendBase, connection->getDBTypes());

    logger->debug("Count query: " + query);
    
    if (connection->fetch(query, paramDup, res) > 0) {
        res->step();
        cnt = res->getInt8(0);
    }
    
    if (paramDup) select->queryBuilder->destroyQueryParam(paramDup);
    if (res) delete res;

    return cnt;
}

TKey KeyValues::getKey(int col) {
    return select->resultSet->getKey(col);
}

TKeys* KeyValues::getKeys() {
    return select->resultSet->getKeys();
}


// =============== GETTERS (Select) ============================================

/**
 * Generic getter - fetches any value from resultset and returns it as string
 * @param col column index
 * @param arrayLimit limits size of elements of returned array
 * @return string representation of field value
 */
string KeyValues::getValue(const int col, const int arrayLimit)
{
    return select->resultSet->getValue(col, arrayLimit);
}

// =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===================
char KeyValues::getChar(const string& key) {
    return select->resultSet->getChar(key);
}
char KeyValues::getChar(const int col) {
    return select->resultSet->getChar(col);
}
string KeyValues::getString(const string& key) {
    return select->resultSet->getString(key);
}
string KeyValues::getString(const int col) {
    return select->resultSet->getString(col);
}

// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================
bool KeyValues::getBool(const string& key) {
    return select->resultSet->getBool(key);
}
bool KeyValues::getBool(const int col) {
    return select->resultSet->getBool(col);
}
int KeyValues::getInt(const string& key) {
    return select->resultSet->getInt(key);
}
int KeyValues::getInt(const int col) {
    return select->resultSet->getInt(col);
}
long long KeyValues::getInt8(const string& key) {
    return select->resultSet->getInt8(key);
}
long long KeyValues::getInt8(const int col) {
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
long long* KeyValues::getInt8A(const string& key, int& size) {
    return select->resultSet->getInt8A(key, size);
}
long long* KeyValues::getInt8A(const int col, int& size) {
    return select->resultSet->getInt8A(col, size);
}
vector<long long>* KeyValues::getInt8V(const string& key) {
    return select->resultSet->getInt8V(key);
}
vector<long long>* KeyValues::getInt8V(const int col) {
    return select->resultSet->getInt8V(col);
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
double* KeyValues::getFloat8A(const string& key, int& size) {
    return select->resultSet->getFloat8A(key, size);
}

double* KeyValues::getFloat8A(const int col, int& size) {
    return select->resultSet->getFloat8A(col, size);
}

vector<double>* KeyValues::getFloat8V(const string& key) {
    return select->resultSet->getFloat8V(key);
}

vector<double>* KeyValues::getFloat8V(const int col) {
    return select->resultSet->getFloat8V(col);
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
PGpoint *KeyValues::getPointA(const std::string& key, int& size) {
    return select->resultSet->getPointA(key, size);
}
PGpoint *KeyValues::getPointA(const int col, int& size) {
    return select->resultSet->getPointA(col, size);
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

IntervalEvent *KeyValues::getIntervalEvent(const std::string& key)
{
    return select->resultSet->getIntervalEvent(key);
}
IntervalEvent *KeyValues::getIntervalEvent(const int col)
{
    return select->resultSet->getIntervalEvent(col);
}
ProcessState *KeyValues::getProcessState(const std::string& key)
{
    return select->resultSet->getProcessState(key);
}

ProcessState *KeyValues::getProcessState(const int col)
{
    return select->resultSet->getProcessState(col);
}
    
// =============== GETTERS - OTHER =============================================



void *KeyValues::getBlob(const std::string& key, int &size) {
    return select->resultSet->getBlob(key, size);
}
void *KeyValues::getBlob(const int col, int &size) {
    return select->resultSet->getBlob(col, size);
}

// =============== PRINT methods =======================================

bool KeyValues::print() {
    if (!select || !select->resultSet->isOk() || select->resultSet->getPosition() < 0) {
        logger->warning(302, "There is nothing to print (see other messages)", thisClass+"::print()");
        return false;
    }
    else {
        int origpos = select->resultSet->getPosition();
        int get_widths = (format == STANDARD);
        pair< TKeys*, vector<int>* > fInfo = select->resultSet->getKeysWidths(select->resultSet->getPosition(), get_widths, arrayLimit);
        if (fInfo.first) {
            printHeader(fInfo);
            printRowOnly(select->resultSet->getPosition(), fInfo.second);
            printFooter(1);
        }
        else {
            cout << "(empty)" << endl;
        }
        vt_destruct(fInfo.first);
        vt_destruct(fInfo.second);
        
        select->resultSet->setPosition(origpos);
        return true;
    }
}

bool KeyValues::printAll() {
    bool retval = true;
    int origpos = select->resultSet->getPosition();
    pair< TKeys*, vector<int>* > fInfo(NULL, NULL);

    do {
        if (!select || !select->resultSet->isOk()) {
            logger->warning(303, "There is nothing to print (see other messages)", thisClass + "::printAll()");
            retval = false;
            break;
        } 

        bool getWidths = (format == STANDARD);
        fInfo = select->resultSet->getKeysWidths(-1, getWidths, arrayLimit);
        if (!fInfo.first || (getWidths && !fInfo.second)) {
            cout << "(empty)" << endl;
            break;
        }
        
        retval = printHeader(fInfo);
        if (!retval) break;
        
        for (int r = 0; r < select->resultSet->countRows(); r++) {
            printRowOnly(r, fInfo.second);
        }
        
        printFooter(select->resultSet->countRows());
        
    } while(0);

    vt_destruct(fInfo.first);
    vt_destruct(fInfo.second);
    select->resultSet->setPosition(origpos);
    
    return retval;
}

// =============== PRINT support methods =======================================

bool KeyValues::printHeader(const pair< TKeys*, vector<int>* > &fInfo) {
    stringstream table, nameln, typeln, border;
    int cols = select->resultSet->countCols();

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
            if (c < cols - 1) {
                nameln << '|';
                typeln << '|';
                border << '+';
            }
        } else if (format == CSV) {
            nameln << (*fInfo.first)[c].key;
            if (c < cols - 1) nameln << ',';
        } else if (format == HTML) {
            table << "<th>" << (*fInfo.first)[c].key << "<br/>";
            table << (*fInfo.first)[c].type << "</th>";
        }
    }
    table << "</tr>" << endl;
    nameln << endl;
    typeln << endl;
    border << endl;
    if (format == STANDARD) {
        cout << nameln.str() << typeln.str() << border.str();
    }
    else if (format == CSV) {
        cout << nameln.str();
    }
    else if (format == HTML) {
        cout << table.str();
    }

    return true;
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

    return true;
}

bool KeyValues::printRowOnly(const int row, const vector<int>* widths) {
    bool retval = true;
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

bool KeyValues::preSet() {
    logger->warning(3010, "Set inherited from KeyValues at class " + thisClass, thisClass+"::preSet()");

    vt_destruct(update);
    update = new Update(*this);

    return update ? true : false;
}

bool KeyValues::setString(const string& key, const string& value) {
    // call preset on the derived class
    if (!update) this->preSet();

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

bool KeyValues::setTimestamp(const std::string& key, const time_t& value)
{
    if (!update) this->preSet();
    return update->setTimestamp(key, value);
}

bool KeyValues::setPStatus(const std::string& key, ProcessState::STATUS_T value)
{
    if (!update) this->preSet();
    return update->setPStatus(key, value);
}

bool KeyValues::setExecute()
{
    bool bRet = false;
    
    if (update) {
        bRet = update->execute();
        vt_destruct(update);
    }
    
    return bRet;
}


// =================== ADDERS (Insert) =========================================
bool KeyValues::addString(const std::string& key, const std::string& value) {
    return this->insert ? this->insert->keyString(key, value) : false;
}
bool KeyValues::addInt(const std::string& key, int value) {
    return this->insert ? this->insert->keyInt(key, value) : false;
}
bool KeyValues::addIntA(const std::string& key, int* value, int size) {
    return this->insert ? this->insert->keyIntA(key, value, size) : false;
}
bool KeyValues::addFloat(const std::string& key, float value) {
    return this->insert ? this->insert->keyFloat(key, value) : false;
}
bool KeyValues::addFloatA(const std::string& key, float* value, int size) {
    return this->insert ? this->insert->keyFloatA(key, value, size) : false;
}
bool KeyValues::addTimestamp(const std::string& key, const time_t& value)
{
    return this->insert ? this->insert->keyTimestamp(key, value) : false;
}
#if HAVE_OPENCV
bool KeyValues::addCvMat(const std::string& key, cv::Mat& value) {
    return this->insert ? this->insert->keyCvMat(key, value) : false;
}
#endif
    
bool KeyValues::addIntervalEvent(const std::string& key, IntervalEvent& value) {
    return this->insert ? this->insert->keyIntervalEvent(key, value) : false;
}

bool KeyValues::addExecute()
{
    bool retval = true;
    bool trans = !store.empty() && insert;
    
    if (trans) {
        trans = insert->beginTransaction();
    }
    
    for (list<Insert *>::iterator it = store.begin(); it != store.end(); it++) {
        retval &= (*it)->execute();
        delete (*it);
    }
    store.clear();

    if (insert) {
        if (retval) {
            retval &= insert->execute();
            if (trans) insert->commitTransaction();
        }
        else {
            if (trans) insert->rollbackTransaction();
        }
        vt_destruct(insert);
    }
    else {
        retval = false;
    }

    return retval;
}

// =================== CHECKERS (Storage) ======================================
bool KeyValues::checkStorage() {
    logger->warning(3018, "Check might fail at class " + thisClass, thisClass+"::checkStorage()");

    if (!sequence.empty() && fileExists(getDataLocation())) return true;
    else if (!dataset.empty() && fileExists(getDataLocation())) return true;

    return false;
}


