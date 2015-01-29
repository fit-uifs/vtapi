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
    typedef enum _STATE_T
    {
        STATE_INIT,
        STATE_STARTED,
        STATE_RUNNING,
        STATE_DONE,
        STATE_ERROR
    } STATE_T;
    
public:

    /**
     * Constructor for processes
     * @param orig pointer to the parrent KeyValues object
     * @param name specific name of process, which we can construct
     */
    Process(const KeyValues& orig, const std::string& name = "");

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
    std::string getName();
    /**
     * Get a name of a table where are stored an input data
     * @return string value with an input data table name
     */
    std::string getInputs();
    /**
     * Get a name of a table where are stored an output data
     * @return string value with an output data table name
     */
    std::string getOutputs();

    /**
     * A dangerous and rather discouraged function...
     * @deprecated by the human power
     * @param method
     * @param name
     * @param selection
     * @return
     * @todo @b doc: má to cenu komentovat? :) Případně doplnit
     */
    bool add(const std::string& method, const std::string& name, const std::string& selection="intervals");

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
    Sequence* newSequence(const std::string& name = "");
    
    /**
     * Prepare output selection table (checks if exist required attributes and if necessary they would be added to the table)
     * @param method   input method which is a core of the process
     * @param selection   selection table for outputs
     * @return success
     */
    bool prepareOutput(const std::string& method, const std::string& selection="intervals");
    /**
     * Diff columns: <required method output columns> - <existing columns in output selection table>
     * @param table   selection table for outputs
     * @param columns   all method attributes
     * @return   vector of columns to be added to the output selection table
     */
    std::map<std::string,std::string> diffColumns(const std::string& table, const TKeys& columns);
    /**
     * Ensure an addition of required columns to the output selection table
     * @param table   selection table for outputs
     * @param columns   vector of columns to be added
     * @return   success
     */
    bool addColumns(const std::string& table, const std::map<std::string,std::string>& columns);

    // http://stackoverflow.com/questions/205529/c-c-passing-variable-number-of-arguments-around
    /**
     * Runs the Method's derivate run() with default parameters Method->run(*this);
     * @return success
     * @todo @b code: neimplementováno
     */
    bool run();
    bool run(...);


    std::string getID() { return std::string(""); }
    std::string getErrorMessage() { return std::string(""); }
    Interval *newResults() { return NULL; }

};

} // namespace vtapi

#endif	/* VTAPI_PROCESS_H */

