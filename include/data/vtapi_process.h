/**
 * @file
 * @brief   Declaration of Process class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
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
 * @see Basic definition on page @ref BASICDEFS
 *
 * @note Error codes 36*
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Process : public KeyValues {
public:
    /**
     * Process state
     */
    typedef enum _STATE_T
    {
        STATE_INIT,      /**< process was initialized */
        STATE_STARTED,   /**< process was started */
        STATE_RUNNING,   /**< process is already running */
        STATE_DONE,      /**< process was sucessfuly done */
        STATE_ERROR      /**< process was terminated with an error */
    } STATE_T;

    /**
     * @todo @b doc: put together a few letters..
     */
    typedef void (*fCallback)(STATE_T state, Process *process, void *context);

public:

    /**
     * Constructor for processes
     * @param orig   pointer to the parrent KeyValues object
     * @param name   specific name of process, which we can construct
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
     * Gets a process name
     * @return string value with the name of the process
     */
    std::string getName();
    /**
     * Gets current process state
     * @return state
     */
    STATE_T getState();
    /**
     * Gets a process name which outputs are inputs for this process
     * @return string value with the input data table name
     * @todo @b doc: Check if it is correct
     */
    std::string getInputs();
    /**
     * Gets a name of a table where output data for this process are stored
     * @return string value with the output data table name
     */
    std::string getOutputs();
    /**
     * Gets a process which outputs are inputs for this process
     * @return process object
     */
    Process *getInputProcess();
    /**
     * Gets input intervals of this process
     * @return input intervals
     */
    Interval *getInputData();
    /**
     * Gets output intervals of this process
     * @return output intervals
     */
    Interval *getOutputData();
    /**
     * Deletes output data for this process
     */
    void deleteOutputData();
    /**
     * Gets a numeric parameter of this process
     * @param key   name of parameter
     * @return value of parameter
     */
    int getParamInt(const std::string& key);
    /**
     * Gets a floating point numeric parameter of this process
     * @param key   name of parameter
     * @return value of parameter
     */
    double getParamDouble(const std::string& key);
    /**
     * Gets a string parameter of this process
     * @param key   name of parameter
     * @return value of parameter
     */
    std::string getParamString(const std::string& key);

    /**
     * Sets output data from another process as inputs for this one
     * @param processName   input process name
     */
    void setInputs(const std::string& processName);
    /**
     * Sets output table for this process
     * @param table   output table name
     */
    void setOutputs(const std::string& table);
    /**
     * Sets an integer parameter of this process
     * @param key     name of parameter
     * @param value   value of parameter
     */
    void setParamInt(const std::string& key, int value);
    /**
     * Sets a floating point numeric parameter of this process
     * @param key     name of parameter
     * @param value   value of parameter
     */
    void setParamDouble(const std::string& key, double value);
    /**
     * Sets a string parameter of this process
     * @param key     name of parameter
     * @param value   value of parameter
     */
    void setParamString(const std::string& key, const std::string& value);
    /**
     * Sets a process status update callback
     * @param callback   callback function
     * @param pContext   context supplied to callbacks
     */
    void setCallback(fCallback callback, void *pContext);
    
    /**
     * Represents processes with specific input process only. Use this before calling next()
     * @param processName   input process name
     */
    void filterByInputs(const std::string& processName);
    
    virtual bool preSet();

    /**
     * Adds a new process instance into database, use Method->addProcess() instead
     * @param outputs   output table
     * @return success
     */
    bool add(const std::string& outputs="");

    /**
     * Creates a new interval for process
     * @param t1   currently unused
     * @param t2   currently unused
     * @return new interval
     * @todo @b code: parameters t1 and t2 are unused!
     */
    Interval* newInterval(const int t1 = -1, const int t2 = -1);
    /**
     * Creates a new sequence for process
     * @param name   specific sequence name
     * @return new sequence
     * @unimplemented
     */
    Sequence* newSequence(const std::string& name = "");
    
    /**
     * Prepares an output selection table (checks if exist required attributes and if is necessary they would be added to the table)
     * @param method      input method which is a core of the process
     * @param selection   selection table for outputs
     * @return success
     */
    bool prepareOutput(const std::string& method, const std::string& selection="intervals");
    /**
     * Performs a diff of columns: \<required method output columns\> - \<existing columns in output selection table\>
     * @param table     selection table for outputs
     * @param columns   all method attributes
     * @return vector of columns to be added to the output selection table
     */
    std::map<std::string,std::string> diffColumns(const std::string& table, const TKeys& columns);
    /**
     * Ensures an addition of required columns to an output selection table
     * @param table     selection table for outputs
     * @param columns   vector of columns to be added
     * @return success
     */
    bool addColumns(const std::string& table, const std::map<std::string,std::string>& columns);

    // http://stackoverflow.com/questions/205529/c-c-passing-variable-number-of-arguments-around
    /**
     * Runs process
     * @return success
     */
    bool run();

protected:
    std::string inputs;   /**< A process name which outputs are inputs for this process */
    TKeyValues params;    /**< Vector of process parameters */
    
    fCallback callback;       /**< @todo @b doc: put together a few letters.. */
    void *pCallbackContext;   /**< @todo @b doc: put together a few letters.. */
    
protected:
    /**
     * Constructs a process name (composed of method name and input parameters)
     * @return process name
     * @todo @b doc: Check if it is correct & exhaustive
     */
    std::string constructName();
    /**
     * Performs a serialization of parameters
     * @return serialized parameters
     * @todo @b doc: Check if it is correct & exhaustive
     */
    std::string serializeParams();
    /**
     * Performs a deserialization of parameters
     * @param paramString serialized parameters
     * @todo @b doc: Check if it is correct & exhaustive
     */
    void deserializeParams(std::string paramString);

};

} // namespace vtapi

#endif	/* VTAPI_PROCESS_H */

