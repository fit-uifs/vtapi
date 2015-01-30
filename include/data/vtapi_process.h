/* 
 * File:   vtapi_process.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:58 PM
 */

#ifndef VTAPI_PROCESS_H
#define	VTAPI_PROCESS_H

#include "vtapi_keyvalues.h"
#include "vtapi_sequence.h"
#include "vtapi_interval.h"
#include "../common/vtapi_tkeyvalue.h"

namespace vtapi {

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
    
    typedef void (*fCallback)(STATE_T state, Process *process, void *context);

public:

    /**
     * Constructor for processes
     * @param orig pointer to the parrent KeyValues object
     * @param process only for specific method
     * @param name specific name of process, which we can construct
     */
    Process(const KeyValues& orig, const std::string& name = "");

    virtual ~Process();
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
     * Get current process state
     * @return state
     */
    STATE_T getState();
    /**
     * Get process name for input data for this process
     * @return string value with an input data table name
     */
    std::string getInputs();
    /**
     * Get a name of a table where are stored an output data
     * @return string value with an output data table name
     */
    std::string getOutputs();
    /**
     * Get process which output are input for this process
     * @return process object
     */
    Process *getInputProcess();
    /**
     * Get output intervals of this process
     * @return output intervals
     */
    Interval *getOutputData();
    /**
     * Get numeric process param
     * @param key param name
     * @return param value
     */
    int getParamInt(const std::string& key);
    /**
     * Get string process param
     * @param key param name
     * @return param value
     */
    std::string getParamString(const std::string& key);

    /**
     * Sets output data from another process as inputs for this one
     * @param processName input process name
     */
    void setInputs(const std::string& processName);
    /**
     * Sets integer argument
     * @param key arg name
     * @param value arg value
     */
    void setParamInt(const std::string& key, int value);
    /**
     * Sets string process argument
     * @param key arg name
     * @param value arg value
     */
    void setParamString(const std::string& key, const std::string& value);
    /**
     * Sets process status update callback
     * @param callback callback function
     * @param pContext context supplied to callbacks
     */
    void setCallback(fCallback callback, void *pContext);
    
    virtual bool preSet();

    /**
     * Add new process instance into database, use Method->addProcess() instead
     * @param method method name
     * @param name new process name
     * @param params serialized process params
     * @param outputs output table
     * @return
     */
    bool add(const std::string& method, const std::string& name, const std::string& params = "", const std::string& outputs="intervals");

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
     * Runs process
     * @return success
     */
    bool run();

protected:
    TKeyValues params;
    bool bParamsDirty;
    
    fCallback callback;
    void *pCallbackContext;
    
protected:
    std::string constructName();
    std::string serializeParams();
    void deserializeParams(std::string paramString);

};

} // namespace vtapi

#endif	/* VTAPI_PROCESS_H */

