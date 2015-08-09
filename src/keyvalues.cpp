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
#include <vtapi/common/global.h>
#include <vtapi/data/keyvalues.h>

using namespace std;

namespace vtapi {


KeyValues::KeyValues(const Commons& commons, const string& selection)
    : Commons(commons, false), _select(commons), _update(NULL)
{
    if (!selection.empty())
        context().selection = selection;
}

KeyValues::~KeyValues()
{
    if (_update) {
        if (!_update->isExecuted()) updateExecute();
        vt_destruct(_update);
    }
}

bool KeyValues::next()
{
    // UPDATE data is handled automatically here
    if (_update && !updateExecute())
        return false;
    
    // SELECT data if result set is not valid
    if (_select.resultset().getPosition() == -1) {
        if (!_select.execute() || _select.resultset().countRows() == 0) {
            _select.resultset().setPosition(-1);
            return false;
        }
        else {
            _select.resultset().setPosition(0);
            return true;
        }
    }
    // use previous resultset
    else {
        // check SELECT limit, should new resultset be fetched?
        int limit = _select.getLimit();
        if (limit > 0 && _select.resultset().getPosition() + 1 >= limit) {
            if (!_select.executeNext() || _select.resultset().countRows() == 0) {
                _select.resultset().setPosition(-1);
                return false;
            }
            else {
                _select.resultset().setPosition(0);
                return true;
            }
        }
        // step through resultset
        else {
            if (_select.resultset().countRows() > _select.resultset().getPosition() + 1) {
                _select.resultset().incPosition();
                return true;
            }
            else {
                _select.resultset().setPosition(-1);
                return false;
            }
        }
    }
}

int KeyValues::count()
{
    int cnt = -1;

    std::string query   = _select.querybuilder().getCountQuery();
    void *param         = _select.querybuilder().getQueryParam();
    void *paramDup      = _select.querybuilder().duplicateQueryParam(param);
    ResultSet *res      = backend().createResultSet(connection().getDBTypes());

    if (connection().fetch(query, paramDup, *res) > 0) {
        res->setPosition(0);
        cnt = res->getInt8(0);
    }

    if (paramDup) _select.querybuilder().destroyQueryParam(paramDup);
    vt_destruct(res);

    return cnt;
}

TKey KeyValues::getKey(int col)
{
    return _select.resultset().getKey(col);
}

TKeys* KeyValues::getKeys()
{
    return _select.resultset().getKeys();
}


// =============== GETTERS (Select) ============================================

/**
 * Generic getter - fetches any value from resultset and returns it as string
 * @param col column index
 * @param arrayLimit limits size of elements of returned array
 * @return string representation of field value
 */
string KeyValues::getValue(const int col)
{
    return _select.resultset().getValue(col);
}

// =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===================

char KeyValues::getChar(const string& key)
{
    return _select.resultset().getChar(key);
}

char KeyValues::getChar(const int col)
{
    return _select.resultset().getChar(col);
}

string KeyValues::getString(const string& key)
{
    return _select.resultset().getString(key);
}

string KeyValues::getString(const int col)
{
    return _select.resultset().getString(col);
}

// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================

bool KeyValues::getBool(const string& key)
{
    return _select.resultset().getBool(key);
}

bool KeyValues::getBool(const int col)
{
    return _select.resultset().getBool(col);
}

int KeyValues::getInt(const string& key)
{
    return _select.resultset().getInt(key);
}

int KeyValues::getInt(const int col)
{
    return _select.resultset().getInt(col);
}

long long KeyValues::getInt8(const string& key)
{
    return _select.resultset().getInt8(key);
}

long long KeyValues::getInt8(const int col)
{
    return _select.resultset().getInt8(col);
}

int* KeyValues::getIntA(const string& key, int& size)
{
    return _select.resultset().getIntA(key, size);
}

int* KeyValues::getIntA(const int col, int& size)
{
    return _select.resultset().getIntA(col, size);
}

vector<int>* KeyValues::getIntV(const string& key)
{
    return _select.resultset().getIntV(key);
}

vector<int>* KeyValues::getIntV(const int col)
{
    return _select.resultset().getIntV(col);
}

long long* KeyValues::getInt8A(const string& key, int& size)
{
    return _select.resultset().getInt8A(key, size);
}

long long* KeyValues::getInt8A(const int col, int& size)
{
    return _select.resultset().getInt8A(col, size);
}

vector<long long>* KeyValues::getInt8V(const string& key)
{
    return _select.resultset().getInt8V(key);
}

vector<long long>* KeyValues::getInt8V(const int col)
{
    return _select.resultset().getInt8V(col);
}

// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================

float KeyValues::getFloat(const string& key)
{
    return _select.resultset().getFloat(key);
}

float KeyValues::getFloat(const int col)
{
    return _select.resultset().getFloat(col);
}

double KeyValues::getFloat8(const string& key)
{
    return _select.resultset().getFloat8(key);
}

double KeyValues::getFloat8(const int col)
{
    return _select.resultset().getFloat8(col);
}

float* KeyValues::getFloatA(const string& key, int& size)
{
    return _select.resultset().getFloatA(key, size);
}

float* KeyValues::getFloatA(const int col, int& size)
{
    return _select.resultset().getFloatA(col, size);
}

vector<float>* KeyValues::getFloatV(const string& key)
{
    return _select.resultset().getFloatV(key);
}

vector<float>* KeyValues::getFloatV(const int col)
{
    return _select.resultset().getFloatV(col);
}

double* KeyValues::getFloat8A(const string& key, int& size)
{
    return _select.resultset().getFloat8A(key, size);
}

double* KeyValues::getFloat8A(const int col, int& size)
{
    return _select.resultset().getFloat8A(col, size);
}

vector<double>* KeyValues::getFloat8V(const string& key)
{
    return _select.resultset().getFloat8V(key);
}

vector<double>* KeyValues::getFloat8V(const int col)
{
    return _select.resultset().getFloat8V(col);
}

// =============== GETTERS - OpenCV MATRICES ===============================

cv::Mat *KeyValues::getCvMat(const string& key)
{
    return _select.resultset().getCvMat(key);
}

cv::Mat *KeyValues::getCvMat(const int col)
{
    return _select.resultset().getCvMat(col);
}

// =============== GETTERS - TIMESTAMP =========================================

time_t KeyValues::getTimestamp(const string& key)
{
    return _select.resultset().getTimestamp(key);
}

time_t KeyValues::getTimestamp(const int col)
{
    return _select.resultset().getTimestamp(col);
}

// =============== GETTERS - GEOMETRIC TYPES ===============================

Point KeyValues::getPoint(const string& key)
{
    return _select.resultset().getPoint(key);
}

Point KeyValues::getPoint(const int col)
{
    return _select.resultset().getPoint(col);
}

Point *KeyValues::getPointA(const std::string& key, int& size)
{
    return _select.resultset().getPointA(key, size);
}

Point *KeyValues::getPointA(const int col, int& size)
{
    return _select.resultset().getPointA(col, size);
}

vector<Point>*  KeyValues::getPointV(const string& key)
{
    return _select.resultset().getPointV(key);
}

vector<Point>*  KeyValues::getPointV(const int col)
{
    return _select.resultset().getPointV(col);
}

IntervalEvent *KeyValues::getIntervalEvent(const std::string& key)
{
    return _select.resultset().getIntervalEvent(key);
}

IntervalEvent *KeyValues::getIntervalEvent(const int col)
{
    return _select.resultset().getIntervalEvent(col);
}

ProcessState *KeyValues::getProcessState(const std::string& key)
{
    return _select.resultset().getProcessState(key);
}

ProcessState *KeyValues::getProcessState(const int col)
{
    return _select.resultset().getProcessState(col);
}

// =============== GETTERS - OTHER =============================================

void *KeyValues::getBlob(const std::string& key, int &size)
{
    return _select.resultset().getBlob(key, size);
}

void *KeyValues::getBlob(const int col, int &size)
{
    return _select.resultset().getBlob(col, size);
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
//
//bool KeyValues::addCvMat(const std::string& key, cv::Mat& value)
//{
//    return this->insert ? this->insert->keyCvMat(key, value) : false;
//}
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
    int cols = _select.resultset().countCols();

    for (int i = 0; i < cols; i++) {
        if (!line.empty()) line += ',';
        line += '\"';
        line += getValue(i);
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
    if (ret && _select.resultset().getPosition() >= 0) {
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
    int cols = _select.resultset().countCols();

    for (int i = 0; i < cols; i++) {
        if (!header.empty()) header += ',';
        header += _select.resultset().getKey(i).m_key;
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
