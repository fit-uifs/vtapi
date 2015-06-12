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
    for (auto item : store) {
        if (!item->executed) {
            logger->warning(313, "INSERT operation was not executed:\n" + item->getQuery(), thisClass + "~KeyValues()");
        }
        vt_destruct(item);
    }

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
    
//    if (insert && !addExecute())
//        return NULL;
    
    if (update && !updateExecute())
        return NULL;

    // select is executed here when position == -1
    if (select->resultSet->getPosition() == -1) {                
        if (!select->execute())
            return NULL;
    }

    // check if end of resultset has been reached
    int rowCount = select->resultSet->countRows();
    
    // no rows
    if (rowCount <= 0) {
        return NULL;
    }
    else {
        // new resultset should be fetched
        int limit = select->getLimit();
        if (limit > 0 && select->resultSet->getPosition() + 1 >= limit) {
            if (!select->executeNext()) {
                select->resultSet->setPosition(-1);
                return NULL;
            }
            else {
                select->resultSet->setPosition(-1);
                rowCount = select->resultSet->countRows();
            }
        }
        
        // there is another row after this one
        if (rowCount > select->resultSet->getPosition() + 1) {
            select->resultSet->step();
            return this;
        }
        // no more rows
        else {
            select->resultSet->setPosition(-1);
            return NULL;
        }
    } 
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

char KeyValues::getChar(const string& key)
{
    return select->resultSet->getChar(key);
}
char KeyValues::getChar(const int col)
{
    return select->resultSet->getChar(col);
}
string KeyValues::getString(const string& key)
{
    return select->resultSet->getString(key);
}
string KeyValues::getString(const int col)
{
    return select->resultSet->getString(col);
}

// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================

bool KeyValues::getBool(const string& key)
{
    return select->resultSet->getBool(key);
}
bool KeyValues::getBool(const int col)
{
    return select->resultSet->getBool(col);
}
int KeyValues::getInt(const string& key)
{
    return select->resultSet->getInt(key);
}
int KeyValues::getInt(const int col)
{
    return select->resultSet->getInt(col);
}
long long KeyValues::getInt8(const string& key)
{
    return select->resultSet->getInt8(key);
}
long long KeyValues::getInt8(const int col)
{
    return select->resultSet->getInt8(col);
}
int* KeyValues::getIntA(const string& key, int& size)
{
    return select->resultSet->getIntA(key, size);
}
int* KeyValues::getIntA(const int col, int& size)
{
    return select->resultSet->getIntA(col, size);
}
vector<int>* KeyValues::getIntV(const string& key)
{
    return select->resultSet->getIntV(key);
}
vector<int>* KeyValues::getIntV(const int col)
{
    return select->resultSet->getIntV(col);
}
long long* KeyValues::getInt8A(const string& key, int& size)
{
    return select->resultSet->getInt8A(key, size);
}
long long* KeyValues::getInt8A(const int col, int& size)
{
    return select->resultSet->getInt8A(col, size);
}
vector<long long>* KeyValues::getInt8V(const string& key)
{
    return select->resultSet->getInt8V(key);
}
vector<long long>* KeyValues::getInt8V(const int col)
{
    return select->resultSet->getInt8V(col);
}

// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================
float KeyValues::getFloat(const string& key)
{
    return select->resultSet->getFloat(key);
}
float KeyValues::getFloat(const int col)
{
    return select->resultSet->getFloat(col);
}
double KeyValues::getFloat8(const string& key)
{
    return select->resultSet->getFloat8(key);
}
double KeyValues::getFloat8(const int col)
{
    return select->resultSet->getFloat8(col);
}
float* KeyValues::getFloatA(const string& key, int& size)
{
    return select->resultSet->getFloatA(key, size);
}
float* KeyValues::getFloatA(const int col, int& size)
{
    return select->resultSet->getFloatA(col, size);
}
vector<float>* KeyValues::getFloatV(const string& key)
{
    return select->resultSet->getFloatV(key);
}
vector<float>* KeyValues::getFloatV(const int col)
{
    return select->resultSet->getFloatV(col);
}
double* KeyValues::getFloat8A(const string& key, int& size)
{
    return select->resultSet->getFloat8A(key, size);
}

double* KeyValues::getFloat8A(const int col, int& size)
{
    return select->resultSet->getFloat8A(col, size);
}

vector<double>* KeyValues::getFloat8V(const string& key)
{
    return select->resultSet->getFloat8V(key);
}

vector<double>* KeyValues::getFloat8V(const int col)
{
    return select->resultSet->getFloat8V(col);
}

// =============== GETTERS - OpenCV MATRICES ===============================
#if HAVE_OPENCV

cv::Mat *KeyValues::getCvMat(const string& key)
{
    return select->resultSet->getCvMat(key);
}

cv::Mat *KeyValues::getCvMat(const int col)
{
    return select->resultSet->getCvMat(col);
}
#endif
// =============== GETTERS - TIMESTAMP =========================================
time_t KeyValues::getTimestamp(const string& key)
{
    return select->resultSet->getTimestamp(key);
}
time_t KeyValues::getTimestamp(const int col)
{
    return select->resultSet->getTimestamp(col);
}

// =============== GETTERS - GEOMETRIC TYPES ===============================
#if HAVE_POSTGRESQL
PGpoint KeyValues::getPoint(const string& key)
{
    return select->resultSet->getPoint(key);
}
PGpoint KeyValues::getPoint(const int col)
{
    return select->resultSet->getPoint(col);
}
PGpoint *KeyValues::getPointA(const std::string& key, int& size)
{
    return select->resultSet->getPointA(key, size);
}
PGpoint *KeyValues::getPointA(const int col, int& size)
{
    return select->resultSet->getPointA(col, size);
}
vector<PGpoint>*  KeyValues::getPointV(const string& key)
{
    return select->resultSet->getPointV(key);
}
vector<PGpoint>*  KeyValues::getPointV(const int col)
{
    return select->resultSet->getPointV(col);
}

