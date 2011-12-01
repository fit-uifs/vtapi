/*
 * File:   query.cpp
 * Author: chmelarp
 *
 * Created on 18. October 2011, 9:50
 */

#include "vtapi.h"
#include <iostream>

Query::Query(const Commons& commons, const String& query, PGparam* param)
: Commons(commons), queryString(query), param(param), res(NULL), executed(false) {
    thisClass = "Query";

    if (!param) this->param = PQparamCreate(connector->conn);
}

Query::~Query() {
    if (!executed) warning(208, "The query was not executed after the last change\n" + queryString);

    PQclear(res);
    PQparamClear(param);

    this->beDoomed();
}

String Query::getQuery() {
    if (queryString.empty()) warning(201, "Query is a virtual class... check whether got what wanted");
    return queryString;
}

bool Query::execute() {
    queryString = this->getQuery();

    if (verbose) logger->debug(queryString);
    res = PQparamExec(connector->getConn(), param, queryString.c_str(), PGF);

    if (!res) error(200, "Query::execute():\n" + String(PQgeterror()));
    else {
        executed = true;

        if (verbose) {
            if (PQresultStatus(res) == PGRES_TUPLES_OK) {
                logger->debug(toString(PQntuples(res)) + " row(s) returned");
            }
            else if (PQresultStatus(res) == PGRES_COMMAND_OK) {
                logger->debug(toString(PQcmdTuples(res)) + " row(s) (not)affected");
            }
            else {
                warning(2011, "You should never see this warning since 2011 :(");
            }
        }
    }

    return (res);
}

bool Query::keyValue(const TKey& key) {
    if (key.size > 0) warning("TODO");
    // keys.push_back(key);

    executed = false;
    return true;
}

bool Query::keyString(const String& key, const String& value, const String& from) {
    TKey k("varchar", key, 1, from);
    keys.push_back(k);
    PQputf(param, "%varchar", value.c_str());

    executed = false;
    return true;
}

bool Query::keyInt(const String& key, int value, const String& from) {
    TKey k("int4", key, 1, from);
    keys.push_back(k);
    PQputf(param, "%int4", value);

    executed = false;
    return true;
}

bool Query::keyIntA(const String& key, const int* values, const int size, const String& from) {
    TKey k("int4[]", key, size, from);
    keys.push_back(k);

    PGarray arr;
    arr.ndims = 0; // one dimensional arrays do not require setting dimension info
    // FIXME: this is a potential bug
    // arr.lbound[0] = 1;
    arr.param = PQparamCreate(connector->conn);

    // put the array elements
    for(int i = 0; i < size; ++i) {
        PQputf(arr.param, "%int4", values[i]);
    }

    PQputf(param, "%int4[]", &arr);
    PQparamClear(arr.param);

    executed = false;
    return true;
}

bool Query::keyFloat(const String& key, const float value, const String& from) {
    TKey k("float4", key, 1, from);
    keys.push_back(k);
    PQputf(param, "%float4", value);

    executed = false;
    return true;
}

bool Query::keyFloatA(const String& key, const float* values, const int size, const String& from) {
    TKey k("float4[]", key, size, from);
    keys.push_back(k);

    PGarray arr;
    arr.ndims = 0; // one dimensional arrays do not require setting dimension info
    // FIXME: this is a potential bug
    // arr.lbound[0] = 1;
    arr.param = PQparamCreate(connector->conn);

    // put the array elements
    for(int i = 0; i < size; ++i) {
        PQputf(arr.param, "%float4", values[i]);
    }

    PQputf(param, "%float4[]", &arr);
    PQparamClear(arr.param);

    executed = false;
    return true;
}



// FIXME: tohle se musi predelat na TKeyValue
bool Query::whereString(const String& column, const String& value, const String& oper, const String& table) {
    if (value.empty()) return false;

    if (!where.empty()) where += " AND ";

    if (value.compare("NULL") == 0) {
        where += column + " IS NULL";
    } else {
        // FIXME: buffer overflow!! use params!
        where += column + "=" + String(PQescapeLiteral(connector->conn, value.c_str(), value.length()));
    }

    executed = false;
    return true;
}


// FIXME: tohle se musi predelat na TKeyValue
bool Query::whereInt(const String& column, const int value, const String& oper, const String& table) {
    if (!where.empty()) where += " AND ";

    where += column + "=" + toString(value);

    executed = false;
    return true;
}

// FIXME: tohle se musi predelat na TKeyValue
bool Query::whereFloat(const String& column, const float value, const String& oper, const String& table) {
    if (!where.empty()) where += " AND ";

    where += column + "=" + toString(value);

    executed = false;
    return true;
}


