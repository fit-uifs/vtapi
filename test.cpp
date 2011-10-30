 /*
 * File:   test.cpp
 * Author: chmelarp
 *
 * Created on 29. září 2011, 10:54
 */

#include "vtapi.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace std;


// TODO: toto neni dobre... melo to byt ve forme how-to + testovat vsechny featury,
// ne jen tady tech par na vypis vsech hodnot

Test::Test(Dataset& orig) {
    dataset = &orig;
/** TODO: tohle je totalni blbost...
    sequence = dataset->newSequence();
    interval = sequence->newInterval();
*/
}

Test::~Test() {
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
    cout << "========== DATASET ==========" << endl;
    dataset->next();
    dataset->print(dataset->select->res);

    cout << "Using dataset " << dataset->getName() << endl;

    Sequence* sequence = dataset->newSequence();
    sequence->next();
    sequence->print(sequence->select->res);

    this->testMethod();
    this->testProcess();
}
