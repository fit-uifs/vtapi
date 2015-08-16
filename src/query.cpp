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
#include <vtapi/queries/predefined.h>

using namespace std;

namespace vtapi {


//================================== QUERY =====================================

Query::Query(const Commons& commons, const string& sql)
    : Commons(commons, false)
{
    _pquerybuilder = NULL;
    _presultset = NULL;

    // query builder may be initialized with query or just default table name
    bool bIsQuery = (sql.find_first_of(" \t\n") != string::npos);
    if (bIsQuery) {
        // init string is query => set it to query builder, default table is selection
        _pquerybuilder = backend().createQueryBuilder(connection(), sql);

        if (!context().selection.empty())
            _pquerybuilder->useDefaultTable(context().selection);
    }
    else {
        // init string isn't query => it is default table
        _pquerybuilder = backend().createQueryBuilder(connection(), string());
        if (!sql.empty())
            _pquerybuilder->useDefaultTable(sql);
    }
    _pquerybuilder->useDefaultSchema(context().dataset);

    _presultset = backend().createResultSet(connection().getDBTypes());

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

bool Query::isExecuted()
{
    return _executed;
}

//================================== SELECT ====================================

Select::Select(const Commons& commons, const string& initString)
    : Query(commons, initString)
{
    _limit = 10000;
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
int Select::getLimit()
{
    return _limit;
}

void Select::setLimit(const int limit)
{
    _limit = limit;
}

bool Select::from(const string& table, const string& column)
{
    bool retval = true;

    retval &= querybuilder().keyFrom(table, column);
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
    return querybuilder().whereString(key, value, oper, from);
}

bool Select::whereInt(const string& key, const int value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereInt(key, value, oper, from);
}

bool Select::whereBool(const string& key, bool value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereBool(key, value, oper, from);
}

bool Select::whereFloat(const string& key, const float value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereFloat(key, value, oper, from);
}

bool Select::whereSeqtype(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereSeqtype(key, value, oper, from);
}

bool Select::whereInouttype(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereInouttype(key, value, oper, from);
}

bool Select::wherePStatus(const string& key, ProcessState::STATUS_T value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().wherePStatus(key, value, oper, from);
}

bool Select::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereTimestamp(key, value, oper, from);
}

bool Select::whereTimeRange(const string& key_start, const string& key_length, const time_t& value_start, const uint value_length, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereTimeRange(key_start, key_length, value_start, value_length, oper, from);
}

bool Select::whereRegion(const string& key, const IntervalEvent::box& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereRegion(key, value, oper, from);
}

bool Select::whereExpression(const string& expression, const string& value, const string& oper)
{
    _executed = false;
    return querybuilder().whereExpression(expression, value, oper);
}

bool Select::whereStringInList(const string& key, const list<string>& values, const string& from)
{
    _executed = false;
    return querybuilder().whereStringList(key, values, "IN", from);
}

bool Select::whereIntInList(const string& key, const list<int>& values, const string& from)
{
    _executed = false;
    return querybuilder().whereIntList(key, values, "IN", from);
}


//================================== INSERT ====================================

Insert::Insert(const Commons& commons, const string& initString)
    : Query(commons, initString)
{ }

string Insert::getQuery()
{
    return querybuilder().getInsertQuery();
}

bool Insert::keyString(const string& key, const string& value, const string& from)
{
    _executed = false;
    return querybuilder().keyString(key, value, from);
}

bool Insert::keyStringA(const string& key, string* values, const int size, const string& from)
{
    _executed = false;
    return querybuilder().keyStringA(key, values, size, from);
}

bool Insert::keyBool(const string &key, bool value, const string &from)
{
    _executed = false;
    return querybuilder().keyBool(key, value, from);
}

bool Insert::keyInt(const string& key, int value, const string& from)
{
    _executed = false;
    return querybuilder().keyInt(key, value, from);
}

bool Insert::keyIntA(const string& key, int* values, const int size, const string& from)
{
    _executed = false;
    return querybuilder().keyIntA(key, values, size, from);
}

bool Insert::keyFloat(const string& key, float value, const string& from)
{
    _executed = false;
    return querybuilder().keyFloat(key, value, from);
}

bool Insert::keyFloatA(const string& key, float* values, const int size, const string& from)
{
    _executed = false;
    return querybuilder().keyFloatA(key, values, size, from);
}

bool Insert::keySeqtype(const string& key, const string& value, const string& from)
{
    _executed = false;
    return querybuilder().keySeqtype(key, value, from);
}

bool Insert::keyInouttype(const string& key, const string& value, const string& from)
{
    _executed = false;
    return querybuilder().keyInouttype(key, value, from);
}

bool Insert::keyTimestamp(const string& key, const time_t& value, const string& from)
{
    _executed = false;
    return querybuilder().keyTimestamp(key, value, from);
}

bool Insert::keyCvMat(const string& key, const cv::Mat& value, const string& from)
{
    _executed = false;
    return querybuilder().keyCvMat(key, value, from);
}

bool Insert::keyIntervalEvent(const string& key, const IntervalEvent& value, const string& from)
{
    _executed = false;
    return querybuilder().keyIntervalEvent(key, value, from);
}

//================================= UPDATE =====================================

Update::Update(const Commons& commons, const string& initString)
    : Query(commons, initString)
{
}

string Update::getQuery()
{
    return querybuilder().getUpdateQuery();
}

bool Update::setString(const string& key, const string& value, const string& from)
{
    _executed = false;
    return querybuilder().keyString(key, value, from);
}

bool Update::setStringA(const string& key, string* values, const int size, const string& from)
{
    _executed = false;
    return querybuilder().keyStringA(key, values, size, from);
}

bool Update::setBool(const string& key, bool value, const string& from)
{
    _executed = false;
    return querybuilder().keyBool(key, value, from);
}

bool Update::setInt(const string& key, int value, const string& from)
{
    _executed = false;
    return querybuilder().keyInt(key, value, from);
}

bool Update::setIntA(const string& key, int* values, const int size, const string& from)
{
    _executed = false;
    return querybuilder().keyIntA(key, values, size, from);
}

bool Update::setFloat(const string& key, float value, const string& from)
{
    _executed = false;
    return querybuilder().keyFloat(key, value, from);
}

bool Update::setFloatA(const string& key, float* values, const int size, const string& from)
{
    _executed = false;
    return querybuilder().keyFloatA(key, values, size, from);
}

bool Update::setSeqtype(const string& key, const string& value, const string& from)
{
    _executed = false;
    return querybuilder().keySeqtype(key, value, from);
}

bool Update::setInouttype(const string& key, const string& value, const string& from)
{
    _executed = false;
    return querybuilder().keyInouttype(key, value, from);
}

bool Update::updateProcessStatus(const string& key, ProcessState::STATUS_T value, const string& from)
{
    _executed = false;
    return querybuilder().keyPStatus(key, value, from);
}

bool Update::setTimestamp(const string& key, const time_t& value, const string& from)
{
    _executed = false;
    querybuilder().keyTimestamp(key, value, from);
}

bool Update::whereString(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereString(key, value, oper, from);
}

bool Update::whereBool(const string& key, bool value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereBool(key, value, oper, from);
}

bool Update::whereInt(const string& key, const int value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereInt(key, value, oper, from);
}

bool Update::whereFloat(const string& key, const float value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereFloat(key, value, oper, from);
}

bool Update::whereSeqtype(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereSeqtype(key, value, oper, from);
}

bool Update::whereInouttype(const string& key, const string& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereInouttype(key, value, oper, from);
}

bool Update::wherePStatus(const string& key, ProcessState::STATUS_T value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().wherePStatus(key, value, oper, from);
}

bool Update::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereTimestamp(key, value, oper, from);
}

bool Update::whereRegion(const string& key, const IntervalEvent::box& value, const string& oper, const string& from)
{
    _executed = false;
    return querybuilder().whereRegion(key, value, oper, from);
}

bool Update::whereExpression(const string& expression, const string& value, const string& oper)
{
    _executed = false;
    return querybuilder().whereExpression(expression, value, oper);
}


//================================= PREDEFINED =====================================

QueryBeginTransaction::QueryBeginTransaction (const Commons& commons)
    : Query(commons)
{
    querybuilder().useQueryString(querybuilder().getBeginQuery());
}

QueryCommitTransaction::QueryCommitTransaction (const Commons& commons)
    : Query(commons)
{
    querybuilder().useQueryString(querybuilder().getCommitQuery());
}

QueryRollbackTransaction::QueryRollbackTransaction (const Commons& commons)
    : Query(commons)
{
    querybuilder().useQueryString(querybuilder().getRollbackQuery());
}

QueryDatasetCreate::QueryDatasetCreate(
                                       const Commons& commons,
                                       const string& name,
                                       const string& location,
                                       const string& friendly_name,
                                       const string& description)
    : Query(commons)
{
    querybuilder().useQueryString(querybuilder().getDatasetCreateQuery(name,
                                                                       location,
                                                                       friendly_name,
                                                                       description));
}

QueryDatasetReset::QueryDatasetReset (const Commons& commons,
                                      const string& name)
    : Query(commons)
{
    querybuilder().useQueryString(querybuilder().getDatasetResetQuery(name));
}

QueryDatasetDelete::QueryDatasetDelete (const Commons& commons,
                                        const string& name)
    : Query(commons)
{
    querybuilder().useQueryString(querybuilder().getDatasetDeleteQuery(name));
}

QueryMethodCreate::QueryMethodCreate (const Commons& commons,
                                      const string& name,
                                      const MethodKeys keys_definition,
                                      const MethodParams params_definition,
                                      const string& description)
    : Query(commons)
{
    querybuilder().useQueryString(
        querybuilder().getMethodCreateQuery(name, keys_definition, params_definition, description));
}

QueryMethodDelete::QueryMethodDelete (const Commons& commons,
                                      const string& name)
    : Query(commons)
{
    querybuilder().useQueryString(querybuilder().getMethodDeleteQuery(name));
}

QuerySequenceDelete::QuerySequenceDelete(const Commons &commons, const string &name)
    : Query(commons)
{
    querybuilder().useQueryString(querybuilder().getSequenceDeleteQuery(name));
}

QueryTaskCreate::QueryTaskCreate(const Commons& commons,
                                 const string& name,
                                 const string& dsname,
                                 const string& mtname,
                                 const string& params,
                                 const string &prereq_task,
                                 const string& outputs)
    : Query(commons)
{
    context().dataset = dsname;
    querybuilder().useQueryString(querybuilder().getTaskCreateQuery(name,
                                                                    mtname,
                                                                    params,
                                                                    prereq_task,
                                                                    outputs));
}

QueryTaskDelete::QueryTaskDelete(const Commons& commons, const string& taskname)
    : Query(commons)
{
    querybuilder().useQueryString(querybuilder().getTaskDeleteQuery(taskname));
}


QueryLastInsertedId::QueryLastInsertedId(const Commons& commons)
    : Query(commons)
{
    querybuilder().useQueryString(querybuilder().getLastInsertedIdQuery());
}

bool QueryLastInsertedId::execute(int &returned_id)
{
    int retval = connection().fetch(querybuilder().getGenericQuery(),
                                    querybuilder().getQueryParam(),
                                    resultset());

    if (retval > 0) {
        resultset().setPosition(0);
        returned_id = resultset().getInt(0);
        return true;
    }
    else {
        return false;
    }
}

}
