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

#include <vtapi/common/global.h>
#include <vtapi/queries/query.h>
#include <vtapi/queries/select.h>
#include <vtapi/queries/insert.h>
#include <vtapi/queries/update.h>
#include <vtapi/queries/delete.h>
#include <vtapi/queries/predefined.h>

using namespace std;

namespace vtapi {


//================================== QUERY =====================================

Query::Query(const Commons& commons, const string& sql, bool sql_is_table)
    : _backend(commons.backend()), _connection(const_cast<Commons&>(commons).connection())
{
    _pquerybuilder = NULL;
    _presultset = NULL;

    Commons::CONTEXT &context = const_cast<Commons&>(commons).context();

    _pquerybuilder = _backend.createQueryBuilder(connection());

    // set default schema
    _pquerybuilder->useDefaultSchema(context.dataset);

    // set initial query string and default table
    if (sql_is_table)
        _pquerybuilder->useDefaultTable(sql);
    else
        _pquerybuilder->useQueryString(sql);

    // create resultset object
    _presultset = _backend.createResultSet(_connection.getDBTypes());

    _executed = false;
}

Query::~Query()
{
    vt_destruct(_presultset);
    vt_destruct(_pquerybuilder);
}

string Query::getQuery()
{
    return _pquerybuilder->getGenericQuery();
}

bool Query::execute()
{
    _executed = true;
    return connection().execute(this->getQuery(), _pquerybuilder->getQueryParam());
}

bool Query::isExecuted()
{
    return _executed;
}

void Query::reset()
{
    _executed = false;
    _pquerybuilder->reset();
}

QueryBuilder& Query::querybuilder()
{
    return *_pquerybuilder;
}

ResultSet& Query::resultset()
{
    return *_presultset;
}

const IBackendInterface & Query::backend()
{
    return _backend;
}

Connection & Query::connection()
{
    return _connection;
}


QueryWhere::QueryWhere(const Commons& commons, const string& table)
    : Query(commons, table, true)
{
}

QueryWhere::~QueryWhere()
{
}


bool QueryWhere::whereString(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereString(key, value, oper, from);
}

bool QueryWhere::whereInt(const string& key, const int value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereInt(key, value, oper, from);
}

bool QueryWhere::whereBool(const string& key, bool value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereBool(key, value, oper, from);
}

bool QueryWhere::whereFloat(const string& key, const float value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereFloat(key, value, oper, from);
}

bool QueryWhere::whereSeqtype(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereSeqtype(key, value, oper, from);
}

bool QueryWhere::whereInouttype(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereInouttype(key, value, oper, from);
}

bool QueryWhere::whereProcessStatus(const string& key, ProcessState::STATUS_T value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereProcessStatus(key, value, oper, from);
}

bool QueryWhere::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereTimestamp(key, value, oper, from);
}

bool QueryWhere::whereTimeRange(const string& key_start, const string& key_length, const time_t& value_start, const uint value_length, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereTimeRange(key_start, key_length, value_start, value_length, oper, from);
}

bool QueryWhere::whereRegion(const string& key, const IntervalEvent::box& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereRegion(key, value, oper, from);
}

bool QueryWhere::whereExpression(const string& expression, const string& value, const string& oper)
{
    _executed = false;
    return querybuilder().whereExpression(expression, value, oper);
}

bool QueryWhere::whereStringInList(const string& key, const list<string>& values, const string& from)
{
    _executed = false;
    return querybuilder().whereStringList(key, values, "IN", from);
}

bool QueryWhere::whereIntInList(const string& key, const list<int>& values, const string& from)
{
    _executed = false;
    return querybuilder().whereIntList(key, values, "IN", from);
}


//================================== SELECT ====================================

Select::Select(const Commons& commons, const string &table)
    : QueryWhere(commons, table)
{
    _limit = 0;
    _offset = 0;
}

string Select::getQuery()
{
    return querybuilder().getSelectQuery(_groupby, _orderby, _limit, _offset);
}

bool Select::execute()
{
    _executed = true;
    return connection().fetch(this->getQuery(), querybuilder().getQueryParam(), resultset()) >= 0;
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
    _executed = false;
    return querybuilder().keyFrom(table, column);
}

