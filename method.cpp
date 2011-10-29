/*
 * File:   method.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:54
 */

#include "VTApi.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace std;



Method::Method(const Dataset& orig) : KeyValues(orig) {

    //    res = PQexecf(getConnector()->getConn(), String("SELECT * FROM public.methods;").c_str());
    //      res = PQexecf(getConnector()->getConnection(), String("SELECT * FROM public.methods_keys WHERE mtname = '" + methodName + "'"));
    methodkeys = new MethodKeys(orig);
}

Method::Method(const Method& orig) : KeyValues(orig) {
}

Method::~Method() {
    delete methodkeys;
}

String Method::getMtname() {
    return this->getString("mtname");
}

void Method::printData(const String& inout) {
    methodkeys->getMethodKeyData(this->getMtname(), inout);

    cout << left;
    cout << setw(20) << "Keyname" << setw(30) << "Typename" << setw(10)<< "In/Out" << endl;
    for (int i = 0; i < methodkeys->getMethodKeyDataSize(); i++) {
        methodkeys->next();
        cout << setw(20) << methodkeys->getKeyname() << setw(30) << methodkeys->getTypname() << setw(10)<< inout << endl;
    }
}

void Method::getInputData() {
    return this->printData("in");
}

void Method::getOutputData() {
    return this->printData("out");
}

