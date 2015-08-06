/**
 * @file
 * @brief   Methods of KeyValues class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <iostream>
#include <common/vtapi_global.h>
#include <data/vtapi_keyvalues.h>

using namespace std;

namespace vtapi {


//================================ KEYVALUES ===================================


KeyValues::KeyValues(const Commons& commons, const string& selection)
    : Commons(commons, false), _select(commons), _update(NULL)
{
    if (!selection.empty())
        _context.selection = selection;
}

KeyValues::~KeyValues()
{
    if (_update) {
        if (!_update->_executed) updateExecute();
        vt_destruct(_update);
    }
}

bool KeyValues::next()
{
    // UPDATE data is handled automatically here
    if (_update && !updateExecute())
        return false;
    
    // SELECT data if result set is not valid
    if (_select._resultSet->getPosition() == -1) {
        if (!_select.execute() || _select._resultSet->countRows() == 0) {
            _select._resultSet->setPosition(-1);
            return false;
        }
        else {
            _select._resultSet->setPosition(0);
            return true;
        }
    }
    // use previous resultset
    else {
        // check SELECT limit, should new resultset be fetched?
        int limit = _select.getLimit();
        if (limit > 0 && _select._resultSet->getPosition() + 1 >= limit) {
            if (!_select.executeNext() || _select._resultSet->countRows() == 0) {
                _select._resultSet->setPosition(-1);
                return false;
            }
            else {
                _select._resultSet->setPosition(0);
                return true;
            }
        }
        // step through resultset
        else {
            if (_select._resultSet->countRows() > _select._resultSet->getPosition() + 1) {
                _select._resultSet->incPosition();
                return true;
            }
            else {
                _select._resultSet->setPosition(-1);
                return false;
            }
        }
    }
}

int KeyValues::count()
{
    int cnt = -1;

    std::string query   = _select._queryBuilder->getCountQuery();
    void *param         = _select._queryBuilder->getQueryParam();
    void *paramDup      = _select._queryBuilder->duplicateQueryParam(param);
    ResultSet *res      = BackendFactory::createResultSet(_config->backend,
                                                          *_backendBase,
                                                          _connection->getDBTypes());
    if (_connection->fetch(query, paramDup, res) > 0) {
        res->setPosition(0);
        cnt = res->getInt8(0);
    }

    if (paramDup) _select._queryBuilder->destroyQueryParam(paramDup);
    vt_destruct(res);

    return cnt;
}

TKey KeyValues::getKey(int col)
{
    return _select._resultSet->getKey(col);
}

TKeys* KeyValues::getKeys()
{
    return _select._resultSet->getKeys();
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
    return _select._resultSet->getValue(col, arrayLimit);
}

// =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===================

char KeyValues::getChar(const string& key)
{
    return _select._resultSet->getChar(key);
}

char KeyValues::getChar(const int col)
{
    return _select._resultSet->getChar(col);
}

string KeyValues::getString(const string& key)
{
    return _select._resultSet->getString(key);
}

string KeyValues::getString(const int col)
{
    return _select._resultSet->getString(col);
}

// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================

bool KeyValues::getBool(const string& key)
{
    return _select._resultSet->getBool(key);
}

bool KeyValues::getBool(const int col)
{
    return _select._resultSet->getBool(col);
}

int KeyValues::getInt(const string& key)
{
    return _select._resultSet->getInt(key);
}

int KeyValues::getInt(const int col)
{
    return _select._resultSet->getInt(col);
}

long long KeyValues::getInt8(const string& key)
{
    return _select._resultSet->getInt8(key);
}

long long KeyValues::getInt8(const int col)
{
    return _select._resultSet->getInt8(col);
}

int* KeyValues::getIntA(const string& key, int& size)
{
    return _select._resultSet->getIntA(key, size);
}

int* KeyValues::getIntA(const int col, int& size)
{
    return _select._resultSet->getIntA(col, size);
}

vector<int>* KeyValues::getIntV(const string& key)
{
    return _select._resultSet->getIntV(key);
}

vector<int>* KeyValues::getIntV(const int col)
{
    return _select._resultSet->getIntV(col);
}

long long* KeyValues::getInt8A(const string& key, int& size)
{
    return _select._resultSet->getInt8A(key, size);
}

long long* KeyValues::getInt8A(const int col, int& size)
{
    return _select._resultSet->getInt8A(col, size);
}

vector<long long>* KeyValues::getInt8V(const string& key)
{
    return _select._resultSet->getInt8V(key);
}

vector<long long>* KeyValues::getInt8V(const int col)
{
    return _select._resultSet->getInt8V(col);
}

// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================

float KeyValues::getFloat(const string& key)
{
    return _select._resultSet->getFloat(key);
}

float KeyValues::getFloat(const int col)
{
    return _select._resultSet->getFloat(col);
}

double KeyValues::getFloat8(const string& key)
{
    return _select._resultSet->getFloat8(key);
}

double KeyValues::getFloat8(const int col)
{
    return _select._resultSet->getFloat8(col);
}

float* KeyValues::getFloatA(const string& key, int& size)
{
    return _select._resultSet->getFloatA(key, size);
}

float* KeyValues::getFloatA(const int col, int& size)
{
    return _select._resultSet->getFloatA(col, size);
}

vector<float>* KeyValues::getFloatV(const string& key)
{
    return _select._resultSet->getFloatV(key);
}

vector<float>* KeyValues::getFloatV(const int col)
{
    return _select._resultSet->getFloatV(col);
}

double* KeyValues::getFloat8A(const string& key, int& size)
{
    return _select._resultSet->getFloat8A(key, size);
}

double* KeyValues::getFloat8A(const int col, int& size)
{
    return _select._resultSet->getFloat8A(col, size);
}

vector<double>* KeyValues::getFloat8V(const string& key)
{
    return _select._resultSet->getFloat8V(key);
}

vector<double>* KeyValues::getFloat8V(const int col)
{
    return _select._resultSet->getFloat8V(col);
}

// =============== GETTERS - OpenCV MATRICES ===============================
#if VTAPI_HAVE_OPENCV

cv::Mat *KeyValues::getCvMat(const string& key)
{
    return _select._resultSet->getCvMat(key);
}

cv::Mat *KeyValues::getCvMat(const int col)
{
    return _select._resultSet->getCvMat(col);
}
#endif
// =============== GETTERS - TIMESTAMP =========================================

time_t KeyValues::getTimestamp(const string& key)
{
    return _select._resultSet->getTimestamp(key);
}

time_t KeyValues::getTimestamp(const int col)
{
    return _select._resultSet->getTimestamp(col);
}

// =============== GETTERS - GEOMETRIC TYPES ===============================
#if VTAPI_HAVE_POSTGRESQL

PGpoint KeyValues::getPoint(const string& key)
{
    return _select._resultSet->getPoint(key);
}

PGpoint KeyValues::getPoint(const int col)
{
    return _select._resultSet->getPoint(col);
}

PGpoint *KeyValues::getPointA(const std::string& key, int& size)
{
    return _select._resultSet->getPointA(key, size);
}

PGpoint *KeyValues::getPointA(const int col, int& size)
{
    return _select._resultSet->getPointA(col, size);
}

vector<PGpoint>*  KeyValues::getPointV(const string& key)
{
    return _select._resultSet->getPointV(key);
}

vector<PGpoint>*  KeyValues::getPointV(const int col)
{
    return _select._resultSet->getPointV(col);
}

#endif


#if VTAPI_HAVE_POSTGIS

GEOSGeometry *KeyValues::getGeometry(const string& key)
{
    return _select._resultSet->getGeometry(key);
}

GEOSGeometry *KeyValues::getGeometry(const int col)
{
    return _select._resultSet->getGeometry(col);
}

GEOSGeometry *KeyValues::getLineString(const string& key)
{
    return _select._resultSet->getLineString(key);
}

GEOSGeometry *KeyValues::getLineString(const int col)
{
    return _select._resultSet->getLineString(col);
}
#endif

IntervalEvent *KeyValues::getIntervalEvent(const std::string& key)
{
    return _select._resultSet->getIntervalEvent(key);
}

IntervalEvent *KeyValues::getIntervalEvent(const int col)
{
    return _select._resultSet->getIntervalEvent(col);
}

ProcessState *KeyValues::getProcessState(const std::string& key)
{
    return _select._resultSet->getProcessState(key);
}

ProcessState *KeyValues::getProcessState(const int col)
{
    return _select._resultSet->getProcessState(col);
}

// =============== GETTERS - OTHER =============================================

void *KeyValues::getBlob(const std::string& key, int &size)
{
    return _select._resultSet->getBlob(key, size);
}

void *KeyValues::getBlob(const int col, int &size)
{
    return _select._resultSet->getBlob(col, size);
}


// =============== SETTERS (Update) ============================================

bool KeyValues::preUpdate()
{
    VTLOG_WARNING("Update performed using generic KeyValues");
    return false;
}

bool KeyValues::preUpdate(const std::string& table)
{
    if (!_update) {
        vt_destruct(_update);
        _update = new Update(*this, table);
    }

    return _update ? true : false;
}

bool KeyValues::updateString(const string& key, const string& value)
{
    this->preUpdate();
    return _update->setString(key, value);
}

bool KeyValues::updateInt(const string& key, int value)
{
    this->preUpdate();
    return _update->setInt(key, value);
}

bool KeyValues::updateIntA(const string& key, int* values, int size)
{
    this->preUpdate();
    return _update->setIntA(key, values, size);
}

bool KeyValues::updateFloat(const string& key, float value)
{
    this->preUpdate();
    return _update->setFloat(key, value);
}

bool KeyValues::updateFloatA(const string& key, float* values, int size)
{
    this->preUpdate();
    return _update->setFloatA(key, values, size);
}

bool KeyValues::updateTimestamp(const std::string& key, const time_t& value)
{
    this->preUpdate();
    return _update->setTimestamp(key, value);
}

bool KeyValues::updateProcessStatus(const std::string& key, ProcessState::STATUS_T value)
{
    this->preUpdate();
    return _update->updateProcessStatus(key, value);
}

bool KeyValues::updateExecute()
{
    if (_update) {
        bool ret = _update->execute();
        vt_destruct(_update);
        return ret;
    }
    else {
        return false;
    }
}


// =================== ADDERS (Insert) =========================================

//bool KeyValues::addString(const std::string& key, const std::string& value)
//{
//    return this->insert ? this->insert->keyString(key, value) : false;
//}
//
//bool KeyValues::addInt(const std::string& key, int value)
//{
//    return this->insert ? this->insert->keyInt(key, value) : false;
//}
//
//bool KeyValues::addIntA(const std::string& key, int* value, int size)
//{
//    return this->insert ? this->insert->keyIntA(key, value, size) : false;
//}
//
//bool KeyValues::addFloat(const std::string& key, float value)
//{
//    return this->insert ? this->insert->keyFloat(key, value) : false;
//}
//
//bool KeyValues::addFloatA(const std::string& key, float* value, int size)
//{
//    return this->insert ? this->insert->keyFloatA(key, value, size) : false;
//}
//
//bool KeyValues::addTimestamp(const std::string& key, const time_t& value)
//{
//    return this->insert ? this->insert->keyTimestamp(key, value) : false;
//}
//#if VTAPI_HAVE_OPENCV
//
//bool KeyValues::addCvMat(const std::string& key, cv::Mat& value)
//{
//    return this->insert ? this->insert->keyCvMat(key, value) : false;
//}
//#endif
//
//bool KeyValues::addIntervalEvent(const std::string& key, const IntervalEvent& value)
//{
//    return this->insert ? this->insert->keyIntervalEvent(key, value) : false;
//}
//
//bool KeyValues::addExecute()
//{
//    bool ret = true;
//
//    do {
//        if (!insert) {
//            ret = false;
//            break;
//        }
//
//        ret = insert->beginTransaction();
//        if (!ret) break;
//
//        for (auto item : store) {
//            ret = item->execute();
//            if (!ret) break;
//        }
//        if (!ret) break;
//
//        ret = insert->commitTransaction();
//        if (!ret) break;
//    } while (0);
//
//
//    if (insert && !ret) {
//        insert->rollbackTransaction();
//    }
//
//    for (auto item : store) {
//        delete item;
//    }
//    store.clear();
//    insert = NULL;
//
//    return ret;
//}


// =============== PRINT methods =======================================

bool KeyValues::print()
{
    string line;
    int cols = _select._resultSet->countCols();

    for (int i = 0; i < cols; i++) {
        if (!line.empty()) line += ',';
        line += '\"';
        line += getValue(i, _config->arrayLimit);
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
    if (ret && _select._resultSet->getPosition() >= 0) {
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
    int cols = _select._resultSet->countCols();

    for (int i = 0; i < cols; i++) {
        if (!header.empty()) header += ',';
        header += _select._resultSet->getKey(i).m_key;
    }

    if (!header.empty()) {
        cout << header << endl;
        return true;
    }
    else {
        return false;
    }
}

}
