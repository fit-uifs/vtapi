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
    else executed = true;

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

bool Query::keyInt(const String& key, const int& value, const String& from) {
    TKey k("int4", key, 1, from);
    keys.push_back(k);
    PQputf(param, "%int4", value);

    executed = false;
    return true;
}

bool Query::keyIntA(const String& key, const int* values, const int size, const String& from) {
    TKey k("int4[]", key, size, from);
    keys.push_back(k);
    PQputf(param, "%int4[]", values);

    executed = false;
    return true;
}

bool Query::keyFloat(const String& key, const float& value, const String& from) {
    TKey k("float4", key, 1, from);
    keys.push_back(k);
    PQputf(param, "%float4", value);

    executed = false;
    return true;
}

bool Query::keyFloatA(const String& key, const float* values, const int size, const String& from) {
    TKey k("float4[]", key, size, from);
    keys.push_back(k);
    PQputf(param, "%float4[]", values);

    executed = false;
    return true;
}




// ************************************************************************** //
// Select::Select() { }
Select::Select(const Commons& commons, const String& queryString, PGparam* param)
: Query(commons, queryString, param), limit(0), offset(0) {
}

// FIXME: tohle se musi predelat na TKey

bool Select::from(const String& table, const String& column) {
    fromList.insert(std::pair<String, String > (table, column));

    executed = false;
    return true;
}

// FIXME: tohle se musi predelat na TKeyValue
bool Select::whereString(const String& column, const String& value, const String& table) {
    if (value.empty()) return false;

    if (value.compare("NULL") == 0) {
        where += column + " IS NULL ";
    } else {
        // FIXME: buffer overflow!! use params!
        where += column + " = " + String(PQescapeLiteral(connector->conn, value.c_str(), value.length())) + " AND ";
    }

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
        tmpStr = "\n  WHERE " + where;
        queryString += tmpStr.erase(tmpStr.length() - 5);
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
}

bool Insert::into(const String& table) {
    intoTable = table; // we can handle only a single table/view at the moment
    // TODO: check if table exists to return true???

    executed = false;
    return true;
}


String Insert::getQuery() {
    if (keys.empty()) return queryString; // in case of a direct query

    // in case we're lazy, we have the table specified in the queryString
    if (intoTable.empty() && queryString.find("INSERT") == String::npos) intoTable = queryString;
    
    // add the dataset selected and escape identifiers
    if (intoTable.find(".") == String::npos) {
        intoTable = String(PQescapeIdentifier(connector->conn, this->dataset.c_str(), this->dataset.length())) +
              "." + String(PQescapeIdentifier(connector->conn, intoTable.c_str(), intoTable.length()));
    }

    queryString = "INSERT INTO " + intoTable + "(";
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

