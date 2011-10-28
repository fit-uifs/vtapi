/*
 * File:   select.cpp
 * Author: chmelarp
 *
 * Created on 18. October 2011, 9:50
 */

#include "VTApi.h"
#include <iostream>

// Select::Select() { }
Select::Select(const Commons& commons, QueryType type)
       : Commons(commons), res(NULL), pos(-1), type(type) {
}

bool Select::from(const String& table, const String& column) {
    // pokud je nalezena . znamena to, ze uz je tam dataset... 
    // jinak ho tam pridam, ale az pri sestaveni dotazu radeji
    // String dsTable = table;
    // if (dsTable.find(".") == String.npos) dsTable = this->dataset + "." + table;
        
    this->fromList.insert( std::pair<String, String>(table, column) );
    return true;
}

bool Select::whereString(const String& table, const String& column, const String& value) {
    if (value.compare("NULL") == 0) {
        this->where += column + " IS NULL ";
    }
    else {
        this->where += column + " = " + value + " ";
    }
    return true;
}

String Select::getQuery() {
   this->query = "SELECT ";

   String tmp = "\nFROM ";
   for (std::multimap<String, String>::iterator ii=fromList.begin(); ii!=fromList.end(); ++ii)
   {
       this->query += (*ii).first + ", ";
       tmp += (*ii).second + ", ";
       std::cout << (*ii).first << ": " << (*ii).second << std::endl;
   }

   // TODO: poresit separatory
   this->query += tmp;

   return (this->query);

}