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
#include "../common/vtapi_compat.h"
#include "vtapi_processstate.h"
#include "vtapi_processcontrol.h"
#include "vtapi_processparams.h"
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
     * Launches process and waits for its finish (unless async is specified)
     * @param async function returns immediately after launching process
     * @param suspended start process in suspended state (use ProcessControl to unpause it)
     * @param ctrl output pointer to new process control object (client)
     * @return success
     */
    bool run(bool async = false, bool suspended = false, ProcessControl **ctrl = NULL);
    
    /**
     * Constructs a process name from method name and input parameters
     * @param params container
     * @return process name
     */
    std::string constructName(const ProcessParams &params);
    
    //////////////////////////////////////////////////
    // getters - SELECT
    //////////////////////////////////////////////////
    
    /**
     * Gets a process name
     * @return string value with the name of the process
     */
    std::string getName();
    /**
     * Gets detailed process state
     * @return process state object
     */
    ProcessState *getState();
    /**
     * Gets a process name which outputs are inputs for this process
     * @return string value with the input data table name
     * @todo @b doc: Check if it is correct
     */
    std::string getInputProcessName();
    /**
     * Gets a name of a table where output data for this process are stored
     * @return string value with the output data table name
     */
    std::string getOutputTable();
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
     * Gets object containing process parameters
     * @return internals params map
     */
    ProcessParams *getParams();
    
    //////////////////////////////////////////////////
    // adders - INSERT
    //////////////////////////////////////////////////

    /**
     * Adds a new process instance into database, use Method->addProcess() instead
     * @param outputs   output table
     * @return success
     */
    bool add(const std::string& outputs = "");
    /**
     * Sets output data from another process as inputs for this one
     * @param processName   input process name
     * @return success
     */
    bool addInputProcessName(const std::string& processName);
    /**
     * Sets output table for this process
     * @param table   output table name
     * @return success
     */
    bool addOutputTable(const std::string& table);
    /**
     * Inserts full process parameters including input process name
     * Uses move semantics
     * @param params process params
     * @return success
     */
    bool addParams(ProcessParams && params);
    /**
     * Execute INSERT specified by previously called add* methods
     * @return success
     */
    virtual bool addExecute();
    
    //////////////////////////////////////////////////
    // updaters - UPDATE
    //////////////////////////////////////////////////

    /**
     * Sets custom process state
     * @param state process state
     * @param control ProcessControl object through which to send state update notification
     * @return success
     */
    bool updateState(const ProcessState& state, ProcessControl *control = NULL);
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
     * @param control ProcessControl object through which to send state update notification
     * @return success
     */
    bool updateStateFinished(ProcessControl *control = NULL);
    /**
     * Sets process status as ERROR and sets last error message
     * @param errorMsg error message
     * @param control ProcessControl object through which to send state update notification
     * @return 
     */
    bool updateStateError(const std::string& lastError, ProcessControl *control = NULL);
    
    //////////////////////////////////////////////////
    // controls - commands to process instance
    //////////////////////////////////////////////////

    /**
     * Get process control object to initialize server/client notifications
     * between launcher and process
     * @return process control object or NULL on failure
     */
    ProcessControl *getProcessControl();
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

    //////////////////////////////////////////////////
    // filters/utilities
    //////////////////////////////////////////////////
    
    /**
     * Filters iteration via next() for processes with specific input process
     * @param processName   input process name
     */
    void filterByInputProcessName(const std::string& processName);
    /**
     * Filters iteration via next() for processes with specific output table
     * @param table output table
     */
    void filterByOutputTable(const std::string& table);
    /**
     * Deletes output data for this process
     * @return succesful clear
     */
    bool clearOutputData();

protected:
    ProcessParams m_params;     /**< Vector of process parameters */
    std::string m_inputProcess; /**< Input process name*/
    compat::ProcessInstance m_instance;  /**< Newly launched instance via run() */

    virtual bool preUpdate();
};

} // namespace vtapi

#endif	/* VTAPI_PROCESS_H */

