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
 
#include <list>
#include "keyvalues.h"
#include "dataset.h"
#include "sequence.h"
#include "task.h"
#include "method.h"
#include "processstate.h"

namespace vtapi {

class Dataset;
class Sequence;
class ImageFolder;
class Video;
class Task;
class Method;


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
class Process : protected KeyValues
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
     * @brief Checks if process can be instance and calls next()
     * Call this before first next()
     * @return succesful intantiations
     */
    bool instantiateSelf();

    /**
     * Launches process instance
     * @param suspended start process in suspended state (use InterProc to unpause it)
     * @return success
     */
    bool launch(bool suspended = false);

    //////////////////////////////////////////////////
    // getters - associated objects
    //////////////////////////////////////////////////

    /**
     * @brief Gets parent dataset object
     * @return dataset object (initialized)
     */
    Dataset *getParentDataset();

    /**
     * @brief Gets parent task object
     * @return task object (initialized)
     */
    Task *getParentTask();

    /**
     * @brief Gets parent method object
     * @return method object (initialized)
     */
    Method *getParentMethod();

    /**
     * @brief Loads image folders which should be processed
     * @return image folders object for iteration
     */
    ImageFolder *loadAssignedImageFolders();

    /**
     * @brief Loads videos which should be processed
     * @return videos object for iteration
     */
    Video *loadAssignedVideos();

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

    /**
     * @brief Gets IPC port of running instance
     * @return IPC port
     */
    int getIpcPort();
    
    //////////////////////////////////////////////////
    // updaters - UPDATE
    //////////////////////////////////////////////////

    /**
     * Sets custom process state
     * @param state process state
     * @return success
     */
    bool updateState(const ProcessState& state);
    /**
     * Sets process status as RUNNING
     * @param progress percentage progress [0-100]
     * @param currentItem currently processed item
     * @return success
     */
    bool updateStateRunning(float progress, const std::string& currentItem);
    /**
     * Sets process status as SUSPENDED (paused)
     * @return success
     */
    bool updateStateSuspended();
    /**
     * Sets process status as FINISHED (without error)
     * @return success
     */
    bool updateStateFinished();
    /**
     * Sets process status as ERROR and sets last error message
     * @param errorMsg error message
     * @return success
     */
    bool updateStateError(const std::string& lastError);
    /**
     * @brief Sets new port for IPC
     * @param port port
     * @return success
     */
    bool updateIpcPort(int port);

    //////////////////////////////////////////////////
    // filters/utilities
    //////////////////////////////////////////////////

    /**
     * Filters iteration via next() by task
     */
    void filterByTask(const std::string& taskname);

protected:
    virtual bool preUpdate();

private:
    Process() = delete;
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;
};

} // namespace vtapi
