/*
 * File:   select.cpp
 * Author: chmelarp
 *
 * Created on 18. October 2011, 9:50
 */

#include "VTApi.h"
#include <iostream>

// Select::Select() { }
Select::Select(const Commons& commons, const String& queryString, PGparam* param)
       : Commons(commons), res(NULL), limit(0), offset(0), param(NULL) {
}

Select::~Select() {
    PQclear(res);
}


bool Select::from(const String& table, const String& column) {
    fromList.insert( std::pair<String, String>(table, column) );
    return true;
}

// FIXME: tohle se musi predelat jakoze struktura, ktera vyuziva fromList
bool Select::whereString(const String& column, const String& value, const String& table) {
    if (value.empty()) return false;

    if (value.compare("NULL") == 0) {
        where += column + " IS NULL ";
    }
    else {
        // FIXME: buffer overflow!! use params!
        where += column + " = \'" + value + "\' AND ";
    }
    return true;
}

// FIXME: vyuzit params a zauvozovkovat nazvy
String Select::getQuery() {
    if (!queryString.empty()) return queryString; // in case of a direct query

   String query = "SELECT ";
   String tmpStr = "\n  FROM ";

   // make the SELECT and FROM statement
   for (std::multimap<String, String>::iterator ii=fromList.begin(); ii!=fromList.end(); ++ii)
   {
       // FIXME: kde se to proboha otocilo???
       String tmpTable = (*ii).first;
       if (tmpTable.find(".") == String::npos) tmpTable = this->getDataset() + "." + tmpTable;

       query += tmpTable + "." + (*ii).second;
       if ((*ii).second.compare("*") != 0) query += " AS " + (*ii).second + ", ";
       else query += ", ";

       tmpStr += tmpTable + ", ";
   }

   // if there are some fields
   if (query.length() < 10 || tmpStr.length() < 9)
       error(201, "Select::getQuery():\n" + query + "\n" + tmpStr);

   query.erase(query.length()-2);
   query += tmpStr.erase(tmpStr.length()-2);

   // FIXME: the rest should be done as above + using params...
   if (!where.empty()) {
       tmpStr = "\n  WHERE " + where;
       query += tmpStr.erase(tmpStr.length()-5);
   }

   if (!groupby.empty()) {
       query += "\n  GROUP BY " + groupby;
   }

   if (!orderby.empty()) {
       query += "\n  ORDER BY " + orderby;
   }

   // TODO Tomas: LIMIT and OFFSET

   query += ";";
   return (query);
}

// TODO: zvazit pouziti statement
bool Select::execute() {
    String query = this->getQuery();
    
    if (verbose) logger->debug(query);
    res = PQparamExec(connector->getConn(), param, query.c_str(), 1);
    // TODO: use params in the getQuery next time

    if (!res) error(200, "Select::execute():\n" + String(PQgeterror()));
    return (res);
}

