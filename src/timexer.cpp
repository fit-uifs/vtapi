/**
 * @file    timexer.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @section DESCRIPTION
 *
 * Methods of TimExer class
 */

#include <vtapi_global.h>
#include <common/vtapi_timexer.h>

using namespace vtapi;


TimExer::TimExer() {
    start();
}

void TimExer::start() {
    startTime = time(NULL); // TODO: use rather gettimeofday?
    meanTime = startTime;
    startClock = clock();
    meanClock = startTime;
}

void TimExer::restart() {
    this->start();
}

double TimExer::getTime() {
    meanTime = time(NULL);
    return (double)  difftime(meanTime, startTime);
}

double TimExer::getMeanTime() {
    time_t newMeanTime = time(NULL);
    double ret = (double) difftime(newMeanTime, meanTime);
    meanTime = newMeanTime;
    return ret;
}

double TimExer::getClock() {
    meanClock = clock();
    return ((double) meanClock - startClock) / ((double) CLOCKS_PER_SEC);
}

double TimExer::getMeanClock() {
    clock_t newMeanClock = clock();
    double ret = ((double) newMeanClock - meanClock) / ((double) CLOCKS_PER_SEC);
    meanTime = newMeanClock;
    return ret;
}

// have libproc-dev?
#ifdef PROCPS_PROC_READPROC_H

int TimExer::getPID() {
    struct proc_t usage;
    look_up_our_self(&usage);
    return usage.ppid;
}

double TimExer::getVirtMemory() {
    struct proc_t usage;
    look_up_our_self(&usage);
    // this is 1024*1024 (however, should be in pages :)
    return (double) usage.vsize / 1048576;
}

double TimExer::getMemory() {
    struct proc_t usage;
    look_up_our_self(&usage);
    // this is rss * 4096 / 1024*1024 (pages of 4KB ... $ getconf PAGESIZE)
    return (double) usage.rss / 256;
}

#endif
