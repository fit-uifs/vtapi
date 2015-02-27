/**
 * @file
 * @brief   Declaration of TimExer class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref Licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef VTAPI_TIMEXER_H
#define	VTAPI_TIMEXER_H


namespace vtapi {


/**
 * @brief A general experiment timer++ class...
 * - each time called getTime() returns time used by the program so far (user time + system time)
 * - Plus you can use meantime.
 * - PlusPlus, you can get the PID, virtual and resident memory.
 * - Well, some may use process information /proc/$pid/status instead of libproc_dev.
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref Licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class TimExer {
private:
    time_t startTime;       /**< original time */
    time_t meanTime;        /**< time of last getTime() or getMeanTime() call */
    clock_t startClock;     /**< original time */
    clock_t meanClock;      /**< time of last getTime() or getMeanTime() call */

public:
    /**
     * Constructor
     */
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
     * Getx time in seconds based on current time
     * @return time [s]
     */
    double getTime();

    /**
     * Gets meantime in seconds based on current time
     * @return meantime [s] from the beginning or the last time or meantime (mezicas)
     * @todo Not so good keyword for method name, because in czech it means "průměrný čas" 
     */
    double getMeanTime();

    /**
     * Gets time in seconds based on program time (user time + system time)
     * @return time [s] used by the program so far (user time + system time)
     */
    double getClock();

    /**
     * Gets meantime in seconds based on program time (user time + system time)
     * @return meantime [s] from the beginning or the last time or meantime (mezicas)
     * @todo Not so good keyword for method name, because in czech it means "průměrný čas" 
     */
    double getMeanClock();


#if HAVE_READPROC

    /**
     * Gets PID of process 
     * @return PID
     */
    int getPID();

    /**
     * Gets total used virtual memory [in MB]
     * @return total [MB] of virtual memory
     */
    double getVirtMemory();

    /**
     * Gets total user (resident) memory [in MB]
     * @return total [MB] of (resident) memory
     */
    double getMemory();

#endif

};

} // namespace vtapi

#endif	/* VTAPI_TIMEXER_H */

