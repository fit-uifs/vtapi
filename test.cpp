 /*
 * File:   test.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:54
 */

#include "VTApi.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace std;



Test::Test(const Dataset& orig) {
    *dataset = orig;
}

Test::~Test() {
}

void Test::testDataset() {
    cout << "========== DATASET ==========" << endl;
    cout << left;
    cout << setw(20) << "Name" << setw(30) << "Location" << endl;
    while (dataset->next()) {
        cout << setw(20) << dataset->getName() << setw(30) << dataset->getLocation() << endl;
    }
    cout << endl;
}

void Test::testSequece() {
    Sequence* sequence = dataset->newSequence();

    cout << "========== SEQUENCE ==========" << endl;
    cout << left;
    cout << setw(20) << "Name" << setw(30) << "Location" << endl;
    while (sequence->next()) {
        cout << setw(20) << sequence->getName();
        cout << setw(30) << sequence->getLocation() << endl;
    }
    cout << endl;
}

void Test::testInterval() {
/*    cout << "========== SEQUENCE ==========" << endl;
    cout << left;
    cout << setw(20) << "Name" << setw(30) << "Location" << endl;
    while (dataset->next()) {
        cout << setw(20) << dataset->getName() << setw(30) << dataset->getLocation() << endl;
    }
    cout << endl;*/
}

void Test::testKeyValues() {

}

void Test::testMethod() {
    Method* method = new Method(*dataset);
    
    cout << "========== METHOD ==========" << endl;
    while (method->next()) {
        cout << "===> Method \"" << method->getMtname() << "\":" << endl;
        method->getInputData();
        method->getOutputData();
        cout << endl;
    }
}

void Test::testProcess() {
    Process* process = new Process(*dataset);

    cout << "========== PROCESS ==========" << endl;
    process->printProcesses();
    cout << endl;
}

void Test::testAll() {
    this->testDataset();
    this->testSequece();
    this->testMethod();
    this->testProcess();
}