void Select::setLimit(const int limit)
{
    _limit = limit;
}

void Select::setOrderBy(const string& key)
{
    _orderby = key;
}

void Select::setGroupBy(const string& key)
{
    _groupby = key;
}

//================================== INSERT ====================================

Insert::Insert(const Commons& commons, const string& table)
    : Query(commons, table, true)
{ }

string Insert::getQuery()
{
    return querybuilder().getInsertQuery();
}

bool Insert::keyString(const string& key, const string& value)
{
    _executed = false;
    return querybuilder().keyString(key, value, string());
}

bool Insert::keyStringA(const string& key, string* values, const int size)
{
    _executed = false;
    return querybuilder().keyStringA(key, values, size, string());
}

bool Insert::keyBool(const string &key, bool value)
{
    _executed = false;
    return querybuilder().keyBool(key, value, string());
}

bool Insert::keyInt(const string& key, int value)
{
    _executed = false;
    return querybuilder().keyInt(key, value, string());
}

bool Insert::keyIntA(const string& key, int* values, const int size)
{
    _executed = false;
    return querybuilder().keyIntA(key, values, size, string());
}

bool Insert::keyFloat(const string& key, float value)
{
    _executed = false;
    return querybuilder().keyFloat(key, value, string());
}

bool Insert::keyFloatA(const string& key, float* values, const int size)
{
    _executed = false;
    return querybuilder().keyFloatA(key, values, size, string());
}

bool vtapi::Insert::keyFloat8(const string &key, double value)
{
    _executed = false;
    return querybuilder().keyFloat8(key, value, string());
}

bool vtapi::Insert::keyFloat8A(const string &key, double *values, const int size)
{
    _executed = false;
    return querybuilder().keyFloat8A(key, values, size, string());
}

bool Insert::keySeqtype(const string& key, const string& value)
{
    _executed = false;
    return querybuilder().keySeqtype(key, value, string());
}

bool Insert::keyInouttype(const string& key, const string& value)
{
    _executed = false;
    return querybuilder().keyInouttype(key, value, string());
}

bool Insert::keyTimestamp(const string& key, const time_t& value)
{
    _executed = false;
    return querybuilder().keyTimestamp(key, value, string());
}

bool Insert::keyCvMat(const string& key, const cv::Mat& value)
{
    _executed = false;
    return querybuilder().keyCvMat(key, value, string());
}

bool Insert::keyIntervalEvent(const string& key, const IntervalEvent& value)
{
    _executed = false;
    return querybuilder().keyIntervalEvent(key, value, string());
}

//================================= UPDATE =====================================

Update::Update(const Commons& commons, const string& table)
    : QueryWhere(commons, table)
{
}

string Update::getQuery()
{
    return querybuilder().getUpdateQuery();
}

bool Update::setString(const string& key, const string& value)
{
    _executed = false;
    return querybuilder().keyString(key, value, string());
}

bool Update::setStringA(const string& key, string* values, const int size)
{
    _executed = false;
    return querybuilder().keyStringA(key, values, size, string());
}

bool Update::setBool(const string& key, bool value)
{
    _executed = false;
    return querybuilder().keyBool(key, value, string());
}

bool Update::setInt(const string& key, int value)
{
    _executed = false;
    return querybuilder().keyInt(key, value, string());
}

bool Update::setIntA(const string& key, int* values, const int size)
{
    _executed = false;
    return querybuilder().keyIntA(key, values, size, string());
}

bool Update::setFloat(const string& key, float value)
{
    _executed = false;
    return querybuilder().keyFloat(key, value, string());
}

bool Update::setFloatA(const string& key, float* values, const int size)
{
    _executed = false;
    return querybuilder().keyFloatA(key, values, size, string());
}

bool Update::setSeqtype(const string& key, const string& value)
{
    _executed = false;
    return querybuilder().keySeqtype(key, value, string());
}

bool Update::setInouttype(const string& key, const string& value)
{
    _executed = false;
    return querybuilder().keyInouttype(key, value, string());
}