#endif


#if HAVE_POSTGIS
GEOSGeometry *KeyValues::getGeometry(const string& key)
{
    return select->resultSet->getGeometry(key);
}
GEOSGeometry *KeyValues::getGeometry(const int col)
{
    return select->resultSet->getGeometry(col);
}

GEOSGeometry *KeyValues::getLineString(const string& key)
{
    return select->resultSet->getLineString(key);
}
GEOSGeometry *KeyValues::getLineString(const int col)
{
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

void *KeyValues::getBlob(const std::string& key, int &size)
{
    return select->resultSet->getBlob(key, size);
}
void *KeyValues::getBlob(const int col, int &size)
{
    return select->resultSet->getBlob(col, size);
}


// =============== SETTERS (Update) ============================================

bool KeyValues::preUpdate()
{
    logger->warning(728, "Update performed using generic KeyValues", thisClass + "::preUpdate()");
    return false;
}
bool KeyValues::preUpdate(const std::string& table)
{
    vt_destruct(update);
    update = new Update(*this, table);

    return update ? true : false;
}
bool KeyValues::updateString(const string& key, const string& value)
{
    if (!update) this->preUpdate();
    return update->setString(key, value);
}
bool KeyValues::updateInt(const string& key, int value)
{
    if (!update) this->preUpdate();
    return update->setInt(key, value);
}
bool KeyValues::updateIntA(const string& key, int* values, int size)
{
    if (!update) this->preUpdate();
    return update->setIntA(key, values, size);
}
bool KeyValues::updateFloat(const string& key, float value)
{
    if (!update) this->preUpdate();
    return update->setFloat(key, value);
}
bool KeyValues::updateFloatA(const string& key, float* values, int size)
{
    if (!update) this->preUpdate();
    return update->setFloatA(key, values, size);
}
bool KeyValues::updateTimestamp(const std::string& key, const time_t& value)
{
    if (!update) this->preUpdate();
    return update->setTimestamp(key, value);
}
bool KeyValues::updateProcessStatus(const std::string& key, ProcessState::STATUS_T value)
{
    if (!update) this->preUpdate();
    return update->updateProcessStatus(key, value);
}
bool KeyValues::updateExecute()
{
    if (update) {
        bool ret = update->execute();
        vt_destruct(update);
        return ret;
    }
    else {
        return false;
    }
}


// =================== ADDERS (Insert) =========================================

bool KeyValues::preAdd(const std::string& table)
{
    if (this->insert = new Insert(*this, table)) {
        this->store.push_back(insert);
        return true;
    }
    else {
        return false;
    }
}

bool KeyValues::addString(const std::string& key, const std::string& value)
{
    return this->insert ? this->insert->keyString(key, value) : false;
}
bool KeyValues::addInt(const std::string& key, int value)
{
    return this->insert ? this->insert->keyInt(key, value) : false;
}
bool KeyValues::addIntA(const std::string& key, int* value, int size)
{
    return this->insert ? this->insert->keyIntA(key, value, size) : false;
}
bool KeyValues::addFloat(const std::string& key, float value)
{
    return this->insert ? this->insert->keyFloat(key, value) : false;
}
bool KeyValues::addFloatA(const std::string& key, float* value, int size)
{
    return this->insert ? this->insert->keyFloatA(key, value, size) : false;
}
bool KeyValues::addTimestamp(const std::string& key, const time_t& value)
{
    return this->insert ? this->insert->keyTimestamp(key, value) : false;
}
#if HAVE_OPENCV
bool KeyValues::addCvMat(const std::string& key, cv::Mat& value)
{
    return this->insert ? this->insert->keyCvMat(key, value) : false;
}
#endif
bool KeyValues::addIntervalEvent(const std::string& key, const IntervalEvent& value)
{
    return this->insert ? this->insert->keyIntervalEvent(key, value) : false;
}
bool KeyValues::addExecute()
{
    bool ret = true;

    do {
        if (!insert) {
            ret = false;
            break;
        }
        
        ret = insert->beginTransaction();
        if (!ret) break;

        for (auto item : store) {
            ret = item->execute();
            if (!ret) break;
        }
        if (!ret) break;

        ret = insert->commitTransaction();
        if (!ret) break;
    } while(0);
    
    
    if (insert && !ret) {
        insert->rollbackTransaction();
    }

    for (auto item : store) {
        delete item;
    }
    store.clear();
    
    return ret;
}


// =============== PRINT methods =======================================

bool KeyValues::print()
{
    string line;
    int cols = select->resultSet->countCols();

    for (int i = 0; i < cols; i++) {
        if (!line.empty()) line += ',';
        line += '\"';
        line += getValue(i, this->arrayLimit);
        line += '\"';
    }

    if (!line.empty()) {
        cout << line << endl;
        return true;
    }
    else {
        return false;
    }
}

bool KeyValues::printAll()
{
    bool ret = printKeys();
    if (ret) {
        do {
            ret = print();
            if (!ret) break;
        } while (next());
    }
    
    return ret;
}

bool KeyValues::printKeys()
{
    string header;
    int cols = select->resultSet->countCols();

    for (int i = 0; i < cols; i++) {
        if (!header.empty()) header += ',';
        header += select->resultSet->getKey(i).key;
    }
    
    if (!header.empty()) {
        cout << header << endl;
        return true;
    }
    else {
        return false;
    }
}
