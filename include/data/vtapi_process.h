/**
 * @file
 * @brief   Declaration of Process class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once
 
#include <string>
#include <list>
#include "vtapi_keyvalues.h"
#include "vtapi_sequence.h"
#include "vtapi_task.h"
#include "vtapi_processstate.h"
#include "vtapi_processcontrol.h"
#include "../common/vtapi_compat.h"

namespace vtapi {

/**
 * @brief A class which represents processes and gets information about them
 * 
 * @see Basic definition on page @ref BASICDEFS
 *
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
     * Construct process object for iterating through VTApi processes
     * If a specific name is set, object will represent one process only
     * @param commons base Commons object
     * @param id process id, 0 for all processes
     */
    Process(const Commons& commons, int id = 0);

    /**
     * Construct process object for iterating through VTApi processes
     * Object will represent set of processes specified by their names
     * @param commons base Commons object
     * @param names list of method processes
     */
    Process(const Commons& commons, const std::list<int>& ids);

    /**
     * Destructor
     */
    virtual ~Process();
    
    /**
     * Individual next() for processes, updates process and selection
     * @return success
     * @note Overloading next() from KeyValues
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
    
    //////////////////////////////////////////////////
    // getters - SELECT
    //////////////////////////////////////////////////
    
    /**
     * Gets a process ID
     * @return process ID
     */
    int getId();
    /**
     * Gets detailed process state
     * @return process state object
     */
    ProcessState *getState();
    
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
     * Filters iteration via next() by task
     */
    void filterByTask(const std::string& taskname);

protected:
    compat::ProcessInstance _instance;  /**< Newly launched instance via run() */

    virtual bool preUpdate();

private:
    Process() = delete;
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;
};

} // namespace vtapi