// ************************************************************************** //
// Select::Select() { }
Select::Select(const Commons& commons, const String& queryString, PGparam* param)
: Query(commons, queryString, param), limit(0), offset(0) {
    thisClass = "Select";
}

// FIXME: tohle se musi predelat na TKey
bool Select::from(const String& table, const String& column) {
    if (this->table.empty()) this->table = table;

    fromList.insert(std::pair<String, String > (table, column));

    executed = false;
    return true;
}

// FIXME: vyuzit params a zauvozovkovat nazvy
String Select::getQuery() {
    if (fromList.empty()) return queryString; // in case of a direct query

    queryString = "SELECT ";
    String tmpStr = "\n  FROM ";

    // make the SELECT and FROM statement
    for (std::multimap<String, String>::iterator ii = fromList.begin(); ii != fromList.end(); ++ii) {
        String tmpTable = (*ii).first;
        if (tmpTable.find(".") == String::npos) tmpTable = this->getDataset() + "." + tmpTable;

        queryString += tmpTable + "." + (*ii).second;
        if ((*ii).second.compare("*") != 0) queryString += " AS " + (*ii).second + ", ";
        else queryString += ", ";

        tmpStr += tmpTable + ", ";
    }

    // if there are some fields
    if (queryString.length() < 10 || tmpStr.length() < 9)
        error(201, "Select::getQuery():\n" + queryString + "\n" + tmpStr);

    queryString.erase(queryString.length() - 2);
    queryString += tmpStr.erase(tmpStr.length() - 2);

    // FIXME: the rest should be done as above + using params...
    if (!where.empty()) {
        queryString += "\n  WHERE " + where;
    }

    if (!groupby.empty()) {
        queryString += "\n  GROUP BY " + groupby;
    }

    if (!orderby.empty()) {
        queryString += "\n  ORDER BY " + orderby;
    }

    if (limit > 0) {
        queryString += "\n  LIMIT " + toString(limit);
    }

    if (offset > 0) {
        queryString += "\n  OFFSET " + toString(offset);
    }
    queryString += ";";
    return (queryString);
}


// ************************************************************************** //
// Insert::Insert() { }
Insert::Insert(const Commons& commons, const String& queryString, PGparam* param)
: Query(commons, queryString, param) {
    thisClass = "Insert";
}

String Insert::getQuery() {
    if (keys.empty()) return queryString; // in case of a direct query

    // in case we're lazy, we have the table specified in the queryString
    if (table.empty() && queryString.find("INSERT") == String::npos) table = queryString;
    
    // add the dataset selected and escape identifiers
    if (table.find(".") == String::npos) {
        table = String(PQescapeIdentifier(connector->conn, this->dataset.c_str(), this->dataset.length())) +
              "." + String(PQescapeIdentifier(connector->conn, table.c_str(), table.length()));
    }

    queryString = "INSERT INTO " + table + "(";
    String tmpStr;

    // go through keys
    for (int i = 0; i < keys.size(); ++i) {
        queryString += String(PQescapeIdentifier(connector->conn, keys[i].key.c_str(), keys[i].key.length())) + ", ";
        tmpStr += "$" + toString(i+1) + ", ";
    }
    // this is to remove ending separators
    queryString.erase(queryString.length()-2);
    tmpStr.erase(tmpStr.length()-2);

    queryString += ") VALUES(" + tmpStr + ");";

    return queryString;
}


// *****************************************************************************
Update::Update(const Commons& commons, const String& queryString, PGparam* param)
       : Query(commons, queryString, param) {
    thisClass = "Update";

}

String Update::getQuery() {
    if (keys.empty()) return queryString; // in case of a direct query

    // in case we're lazy, we have the table specified in the queryString
    if (table.empty() && queryString.find("UPDATE") == String::npos) table = queryString;

    // add the dataset selected and escape identifiers
    if (table.find(".") == String::npos) {
        table = String(PQescapeIdentifier(connector->conn, this->dataset.c_str(), this->dataset.length())) +
              "." + String(PQescapeIdentifier(connector->conn, table.c_str(), table.length()));
    }

    queryString = "UPDATE " + table + "SET ";

    // go through keys
    for (int i = 0; i < keys.size(); ++i) {
        queryString += String(PQescapeIdentifier(connector->conn, keys[i].key.c_str(), keys[i].key.length()))
                + "=$" + toString(i+1) + ", ";
    }
    // this is to remove ending separators
    queryString.erase(queryString.length()-2);

    // MAYBE a where list
    queryString += " WHERE " + where + ";";

    return queryString;
}