bool Update::setProcessStatus(const string& key, ProcessState::STATUS_T value)
{
    _executed = false;
    return querybuilder().keyProcessStatus(key, value, string());
}

bool Update::setTimestamp(const string& key, const time_t& value)
{
    _executed = false;
    querybuilder().keyTimestamp(key, value, string());
}

//================================= DELETE =====================================

Delete::Delete(const Commons& commons, const string& table)
    : QueryWhere(commons, table)
{
}

string Delete::getQuery()
{
    return querybuilder().getDeleteQuery();
}

//================================= PREDEFINED =====================================

QueryPredefined::QueryPredefined(const Commons& commons)
    : Query(commons, string(), false)
{
}

QueryPredefined::~QueryPredefined()
{
}

QueryBeginTransaction::QueryBeginTransaction (const Commons& commons)
    : QueryPredefined(commons)
{
    querybuilder().useQueryString(querybuilder().getBeginQuery());
}

QueryCommitTransaction::QueryCommitTransaction (const Commons& commons)
    : QueryPredefined(commons)
{
    querybuilder().useQueryString(querybuilder().getCommitQuery());
}

QueryRollbackTransaction::QueryRollbackTransaction (const Commons& commons)
    : QueryPredefined(commons)
{
    querybuilder().useQueryString(querybuilder().getRollbackQuery());
}

QueryDatasetCreate::QueryDatasetCreate(
                                       const Commons& commons,
                                       const string& name,
                                       const string& location,
                                       const string& friendly_name,
                                       const string& description)
    : QueryPredefined(commons)
{
    querybuilder().useQueryString(querybuilder().getDatasetCreateQuery(name,
                                                                       location,
                                                                       friendly_name,
                                                                       description));
}

QueryDatasetReset::QueryDatasetReset (const Commons& commons,
                                      const string& name)
    : QueryPredefined(commons)
{
    querybuilder().useQueryString(querybuilder().getDatasetResetQuery(name));
}

QueryDatasetDelete::QueryDatasetDelete (const Commons& commons,
                                        const string& name)
    : QueryPredefined(commons)
{
    querybuilder().useQueryString(querybuilder().getDatasetDeleteQuery(name));
}

QueryMethodCreate::QueryMethodCreate (const Commons& commons,
                                      const string& name,
                                      const MethodKeys keys_definition,
                                      const MethodParams params_definition,
                                      const string& description)
    : QueryPredefined(commons)
{
    querybuilder().useQueryString(
        querybuilder().getMethodCreateQuery(name, keys_definition, params_definition, description));
}

QueryMethodDelete::QueryMethodDelete (const Commons& commons,
                                      const string& name)
    : QueryPredefined(commons)
{
    querybuilder().useQueryString(querybuilder().getMethodDeleteQuery(name));
}

QueryTaskCreate::QueryTaskCreate(const Commons& commons,
                                 const string& name,
                                 const string& dsname,
                                 const string& mtname,
                                 const string& params,
                                 const string &prereq_task,
                                 const string& outputs)
    : QueryPredefined(commons)
{
    querybuilder().useQueryString(querybuilder().getTaskCreateQuery(name,
                                                                    dsname,
                                                                    mtname,
                                                                    params,
                                                                    prereq_task,
                                                                    outputs));
}

QueryTaskDelete::QueryTaskDelete(const Commons& commons,
                                 const string &dsname,
                                 const string &taskname)
    : QueryPredefined(commons)
{
    querybuilder().useQueryString(querybuilder().getTaskDeleteQuery(dsname, taskname));
}


QueryLastInsertedId::QueryLastInsertedId(const Commons& commons)
    : QueryPredefined(commons)
{
    querybuilder().useQueryString(querybuilder().getLastInsertedIdQuery());

    _last_id = 0;
}

bool QueryLastInsertedId::execute()
{
    int retval = connection().fetch(querybuilder().getGenericQuery(),
                                    querybuilder().getQueryParam(),
                                    resultset());

    if (retval > 0) {
        resultset().setPosition(0);
        _last_id = resultset().getInt(0);
        return true;
    }
    else {
        return false;
    }
}

int QueryLastInsertedId::getLastId()
{
    return _last_id;
}


}
