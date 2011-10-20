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



Test::Test(Dataset& orig) {
    dataset = &orig;
    sequence = dataset->newSequence();
    interval = sequence->newInterval();
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

void Test::testSequence() {
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
    cout << "========== INTERVAL ==========" << endl;
    cout << left;
    cout << setw(20) << "Sequence" << setw(8) << "Start" << setw(8) << "End" <<
        setw(20) << "Location" << setw(10) << "Tags" << endl;
    while (interval->next()) {
        vector<int> tags;
        std::stringstream tagStr;
        tags = interval->getTags();
        cout << setw(20) << interval->getSequence() << setw(8) << interval->getStartTime() <<
                setw(8) << interval->getEndTime() << setw(20) << interval->getLocation() << setw(10);
        for (int i = 0; i < tags.size(); i++) {
            tagStr << tags.at(i);
            if (i < tags.size() - 1) tagStr << ",";
            else cout << tagStr.str() << endl;
        }

    }
    cout << endl;
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
    this->testSequence();
    this->testInterval();
    this->testMethod();
    this->testProcess();
}
