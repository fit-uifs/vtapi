/**
 * @file
 * @brief   Methods of Query, Select, Insert and Update classes
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <common/vtapi_global.h>
#include <backends/vtapi_backendfactory.h>
#include <queries/vtapi_query.h>
#include <queries/vtapi_select.h>
#include <queries/vtapi_insert.h>
#include <queries/vtapi_update.h>
#include <queries/vtapi_predefined_queries.h>

using namespace std;

namespace vtapi {


//================================== QUERY =====================================

Query::Query(const Commons& commons, const string& sql)
    : Commons(commons, false)
{
    bool bIsQuery = (sql.find_first_of(" \t\n") != string::npos);
    if (bIsQuery) {
        // init string is query => set it to query builder, default table is selection
        _queryBuilder = BackendFactory::createQueryBuilder(_config->backend,
                                                           *_backendBase,
                                                           _connection->getConnectionObject(),
                                                           sql);
        if (!_context.selection.empty())
            _queryBuilder->useDefaultTable(_context.selection);
    }
    else {
        // init string isn't query => it is default table
        _queryBuilder = BackendFactory::createQueryBuilder(_config->backend,
                                                           *_backendBase,
                                                           _connection->getConnectionObject(),
                                                           std::string());
        if (!sql.empty())
            _queryBuilder->useDefaultTable(sql);
    }
    _queryBuilder->useDefaultSchema(_context.dataset);

    _resultSet = BackendFactory::createResultSet(_config->backend,
                                                 *_backendBase,
                                                 _connection->getDBTypes());

    _executed = false;
}

Query::~Query()
{
    vt_destruct(_resultSet);
    vt_destruct(_queryBuilder);
}

string Query::getQuery()
{
    return _queryBuilder->getGenericQuery();
}

bool Query::execute()
{
    _executed = true;
    return _connection->execute(this->getQuery(), _queryBuilder->getQueryParam());
}

void Query::reset()
{
    _executed = false;
    _queryBuilder->reset();
}

bool Query::checkQueryObject()
{
    return (_queryBuilder && _resultSet);
}

//================================== SELECT ====================================

Select::Select(const Commons& commons, const string& initString)
    : Query(commons, initString)
{
    _limit = _config->queryLimit;
    _offset = 0;
}

string Select::getQuery()
{
    return _queryBuilder->getSelectQuery(_groupby, _orderby, _limit, _offset);
}

bool Select::execute()
{
    _executed = true;
    return _connection->fetch(this->getQuery(), _queryBuilder->getQueryParam(), _resultSet) >= 0;
}

bool Select::executeNext()
{
    if (_limit > 0) {
        _offset += _limit;
        return this->execute();
    }
    else {
        return false;
    }
}

bool Select::from(const string& table, const string& column)
{
    bool retval = true;

    retval &= _queryBuilder->keyFrom(table, column);
    _executed = false;

    return retval;
}

void Select::orderBy(const string& key)
{
    _orderby = key;
}

bool Select::whereString(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereString(key, value, oper, from);
}

bool Select::whereInt(const string& key, const int value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereInt(key, value, oper, from);
}

bool Select::whereFloat(const string& key, const float value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereFloat(key, value, oper, from);
}

bool Select::whereSeqtype(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereSeqtype(key, value, oper, from);
}

bool Select::whereInouttype(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereInouttype(key, value, oper, from);
}

bool Select::wherePStatus(const string& key, ProcessState::STATUS_T value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->wherePStatus(key, value, oper, from);
}

bool Select::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereTimestamp(key, value, oper, from);
}

bool Select::whereTimeRange(const string& key_start, const string& key_length, const time_t& value_start, const uint value_length, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereTimeRange(key_start, key_length, value_start, value_length, oper, from);
}

bool Select::whereRegion(const string& key, const IntervalEvent::box& value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereRegion(key, value, oper, from);
}

bool Select::whereExpression(const string& expression, const string& value, const string& oper)
{
    _executed = false;
    return _queryBuilder->whereExpression(expression, value, oper);
}

bool Select::whereStringInList(const string& key, const list<string>& values)
{
    _executed = false;

    //TODO: whereStringInList

    //return _queryBuilder->whereExpression(expression, value, oper);
    return true;
}

bool Select::whereIntInList(const string& key, const list<int>& values)
{
    _executed = false;
    //TODO: whereIntInList

    return true;
}


//================================== INSERT ====================================

Insert::Insert(const Commons& commons, const string& initString)
    : Query(commons, initString) { }

string Insert::getQuery()
{
    return _queryBuilder->getInsertQuery();
}

bool Insert::keyString(const string& key, const string& value, const string& from)
{
    _executed = false;
    return _queryBuilder->keyString(key, value, from);
}

bool Insert::keyStringA(const string& key, string* values, const int size, const string& from)
{
    _executed = false;
    return _queryBuilder->keyStringA(key, values, size, from);
}

bool Insert::keyInt(const string& key, int value, const string& from)
{
    _executed = false;
    return _queryBuilder->keyInt(key, value, from);
}

bool Insert::keyIntA(const string& key, int* values, const int size, const string& from)
{
    _executed = false;
    return _queryBuilder->keyIntA(key, values, size, from);
}

bool Insert::keyFloat(const string& key, float value, const string& from)
{
    _executed = false;
    return _queryBuilder->keyFloat(key, value, from);
}

bool Insert::keyFloatA(const string& key, float* values, const int size, const string& from)
{
    _executed = false;
    return _queryBuilder->keyFloatA(key, values, size, from);
}

bool Insert::keySeqtype(const string& key, const string& value, const string& from)
{
    _executed = false;
    return _queryBuilder->keySeqtype(key, value, from);
}

bool Insert::keyInouttype(const string& key, const string& value, const string& from)
{
    _executed = false;
    return _queryBuilder->keyInouttype(key, value, from);
}

bool Insert::keyTimestamp(const string& key, const time_t& value, const string& from)
{
    _executed = false;
    return _queryBuilder->keyTimestamp(key, value, from);
}
#ifdef VTAPI_HAVE_OPENCV

bool Insert::keyCvMat(const string& key, const cv::Mat& value, const string& from)
{
    _executed = false;
    return _queryBuilder->keyCvMat(key, value, from);
}
#endif

bool Insert::keyIntervalEvent(const string& key, const IntervalEvent& value, const string& from)
{
    _executed = false;
    return _queryBuilder->keyIntervalEvent(key, value, from);
}

//================================= UPDATE =====================================

Update::Update(const Commons& commons, const string& initString)
    : Query(commons, initString)
{
}

string Update::getQuery()
{
    return _queryBuilder->getUpdateQuery();
}

bool Update::setString(const string& key, const string& value, const string& from)
{
    _executed = false;
    return _queryBuilder->keyString(key, value, from);
}

bool Update::setStringA(const string& key, string* values, const int size, const string& from)
{
    _executed = false;
    return _queryBuilder->keyStringA(key, values, size, from);
}

bool Update::setInt(const string& key, int value, const string& from)
{
    _executed = false;
    return _queryBuilder->keyInt(key, value, from);
}

bool Update::setIntA(const string& key, int* values, const int size, const string& from)
{
    _executed = false;
    return _queryBuilder->keyIntA(key, values, size, from);
}

bool Update::setFloat(const string& key, float value, const string& from)
{
    _executed = false;
    return _queryBuilder->keyFloat(key, value, from);
}

bool Update::setFloatA(const string& key, float* values, const int size, const string& from)
{
    _executed = false;
    return _queryBuilder->keyFloatA(key, values, size, from);
}

bool Update::setSeqtype(const string& key, const string& value, const string& from)
{
    _executed = false;
    return _queryBuilder->keySeqtype(key, value, from);
}

bool Update::setInouttype(const string& key, const string& value, const string& from)
{
    _executed = false;
    return _queryBuilder->keyInouttype(key, value, from);
}

bool Update::updateProcessStatus(const string& key, ProcessState::STATUS_T value, const string& from)
{
    _executed = false;
    return _queryBuilder->keyPStatus(key, value, from);
}

bool Update::setTimestamp(const string& key, const time_t& value, const string& from)
{
    _executed = false;
    _queryBuilder->keyTimestamp(key, value, from);
}

bool Update::whereString(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereString(key, value, oper, from);
}

bool Update::whereInt(const string& key, const int value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereInt(key, value, oper, from);
}

bool Update::whereFloat(const string& key, const float value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereFloat(key, value, oper, from);
}

bool Update::whereSeqtype(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereSeqtype(key, value, oper, from);
}

bool Update::whereInouttype(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereInouttype(key, value, oper, from);
}

bool Update::wherePStatus(const string& key, ProcessState::STATUS_T value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->wherePStatus(key, value, oper, from);
}

bool Update::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereTimestamp(key, value, oper, from);
}

bool Update::whereRegion(const string& key, const IntervalEvent::box& value, const string& oper, const string& from)
{
    _executed = false;
    return _queryBuilder->whereRegion(key, value, oper, from);
}

bool Update::whereExpression(const string& expression, const string& value, const string& oper)
{
    _executed = false;
    return _queryBuilder->whereExpression(expression, value, oper);
}


//================================= PREDEFINED =====================================

QueryBeginTransaction::QueryBeginTransaction (const Commons& commons)
    : Query(commons)
{
    _queryBuilder->useQueryString(_queryBuilder->getBeginQuery());
}

QueryCommitTransaction::QueryCommitTransaction (const Commons& commons)
    : Query(commons)
{
    _queryBuilder->useQueryString(_queryBuilder->getCommitQuery());
}

QueryRollbackTransaction::QueryRollbackTransaction (const Commons& commons)
    : Query(commons)
{
    _queryBuilder->useQueryString(_queryBuilder->getRollbackQuery());
}

QueryDatasetCreate::QueryDatasetCreate(
                                       const Commons& commons,
                                       const string& name,
                                       const string& location,
                                       const string& friendly_name,
                                       const string& description)
    : Query(commons)
{
    _queryBuilder->useQueryString(
        _queryBuilder->getDatasetCreateQuery(name,location, friendly_name, description));
}

QueryDatasetReset::QueryDatasetReset (
                                      const Commons& commons,
                                      const string& name)
    : Query(commons)
{
    _queryBuilder->useQueryString(_queryBuilder->getDatasetResetQuery(name));
}

QueryDatasetDelete::QueryDatasetDelete (
                                        const Commons& commons,
                                        const string& name)
    : Query(commons)
{
    _queryBuilder->useQueryString(_queryBuilder->getDatasetDeleteQuery(name));
}

QueryMethodCreate::QueryMethodCreate (
                                      const Commons& commons,
                                      const string& name,
                                      const MethodKeys keys_definition,
                                      const MethodParams params_definition,
                                      const string& description)
    : Query(commons)
{
    _queryBuilder->useQueryString(
        _queryBuilder->getMethodCreateQuery(name, keys_definition, params_definition, description));
}

QueryMethodDelete::QueryMethodDelete (
                                      const Commons& commons,
                                      const string& name)
    : Query(commons)
{
    _queryBuilder->useQueryString(
        _queryBuilder->getMethodDeleteQuery(name));
}

QueryLastInsertedId::QueryLastInsertedId(const Commons& commons)
    : Query(commons)
{
    _queryBuilder->useQueryString(
                                  _queryBuilder->getLastInsertedIdQuery());
}

bool QueryLastInsertedId::execute(int &returned_id)
{
    int retval = _connection->fetch(_queryBuilder->getGenericQuery(),
                                    _queryBuilder->getQueryParam(),
                                    _resultSet);

    if (retval > 0) {
        _resultSet->setPosition(0);
        returned_id = _resultSet->getInt(0);
        return true;
    }
    else {
        return false;
    }
}

}
