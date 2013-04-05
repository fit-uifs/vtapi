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

    setTable(commons.selection);
    if (!param) this->param = PQparamCreate(connector->getConn());
}

Query::~Query() {
    if (!executed) warning(208, "The query was not executed after the last change\n" + queryString);

    PQclear(res);
    PQparamClear(param);

    this->beDoomed();
}

bool Query::setTable(const String& table) {
    this->table = table;
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

bool Query::keySeqtype(const String& key, const String& value, const String& from) {
    TKey k("seqtype", key, 1, from);
    keys.push_back(k);
    int ret = PQputf(param, "%seqtype", value.c_str());
    if (!ret) std::cerr << PQgeterror();
    executed = false;
    return true;
}

bool Query::keyInouttype(const String& key, const String& value, const String& from) {
    TKey k("inouttype", key, 1, from);
    keys.push_back(k);
    int ret = PQputf(param, "%inouttype", value.c_str());

    if (!ret) std::cerr << PQgeterror();
    executed = false;
    return true;
}

bool Query::keyPermissions(const String& key, const String& value, const String& from) {
    TKey k("permissions", key, 1, from);
    keys.push_back(k);
    int ret = PQputf(param, "%permissions", value.c_str());

    if (!ret) std::cerr << PQgeterror();
    executed = false;
    return true;
}

bool Query::keyTimestamp(const String& key, const time_t& value, const String& from) {
    PGtimestamp timestamp = {0};
    struct tm* ts;

    TKey k("timestamp", key, 1, from);
    keys.push_back(k);

    ts = localtime(&value);
    timestamp.date.isbc = 0;
    timestamp.date.year = ts->tm_year + 1900;
    timestamp.date.mon = ts->tm_mon;
    timestamp.date.mday = ts->tm_mday;
    timestamp.time.hour = ts->tm_hour;
    timestamp.time.min = ts->tm_min;
    timestamp.time.sec = ts->tm_sec;
    timestamp.time.usec = 0;

    int ret = PQputf(param, "%timestamp", &timestamp);

    if (!ret) std::cerr << PQgeterror();
    executed = false;
    return true;
}


String Query::escapeColumn(const String& key, const String& table) {
    executed = false;

    int keyLength = key.length();
    String rest = "";

    // FIXME: in a case, there may be a buffer overflow - do not expose comumn names!
    // FIXME: in a case, this may reorder somethings' meaning!
    if (key.find(':') != String::npos) {
        keyLength = key.find(':');
        rest += key.substr(key.find(':'));
    } else if (key.find('[') != String::npos) {
        keyLength = key.find('[');
        rest += key.substr(key.find('['));
    } else if (key.find('(') != String::npos) {
        keyLength = key.find('(');
        rest += key.substr(key.find('('));
    } // else { // nothing to do

    String ret = "";
    char* c = PQescapeIdentifier(connector->conn, key.c_str(), keyLength);

    if (!table.empty()) {
        char* t = PQescapeIdentifier(connector->conn, table.c_str(), table.length());
        ret += String(t) + ".";
        PQfreemem(t);
    }

    ret += String(c) + rest + " ";
    PQfreemem(c);

    return ret;
}


// FIXME: tohle se musi predelat na TKeyValue
bool Query::whereString(const String& key, const String& value, const String& oper, const String& table) {
    if (value.empty()) return false;

    if (!where.empty()) where += " AND ";
    where += escapeColumn(key, table);

    // FIXME: buffer overflow!! use params!
    if (value.compare("NULL") == 0) {
        where += "IS NULL";
    }
    else if (value.compare("NOT NULL") == 0) {
        where += "IS NOT NULL";
    } else {
        where += oper + " " + String(PQescapeLiteral(connector->conn, value.c_str(), value.length()));
    }

    return true;
}


// FIXME: tohle se musi predelat na TKeyValue
bool Query::whereInt(const String& key, const int value, const String& oper, const String& table) {

    if (!where.empty()) where += " AND ";
    where += escapeColumn(key, table);

    where += oper + " " + toString(value);

    executed = false;
    return true;
}

// FIXME: tohle se musi predelat na TKeyValue
bool Query::whereFloat(const String& key, const float value, const String& oper, const String& table) {

    if (!where.empty()) where += " AND ";
    where += escapeColumn(key, table);

    where += oper + " " + toString(value);

    executed = false;
    return true;
}


// ************************************************************************** //
// Select::Select() { }
Select::Select(const Commons& commons, const String& queryString, PGparam* param)
: Query(commons, queryString, param) {
    thisClass = "Select";

    limit = commons.queryLimit;
    offset = 0;
}

// FIXME: tohle se musi predelat na TKey
bool Select::from(const String& table, const String& column) {
    String t = table;
    if (this->table.empty()) this->table = table;
    else if (table.empty()) t = this->table;
    fromList.insert(std::pair<String, String > (t, column));
    executed = false;
    return true;
}

// TODO: dat do hlavicky
bool Select::function(const String& funtext) {
    // TODO: 
}


// FIXME: vyuzit params (zauvozovkovat nazvy tabulek a datasetu???)
String Select::getQuery() {
    if (fromList.empty()) {
        if (!queryString.empty()) return queryString; // in case of a direct query
        // else add * from this->table
        else if (!table.empty()) {
            this->from(table, "*");
        }
        else warning(2012, "No table specified - don't know how to make a query.");
    }

    queryString = "SELECT ";
    String tmpStr = "";
    
    // this is the previous value ... is it the same in this ordered list???
    std::multimap<String, String>::iterator ilast = fromList.end();

    // make the SELECT and FROM statement
    for (std::multimap<String, String>::iterator ii = fromList.begin(); ii != fromList.end(); ++ii) {
        String tmpTable = (*ii).first;
        if (tmpTable.find(".") == String::npos) tmpTable = this->getDataset() + "." + tmpTable;

        String tmpColumn = (*ii).second;
        if (tmpColumn.empty() || tmpColumn.compare("*") == 0) {
            queryString += tmpTable + ".*, ";
        }
        else {
            char* escapedIdent = PQescapeIdentifier(connector->conn, tmpColumn.c_str(), tmpColumn.length());
            queryString += tmpTable + "." + escapeColumn(tmpColumn);
            queryString += toString("AS ") + escapedIdent + ", ";
            PQfreemem(escapedIdent);
        }

        // tables in the from list should not be specified multiple times -> use views instead!
        if (ilast==fromList.end() || (*ii).first.compare((*ilast).first)!=0) {
            if (!tmpStr.empty()) tmpStr += ", "; // " NATURAL JOIN ";
            tmpStr += tmpTable;
        }

        ilast = ii;
    }
    // TODO: zajistit, aby se daly zpracovat funkce a prazdny from

    // if there are some fields
    if (queryString.length() < 10 || tmpStr.empty())
        error(201, "Select::getQuery():\n" + queryString + "\n" + tmpStr);

    queryString.erase(queryString.length() - 2);
    queryString += "\n  FROM " + tmpStr;

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

bool Select::executeNext() {
    if (limit > 0) {
        if (res) PQclear(res);
        offset += limit;
        return this->execute();
    }
    else return false;
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

    queryString = "UPDATE " + table + " SET ";

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