/* 
 * File:   vtapi_timexer.h
 * Author: vojca
 *
 * Created on May 7, 2013, 5:00 PM
 */

#ifndef VTAPI_TIMEXER_H
#define	VTAPI_TIMEXER_H

namespace vtapi {
    class TimExer;
}

#include <time.h>

#include "vtapi_global.h"

namespace vtapi {


/**
 * @brief A general experiment timer++ class...
 * - each time called getTime() returns time used by the program so far (user time + system time)
 * - Plus you can use meantime.
 * - PlusPlus, you can get the PID, virtual and resident memory.
 * - Well, some may use process information /proc/$pid/status instead of libproc_dev.
 */
class TimExer {
private:
    time_t startTime;  // this is the original time
    time_t meanTime;   // this is the time of last getTime() or getMeanTime() call
    clock_t startClock;  // this is the original time
    clock_t meanClock;   // this is the time of last getTime() or getMeanTime() call

public:
    TimExer();
    /**
     * Starts the timer
     */
    void start();
    /**
     * Restarts the timer
     */
    void restart();

    /**
     * @return time [s] passed accordgin to the theory of relativity
     */
    double getTime();

    /**
     * @return meantime [s] from the beginning or the last time or meantime (mezicas)
     */
    double getMeanTime();

    /**
     * @return time [s] used by the program so far (user time + system time)
     */
    double getClock();

    /**
     * @return meantime [s] from the beginning or the last time or meantime (mezicas)
     */
    double getMeanClock();


#ifdef PROCPS_PROC_READPROC_H

    /**
     * @return PID
     */
    int getPID();

    /**
     * @return total [MB] of virtual memory
     */
    double getVirtMemory();

    /**
     * @return total [MB] of (resident) memory
     */
    double getMemory();

#endif

};

} // namespace vtapi

#endif	/* VTAPI_TIMEXER_H */

