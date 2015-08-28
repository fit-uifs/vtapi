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
#include <vtapi/common/exception.h>
#include <vtapi/common/defs.h>
#include <vtapi/data/keyvalues.h>

using namespace std;

namespace vtapi {


KeyValues::KeyValues(const Commons& commons, const string& selection)
    : Commons(commons, false), _select(commons, selection), _update(NULL)
{
    if (selection.empty())
        throw RuntimeException("no table specified for KeyValues");

    context().selection = selection;

    _select.setLimit(DEF_SELECT_LIMIT);
}

KeyValues::~KeyValues()
{
    if (_update) {
        updateExecute();
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
        if (DEF_SELECT_LIMIT > 0 && _select.resultset().getPosition() + 1 >= DEF_SELECT_LIMIT) {
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

Select &KeyValues::select()
{
    return _select;
}

Update &KeyValues::update()
{
    if (!_update) {
        _update = new Update(*this, context().selection);
        if (!preUpdate())
            throw RuntimeException("preUpdate() failed");
    }

    return *_update;
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
    return select().resultset().getValue(col);
}

// =============== GETTERS FOR CHAR, CHAR ARRAYS AND STRINGS ===================

char KeyValues::getChar(const string& key)
{
    return select().resultset().getChar(key);
}

char KeyValues::getChar(const int col)
{
    return select().resultset().getChar(col);
}

string KeyValues::getString(const string& key)
{
    return select().resultset().getString(key);
}

string KeyValues::getString(const int col)
{
    return select().resultset().getString(col);
}

// =============== GETTERS FOR INTEGERS OR ARRAYS OF INTEGERS ==================

bool KeyValues::getBool(const string& key)
{
    return select().resultset().getBool(key);
}

bool KeyValues::getBool(const int col)
{
    return select().resultset().getBool(col);
}

int KeyValues::getInt(const string& key)
{
    return select().resultset().getInt(key);
}

int KeyValues::getInt(const int col)
{
    return select().resultset().getInt(col);
}

long long KeyValues::getInt8(const string& key)
{
    return select().resultset().getInt8(key);
}

long long KeyValues::getInt8(const int col)
{
    return select().resultset().getInt8(col);
}

int* KeyValues::getIntA(const string& key, int& size)
{
    return select().resultset().getIntA(key, size);
}

int* KeyValues::getIntA(const int col, int& size)
{
    return select().resultset().getIntA(col, size);
}

vector<int>* KeyValues::getIntV(const string& key)
{
    return select().resultset().getIntV(key);
}

vector<int>* KeyValues::getIntV(const int col)
{
    return select().resultset().getIntV(col);
}

long long* KeyValues::getInt8A(const string& key, int& size)
{
    return select().resultset().getInt8A(key, size);
}

long long* KeyValues::getInt8A(const int col, int& size)
{
    return select().resultset().getInt8A(col, size);
}

vector<long long>* KeyValues::getInt8V(const string& key)
{
    return select().resultset().getInt8V(key);
}

vector<long long>* KeyValues::getInt8V(const int col)
{
    return select().resultset().getInt8V(col);
}

// =============== GETTERS FOR FLOATS OR ARRAYS OF FLOATS ======================

float KeyValues::getFloat(const string& key)
{
    return select().resultset().getFloat(key);
}

float KeyValues::getFloat(const int col)
{
    return select().resultset().getFloat(col);
}

double KeyValues::getFloat8(const string& key)
{
    return select().resultset().getFloat8(key);
}

double KeyValues::getFloat8(const int col)
{
    return select().resultset().getFloat8(col);
}

float* KeyValues::getFloatA(const string& key, int& size)
{
    return select().resultset().getFloatA(key, size);
}

float* KeyValues::getFloatA(const int col, int& size)
{
    return select().resultset().getFloatA(col, size);
}

vector<float>* KeyValues::getFloatV(const string& key)
{
    return select().resultset().getFloatV(key);
}

vector<float>* KeyValues::getFloatV(const int col)
{
    return select().resultset().getFloatV(col);
}

double* KeyValues::getFloat8A(const string& key, int& size)
{
    return select().resultset().getFloat8A(key, size);
}

double* KeyValues::getFloat8A(const int col, int& size)
{
    return select().resultset().getFloat8A(col, size);
}

vector<double>* KeyValues::getFloat8V(const string& key)
{
    return select().resultset().getFloat8V(key);
}

vector<double>* KeyValues::getFloat8V(const int col)
{
    return select().resultset().getFloat8V(col);
}

// =============== GETTERS - OpenCV MATRICES ===============================

cv::Mat *KeyValues::getCvMat(const string& key)
{
    return select().resultset().getCvMat(key);
}

cv::Mat *KeyValues::getCvMat(const int col)
{
    return select().resultset().getCvMat(col);
}

// =============== GETTERS - TIMESTAMP =========================================

time_t KeyValues::getTimestamp(const string& key)
{
    return select().resultset().getTimestamp(key);
}

time_t KeyValues::getTimestamp(const int col)
{
    return select().resultset().getTimestamp(col);
}

// =============== GETTERS - GEOMETRIC TYPES ===============================

Point KeyValues::getPoint(const string& key)
{
    return select().resultset().getPoint(key);
}

Point KeyValues::getPoint(const int col)
{
    return select().resultset().getPoint(col);
}

Point *KeyValues::getPointA(const std::string& key, int& size)
{
    return select().resultset().getPointA(key, size);
}

Point *KeyValues::getPointA(const int col, int& size)
{
    return select().resultset().getPointA(col, size);
}

vector<Point>*  KeyValues::getPointV(const string& key)
{
    return select().resultset().getPointV(key);
}

vector<Point>*  KeyValues::getPointV(const int col)
{
    return select().resultset().getPointV(col);
}

IntervalEvent *KeyValues::getIntervalEvent(const std::string& key)
{
    return select().resultset().getIntervalEvent(key);
}

IntervalEvent *KeyValues::getIntervalEvent(const int col)
{
    return select().resultset().getIntervalEvent(col);
}

ProcessState *KeyValues::getProcessState(const std::string& key)
{
    return select().resultset().getProcessState(key);
}

ProcessState *KeyValues::getProcessState(const int col)
{
    return select().resultset().getProcessState(col);
}

// =============== GETTERS - OTHER =============================================

void *KeyValues::getBlob(const std::string& key, int &size)
{
    return select().resultset().getBlob(key, size);
}

void *KeyValues::getBlob(const int col, int &size)
{
    return select().resultset().getBlob(col, size);
}


// =============== SETTERS (Update) ============================================

bool KeyValues::preUpdate()
{
    VTLOG_WARNING("Update performed using generic KeyValues");
    return false;
}

bool KeyValues::updateString(const string& key, const string& value)
{
    return update().setString(key, value);
}

bool KeyValues::updateBool(const string &key, bool value)
{
    return update().setBool(key, value);
}

bool KeyValues::updateInt(const string& key, int value)
{
    return update().setInt(key, value);
}

bool KeyValues::updateIntA(const string& key, int* values, int size)
{
    return update().setIntA(key, values, size);
}

bool KeyValues::updateFloat(const string& key, float value)
{
    return update().setFloat(key, value);
}

bool KeyValues::updateFloatA(const string& key, float* values, int size)
{
    return update().setFloatA(key, values, size);
}

bool KeyValues::updateTimestamp(const std::string& key, const time_t& value)
{
    return update().setTimestamp(key, value);
}

bool KeyValues::updateProcessStatus(const std::string& key, ProcessState::STATUS_T value)
{
    return update().setProcessStatus(key, value);
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
