/**
 * @file    query.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of Query, Select, Insert and Update classes
 */

#include <common/vtapi_global.h>
#include <backends/vtapi_backendfactory.h>
#include <backends/vtapi_backends.h>
#include <queries/vtapi_queries.h>

using std::string;

using namespace vtapi;


//================================== QUERY =====================================


Query::Query(const Commons& commons, const string& initString)
: Commons(commons) {
    thisClass       = "Query";

    queryBuilder    = fmap ? g_BackendFactory.createQueryBuilder(fmap, connection, logger, initString) : NULL;
    resultSet       = fmap ? g_BackendFactory.createResultSet(fmap, typeManager, logger) : NULL;
    if (queryBuilder) {
        this->queryBuilder->setDataset(this->dataset);
        this->queryBuilder->setTable(this->selection);
    }
    executed        = false;
}

Query::~Query() {
    if (!executed) logger-> warning(208, "The query was not executed after the last change\n" + this->getQuery(), thisClass+"::~Query()");

    vt_destruct(resultSet);
    vt_destruct(queryBuilder);
}

string Query::getQuery() {
    return queryBuilder->getGenericQuery();
}

void Query::reset() {
    executed = false;
    queryBuilder->reset();
}

bool Query::execute() {
    bool retval = VT_OK;
    string queryString = this->getQuery();

    logger->debug("SQL query:\n" + queryString);
    retval = connection->execute(queryString, queryBuilder->getParam());
    executed = true;

    if (!retval) {
        logger->warning(200, "Query failed\n"+connection->getErrorMessage(), thisClass+"::execute()");
    }
    else {
        logger->debug("Query succeeded");
    }

    return retval;
}

bool Query::checkQueryObject() {
    return (queryBuilder && resultSet);
}

//================================== SELECT ====================================


Select::Select(const Commons& commons, const string& initString)
: Query(commons, initString) {
    thisClass = "Select";
    this->limit = queryLimit;
    this->offset = 0;
}

string Select::getQuery() {
    return queryBuilder->getSelectQuery(groupby, orderby, limit, offset);
}

bool Select::function(const string& funtext) {
    // TODO: 
}

bool Select::execute() {
    int result = 0;
    string queryString = this->getQuery();

    logger->debug("Select query:\n" + queryString);
    result = connection->fetch(queryString, queryBuilder->getParam(), resultSet);
    executed = true;

    if (result < 0) {
        logger->warning(200, "SELECT failed\n"+connection->getErrorMessage(), thisClass+"::execute()");
        return VT_FAIL;
    }
    else {        
        logger->debug("SELECT succeeded, "+toString(result)+" row(s) returned");
        return VT_OK;
    }
}

bool Select::executeNext() {
    if (limit > 0) {
        offset += limit;
        return this->execute();
    }
    else return VT_FAIL;
}

bool Select::from(const string& table, const string& column) {
    bool retval = VT_OK;

    retval &= this->queryBuilder->keyFrom(table, column);
    executed = false;

    return retval;
}

bool Select::whereString(const string& key, const string& value, const string& oper, const string& from) {
    executed = false;
    return this->queryBuilder->whereString(key, value, oper, from);
}
bool Select::whereInt(const string& key, const int value, const string& oper, const string& from) {
    executed = false;
    return this->queryBuilder->whereInt(key, value, oper, from);
}
bool Select::whereFloat(const string& key, const float value, const string& oper, const string& from) {
    executed = false;
    return this->queryBuilder->whereFloat(key, value, oper, from);
}
bool Select::whereSeqtype(const string& key, const string& value, const string& oper, const string& from) {
    executed = false;
    return this->queryBuilder->whereSeqtype(key, value, oper, from);
}
bool Select::whereInouttype(const string& key, const string& value, const string& oper, const string& from) {
    executed = false;
    return this->queryBuilder->whereInouttype(key, value, oper, from);
}
bool Select::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from) {
    executed = false;
    return this->queryBuilder->whereTimestamp(key, value, oper, from);
}

//================================== INSERT ====================================


Insert::Insert(const Commons& commons, const string& initString)
: Query(commons, initString) {
    thisClass = "Insert";
}

string Insert::getQuery() {
    return queryBuilder->getInsertQuery();
}


bool Insert::execute() {
    bool retval = VT_OK;
    string queryString = this->getQuery();

    logger->debug("Insert query:\n" + queryString);
    retval = connection->execute(queryString, queryBuilder->getParam());
    executed = true;
    if (retval) {
        logger->debug("INSERT succeeded");
    }
    else {
        logger->warning(200, "INSERT failed\n"+connection->getErrorMessage(), thisClass+"::execute()");
    }

    return retval;
}

