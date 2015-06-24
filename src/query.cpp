/**
 * @file
 * @brief   Methods of Query, Select, Insert and Update classes
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
#include <backends/vtapi_backendfactory.h>
#include <queries/vtapi_query.h>
#include <queries/vtapi_select.h>
#include <queries/vtapi_insert.h>
#include <queries/vtapi_update.h>

using namespace std;

namespace vtapi {


//================================== QUERY =====================================


Query::Query(const Commons& commons, const string& initString)
: Commons(commons)
{
    this->thisClass = "Query";

    bool bIsQuery = (initString.find_first_of(" \t\n") != string::npos);
    
    queryBuilder = BackendFactory::createQueryBuilder(
        backend,
        *backendBase,
        connection->getConnectionObject(),
        bIsQuery ? initString : "");
    
    const string &defTable = bIsQuery ? this->selection : initString;
    queryBuilder->useDefaultTable(defTable);
    queryBuilder->useDefaultSchema(this->dataset);

    resultSet = BackendFactory::createResultSet(backend, *backendBase, connection->getDBTypes());

    executed = false;
}

Query::~Query()
{
    //if (!executed) logger-> warning(208, "The query was not executed after the last change\n" + this->getQuery(), thisClass+"::~Query()");

    vt_destruct(resultSet);
    vt_destruct(queryBuilder);
}

string Query::getQuery() {
    return queryBuilder->getGenericQuery();
}

bool Query::beginTransaction()
{
    return connection->execute(this->queryBuilder->getBeginQuery(), NULL);
}

bool Query::commitTransaction()
{
    return connection->execute(this->queryBuilder->getCommitQuery(), NULL);
}

bool Query::rollbackTransaction()
{
    return connection->execute(this->queryBuilder->getRollbackQuery(), NULL);
}

bool Query::execute()
{
    bool retval = true;
    string queryString = this->getQuery();

    logger->debug("SQL query:\n" + queryString);
    retval = connection->execute(queryString, queryBuilder->getQueryParam());
    executed = true;

    if (!retval) {
        logger->warning(200, "Query failed\n"+connection->getErrorMessage(), thisClass+"::execute()");
    }
    else {
        logger->debug("Query succeeded");
    }

    return retval;
}

void Query::reset()
{
    executed = false;
    queryBuilder->reset();
}

bool Query::checkQueryObject() {
    return (queryBuilder && resultSet);
}

//================================== SELECT ====================================


Select::Select(const Commons& commons, const string& initString)
: Query(commons, initString)
{
    this->thisClass = "Select";
    
    this->limit = queryLimit;
    this->offset = 0;
}

string Select::getQuery()
{
    return queryBuilder->getSelectQuery(groupby, orderby, limit, offset);
}

bool Select::execute()
{
    int result = 0;
    string queryString = this->getQuery();

    logger->debug("Select query:\n" + queryString);
    result = connection->fetch(queryString, queryBuilder->getQueryParam(), resultSet);
    executed = true;

    if (result < 0) {
        logger->warning(200, "SELECT failed\n"+connection->getErrorMessage(), thisClass+"::execute()");
        return false;
    }
    else {        
        logger->debug("SELECT succeeded, "+toString(result)+" row(s) returned");
        return true;
    }
}

bool Select::executeNext()
{
    if (limit > 0) {
        offset += limit;
        return this->execute();
    }
    else {
        return false;
    }
}

bool Select::from(const string& table, const string& column)
{
    bool retval = true;

    retval &= this->queryBuilder->keyFrom(table, column);
    executed = false;

    return retval;
}

void Select::orderBy(const string& key)
{
    this->orderby = key;
}


bool Select::whereString(const string& key, const string& value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereString(key, value, oper, from);
}
bool Select::whereInt(const string& key, const int value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereInt(key, value, oper, from);
}
bool Select::whereFloat(const string& key, const float value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereFloat(key, value, oper, from);
}
bool Select::whereSeqtype(const string& key, const string& value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereSeqtype(key, value, oper, from);
}
bool Select::whereInouttype(const string& key, const string& value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereInouttype(key, value, oper, from);
}
bool Select::wherePStatus(const string& key, ProcessState::STATUS_T value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->wherePStatus(key, value, oper, from);
}
bool Select::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereTimestamp(key, value, oper, from);
}
bool Select::whereTimeRange(const string& key_start, const string& key_length, const time_t& value_start, const uint value_length, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereTimeRange(key_start, key_length, value_start, value_length, oper, from);
}
bool Select::whereRegion(const string& key, const IntervalEvent::box& value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereRegion(key, value, oper, from);
}
bool Select::whereExpression(const string& expression, const std::string& value, const string& oper) {
    executed = false;
    return this->queryBuilder->whereExpression(expression, value, oper);
}


//================================== INSERT ====================================


Insert::Insert(const Commons& commons, const string& initString)
: Query(commons, initString)
{
    thisClass = "Insert";
    
}

string Insert::getQuery() {
    return queryBuilder->getInsertQuery();
}


bool Insert::execute()
{
    bool retval = true;
    string queryString = this->getQuery();

    logger->debug("Insert query:\n" + queryString);
    retval = connection->execute(queryString, queryBuilder->getQueryParam());
    executed = true;
    if (retval) {
        logger->debug("INSERT succeeded");
    }
    else {
        logger->warning(200, "INSERT failed\n"+connection->getErrorMessage(), thisClass+"::execute()");
    }

    return retval;
}

bool Insert::keyString(const string& key, const string& value, const string& from)
{
    executed = false;
    return this->queryBuilder->keyString(key, value, from);
}
bool Insert::keyStringA(const string& key, string* values, const int size, const string& from)
{
    executed = false;
    return this->queryBuilder->keyStringA(key, values, size, from);
}
bool Insert::keyInt(const string& key, int value, const string& from)
{
    executed = false;
    return this->queryBuilder->keyInt(key, value, from);
}
bool Insert::keyIntA(const string& key, int* values, const int size, const string& from)
{
    executed = false;
    return this->queryBuilder->keyIntA(key, values, size, from);
}
bool Insert::keyFloat(const string& key, float value, const string& from)
{
    executed = false;
    return this->queryBuilder->keyFloat(key, value, from);
}
bool Insert::keyFloatA(const string& key, float* values, const int size, const string& from)
{
    executed = false;
    return this->queryBuilder->keyFloatA(key, values, size, from);
}
bool Insert::keySeqtype(const string& key, const string& value, const string& from)
{
    executed = false;
    return this->queryBuilder->keySeqtype(key, value, from);
}
bool Insert::keyInouttype(const string& key, const string& value, const string& from)
{
    executed = false;
    return this->queryBuilder->keyInouttype(key, value, from);
}
bool Insert::keyTimestamp(const string& key, const time_t& value, const string& from)
{
    executed = false;
    return this->queryBuilder->keyTimestamp(key, value, from);
}
#ifdef VTAPI_HAVE_OPENCV
bool Insert::keyCvMat(const std::string& key, const cv::Mat& value, const std::string& from)
{
    executed = false;
    return this->queryBuilder->keyCvMat(key, value, from);
}
#endif

bool Insert::keyIntervalEvent(const std::string& key, const IntervalEvent& value, const std::string& from)
{
    executed = false;
    return this->queryBuilder->keyIntervalEvent(key, value, from);
}

//================================= UPDATE =====================================


Update::Update(const Commons& commons, const string& initString)
: Query(commons, initString)
{
    thisClass = "Update";

}

string Update::getQuery() {
    return queryBuilder->getUpdateQuery();
}

bool Update::execute()
{
    bool retval = true;
    string queryString = this->getQuery();

    logger->debug("Update query:\n" + queryString);
    retval = connection->execute(queryString, queryBuilder->getQueryParam());
    executed = true;

    if (retval) {
        logger->debug("UPDATE succeeded");
    }
    else {
        logger->warning(200, "UPDATE failed\n"+connection->getErrorMessage(), thisClass+"::execute()");
    }

    return retval;
}

bool Update::setString(const string& key, const string& value, const string& from)
{
    executed = false;
    return this->queryBuilder->keyString(key, value, from);
}
bool Update::setStringA(const string& key, string* values, const int size, const string& from)
{
    executed = false;
    return this->queryBuilder->keyStringA(key, values, size, from);
}
bool Update::setInt(const string& key, int value, const string& from)
{
    executed = false;
    return this->queryBuilder->keyInt(key, value, from);
}
bool Update::setIntA(const string& key, int* values, const int size, const string& from)
{
    executed = false;
    return this->queryBuilder->keyIntA(key, values, size, from);
}
bool Update::setFloat(const string& key, float value, const string& from)
{
    executed = false;
    return this->queryBuilder->keyFloat(key, value, from);
}
bool Update::setFloatA(const string& key, float* values, const int size, const string& from)
{
    executed = false;
    return this->queryBuilder->keyFloatA(key, values, size, from);
}
bool Update::setSeqtype(const string& key, const string& value, const string& from)
{
    executed = false;
    return this->queryBuilder->keySeqtype(key, value, from);
}
bool Update::setInouttype(const string& key, const string& value, const string& from)
{
    executed = false;
    return this->queryBuilder->keyInouttype(key, value, from);
}
bool Update::updateProcessStatus(const string& key, ProcessState::STATUS_T value, const string& from)
{
    executed = false;
    return this->queryBuilder->keyPStatus(key, value, from);
}
bool Update::setTimestamp(const string& key, const time_t& value, const string& from)
{
    executed = false;
    this->queryBuilder->keyTimestamp(key, value, from);
}

bool Update::whereString(const string& key, const string& value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereString(key, value, oper, from);
}
bool Update::whereInt(const string& key, const int value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereInt(key, value, oper, from);
}
bool Update::whereFloat(const string& key, const float value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereFloat(key, value, oper, from);
}
bool Update::whereSeqtype(const string& key, const string& value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereSeqtype(key, value, oper, from);
}
bool Update::whereInouttype(const string& key, const string& value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereInouttype(key, value, oper, from);
}
bool Update::wherePStatus(const string& key, ProcessState::STATUS_T value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->wherePStatus(key, value, oper, from);
}
bool Update::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereTimestamp(key, value, oper, from);
}
bool Update::whereRegion(const string& key, const IntervalEvent::box& value, const string& oper, const string& from)
{
    executed = false;
    return this->queryBuilder->whereRegion(key, value, oper, from);
}
bool Update::whereExpression(const string& expression, const std::string& value, const string& oper) {
    executed = false;
    return this->queryBuilder->whereExpression(expression, value, oper);
}

}
