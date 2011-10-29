/*
 * File:   methodkeys.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:54
 */


// TODO Tomas: odstranit tento soubor, konzultace nutna
#include "VTApi.h"

MethodKeys::MethodKeys(const Dataset& orig) : KeyValues(orig) {
}

MethodKeys::MethodKeys(const Dataset& orig, const String& methodName, const String& inout) : KeyValues(orig) {
      this->getMethodKeyData(methodName, inout);
}

MethodKeys::MethodKeys(const MethodKeys& orig) : KeyValues(orig) {
}

MethodKeys::~MethodKeys() {
}
/*select MK.keyname, PT.*
from public.methods_keys MK
join pg_catalog.pg_type PT ON MK.typname::oid = PT.oid*/
void MethodKeys::getMethodKeyData(const String& methodName, const String& inout) {
/*
    res = PQexecf(getConnector()->getConn(), String(" SELECT MK.keyname, PT.typname "
                                                            " FROM public.methods_keys MK "
                                                            " JOIN pg_catalog.pg_type PT ON MK.typname::regtype = PT.oid "
                                                            " WHERE mtname = '" + methodName + "' "
                                                            " AND inout = '" + inout + "' "
                                                           ).c_str());
// FIXME:      this->rewind();
 */
}

int MethodKeys::getMethodKeyDataSize() {
    return PQntuples(select->res);
}

String MethodKeys::getKeyname() {
    return this->getString("keyname");
}

String MethodKeys::getTypname() {

    return this->getString("typname");
}