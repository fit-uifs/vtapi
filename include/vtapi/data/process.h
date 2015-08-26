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
#include "../common/interproc.h"


namespace vtapi {

class Dataset;
class Sequence;
class ImageFolder;
class Video;
class Task;
class Method;
class InterProcessServer;
class InterProcessClient;


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
     * @brief Copy constructor
     * @param copy original object
     */
    Process(const Process& copy);

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
    
    using KeyValues::count;
    using KeyValues::print;
    using KeyValues::printAll;
    using KeyValues::printKeys;

    /**
     * Individual next() for processes, updates process and selection
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next() override;

    //////////////////////////////////////////////////
    // process instance control
    //////////////////////////////////////////////////

    /**
     * @brief Initializes current process as this process's instance
     * Call this before first next()
     * @param callback callback on received command
     * @return object for registering interprocess communication with clients, NULL on error
     */
    InterProcessServer * initializeInstance(InterProcessServer::fnCommandCallback callback);

    /**
     * @brief Launches this process' instance and connects to it
     * @return object for interprocess communication with the instance, NULL on error
     */
    InterProcessClient * launchInstance();

    /**
     * @brief Connects to this process' already running instance
     * @return object for interprocess communication with the instance, NULL on error
     */
    InterProcessClient * connectToInstance();

    /**
     * @brief Is this process's instance currently running
     * @return success
     */
    bool isInstanceRunning();

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

    /**
     * @brief Locks sequence for processing by this process
     * @param seqname sequence name to lock
     * @return true on succesful lock
     */
    bool lockAssignedSequence(const std::string &seqname);

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
     * @brief Gets system PID value of running instance
     * @return PID
     */
    int getInstancePID();

    /**
     * @brief Gets instance's listenting port for IPC
     * @return port
     */
    int getInstancePort();
    
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
     * @brief Sets instance's PID
     * @param pid pid
     * @return success
     */
    bool updateInstancePID(int pid);

    /**
     * @brief Sets instance's listening port
     * @param port listening port
     * @return success
     */
    bool updateInstancePort(int port);

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
    Process& operator=(const Process&) = delete;
};

} // namespace vtapi
