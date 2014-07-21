/* 
 * File:   vtapi_process.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:58 PM
 */

#ifndef VTAPI_PROCESS_H
#define	VTAPI_PROCESS_H

#include "vtapi_keyvalues.h"

namespace vtapi {

class Interval;
class Sequence;

/**
 * @brief A class which represents processes and gets information about them
 *
 * @see Basic definition on page @ref LOGICAL
 *
 * @note Error codes 36*
 */
class Process : public KeyValues {
public:

    /**
     * Constructor for processes
     * @param orig pointer to the parrent KeyValues object
     * @param name specific name of process, which we can construct
     */
    Process(const KeyValues& orig, const string& name = "");

    /**
     * Individual next() for processes, which stores current process
     * and selection to commons
     * @note Overloading next() from KeyValues
     * @return success
     */
    bool next();


    /**
     * Get a process name
     * @return string value with a process name
     */
    string getName();
    /**
     * Get a name of a table where are stored an input data
     * @return string value with an input data table name
     */
    string getInputs();
    /**
     * Get a name of a table where are stored an output data
     * @return string value with an output data table name
     */
    string getOutputs();

    /**
     * A dangerous and rather discouraged function...
     * @deprecated by the human power
     * @param method
     * @param name
     * @param selection
     * @return
     * @todo @b doc: má to cenu komentovat? :) Případně doplnit
     */
    bool add(const string& method, const string& name, const string& selection="intervals");

    /**
     * Create new interval for process
     * @param t1 currently unused
     * @param t2 currently unused
     * @return new interval
     * @todo @b code: Nepoužívané parametry t1, t2
     */
    Interval* newInterval(const int t1 = -1, const int t2 = -1);
    /**
     * Create new sequence for process
     * @param name specific sequence name
     * @return new sequence
     * @todo @b code: neimplementováno (zkontrolovat pak i doc)
     */
    Sequence* newSequence(const string& name = "");

    // http://stackoverflow.com/questions/205529/c-c-passing-variable-number-of-arguments-around
    /**
     * Runs the Method's derivate run() with default parameters Method->run(*this);
     * @return success
     * @todo @b code: neimplementováno
     */
    bool run();
    bool run(...);



};

} // namespace vtapi

#endif	/* VTAPI_PROCESS_H */

