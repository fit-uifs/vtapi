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
#include "vtapi_processstate.h"
#include "vtapi_processcontrol.h"
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
class Process : public KeyValues
{
public:

    /**
     * Constructor for processes
     * @param orig   pointer to the parent KeyValues object
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
     * Constructs a process name from method name and input parameters
     * @return process name
     */
    std::string constructName();
    
    /**
     * Gets detailed process state
     * @return process state object
     */
    ProcessState *getState();
    /**
     * Sets process status as RUNNING
     * @param progress percentage progress [0-100]
     * @param currentItem currently processed item
     * @param control ProcessControl object through which to send state update notification
     * @return success
     */
    bool updateStateRunning(float progress, const std::string& currentItem, ProcessControl *control = NULL);
    /**
     * Sets process status as SUSPENDED (paused)
     * @param control ProcessControl object through which to send state update notification
     * @return success
     */
    bool updateStateSuspended(ProcessControl *control = NULL);
    /**
     * Sets process status as FINISHED (without error)
     * @param progress final percentage progress [0-100]
     * @param control ProcessControl object through which to send state update notification
     * @return success
     */
    bool updateStateFinished(float progress, ProcessControl *control = NULL);
    /**
     * Sets process status as ERROR and sets last error message
     * @param errorMsg error message
     * @param control ProcessControl object through which to send state update notification
     * @return 
     */
    bool updateStateError(const std::string& lastError, ProcessControl *control = NULL);
    
    /**
     * Sends control message to process: resume.
     * @param control ProcessControl object through which to send command
     * @return success
     */
    bool controlResume(ProcessControl *control);
    /**
     * Sends control message to process: suspend.
     * @param control ProcessControl object through which to send command
     * @return success
     */
    bool controlSuspend(ProcessControl *control);
    /**
     * Sends control message to process: stop.
     * @param control ProcessControl object through which to send command
     * @return success
     */
    bool controlStop(ProcessControl *control);
    
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
     * Represents processes with specific input process only. Use this before calling next()
     * @param processName   input process name
     */
    void filterByInputs(const std::string& processName);
    
    /**
     * Pre-update default setting
     * @return success
     */
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
     * Launches process and waits for its finish (unless async is specified)
     * @param async function returns immediately after launching process
     * @param suspended start process in suspended state (use ProcessControl to unpause it)
     * @return success
     */
    bool run(bool async = false, bool suspended = false);

    /**
     * Get process control object to initialize server/client notifications
     * between launcher and process
     * @return process control object or NULL on failure
     */
    ProcessControl *getProcessControl();

protected:
    std::string inputs;     /**< A process name which outputs are inputs for this process */
    TKeyValues params;      /**< Vector of process parameters */
    
protected:

    /**
     * Performs a serialization of parameters
     * @return serialized parameters
     */
    std::string serializeParams();
    /**
     * Performs a deserialization of parameters into params memeber
     * @param paramString serialized parameters
     * @return count of deserialized params, -1 on format error
     */
    int deserializeParams(const std::string& paramString);
    /**
     * Deallocates parameters
     */
    void destroyParams();
};

} // namespace vtapi

#endif	/* VTAPI_PROCESS_H */