bool Insert::keyString(const string& key, const string& value, const string& from) {
    executed = false;
    return this->queryBuilder->keyString(key, value, from);
}
bool Insert::keyStringA(const string& key, string* values, const int size, const string& from) {
    executed = false;
    return this->queryBuilder->keyStringA(key, values, size, from);
}
bool Insert::keyInt(const string& key, int value, const string& from) {
    executed = false;
    return this->queryBuilder->keyInt(key, value, from);
}
bool Insert::keyIntA(const string& key, int* values, const int size, const string& from) {
    executed = false;
    return this->queryBuilder->keyIntA(key, values, size, from);
}
bool Insert::keyFloat(const string& key, float value, const string& from){
    executed = false;
    return this->queryBuilder->keyFloat(key, value, from);
}
bool Insert::keyFloatA(const string& key, float* values, const int size, const string& from){
    executed = false;
    return this->queryBuilder->keyFloatA(key, values, size, from);
}
bool Insert::keySeqtype(const string& key, const string& value, const string& from){
    executed = false;
    return this->queryBuilder->keySeqtype(key, value, from);
}
bool Insert::keyInouttype(const string& key, const string& value, const string& from){
    executed = false;
    return this->queryBuilder->keyInouttype(key, value, from);
}
//bool Insert::keyPermissions(const string& key, const string& value, const string& from){
//    executed = false;
//    this->queryBuilder->keyPermissions(key, value, from);
//}
bool Insert::keyTimestamp(const string& key, const time_t& value, const string& from){
    executed = false;
    this->queryBuilder->keyTimestamp(key, value, from);
}


//================================= UPDATE =====================================


Update::Update(const Commons& commons, const string& initString)
: Query(commons, initString) {
    thisClass = "Update";

}

string Update::getQuery() {
    return queryBuilder->getUpdateQuery();
}

bool Update::execute() {
    bool retval = VT_OK;
    string queryString = this->getQuery();

    logger->debug("Update query:\n" + queryString);
    retval = connection->execute(queryString, queryBuilder->getParam());
    executed = true;

    if (retval) {
        logger->debug("UPDATE succeeded");
    }
    else {
        logger->warning(200, "UPDATE failed\n"+connection->getErrorMessage(), thisClass+"::execute()");
    }

    return retval;
}

bool Update::setString(const string& key, const string& value, const string& from) {
    executed = false;
    return this->queryBuilder->keyString(key, value, from);
}
bool Update::setStringA(const string& key, string* values, const int size, const string& from) {
    executed = false;
    return this->queryBuilder->keyStringA(key, values, size, from);
}
bool Update::setInt(const string& key, int value, const string& from) {
    executed = false;
    return this->queryBuilder->keyInt(key, value, from);
}
bool Update::setIntA(const string& key, int* values, const int size, const string& from) {
    executed = false;
    return this->queryBuilder->keyIntA(key, values, size, from);
}
bool Update::setFloat(const string& key, float value, const string& from){
    executed = false;
    return this->queryBuilder->keyFloat(key, value, from);
}
bool Update::setFloatA(const string& key, float* values, const int size, const string& from){
    executed = false;
    return this->queryBuilder->keyFloatA(key, values, size, from);
}
bool Update::setSeqtype(const string& key, const string& value, const string& from){
    executed = false;
    return this->queryBuilder->keySeqtype(key, value, from);
}
bool Update::setInouttype(const string& key, const string& value, const string& from){
    executed = false;
    return this->queryBuilder->keyInouttype(key, value, from);
}
//bool Update::setPermissions(const string& key, const string& value, const string& from){
//    executed = false;
//    this->queryBuilder->keyPermissions(key, value, from);
//}
bool Update::setTimestamp(const string& key, const time_t& value, const string& from){
    executed = false;
    this->queryBuilder->keyTimestamp(key, value, from);
}

bool Update::whereString(const string& key, const string& value, const string& oper, const string& from) {
    executed = false;
    return this->queryBuilder->whereString(key, value, oper, from);
}
bool Update::whereInt(const string& key, const int value, const string& oper, const string& from) {
    executed = false;
    return this->queryBuilder->whereInt(key, value, oper, from);
}
bool Update::whereFloat(const string& key, const float value, const string& oper, const string& from) {
    executed = false;
    return this->queryBuilder->whereFloat(key, value, oper, from);
}
bool Update::whereSeqtype(const string& key, const string& value, const string& oper, const string& from) {
    executed = false;
    return this->queryBuilder->whereSeqtype(key, value, oper, from);
}
bool Update::whereInouttype(const string& key, const string& value, const string& oper, const string& from) {
    executed = false;
    return this->queryBuilder->whereInouttype(key, value, oper, from);
}
bool Update::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from) {
    executed = false;
    return this->queryBuilder->whereTimestamp(key, value, oper, from);
}