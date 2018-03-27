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
 
#include "keyvalues.h"
#include "dataset.h"
#include "sequence.h"
#include "task.h"
#include "taskprogress.h"
#include "method.h"
#include "processstate.h"
#include "../common/interproc.h"


namespace vtapi {

class Dataset;
class Sequence;
class ImageFolder;
class Video;
class Task;
class TaskProgress;
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
     * @param names vector of method processes
     */
    Process(const Commons& commons, const std::vector<int>& ids);
    
    using KeyValues::count;
    using KeyValues::updateExecute;

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
     * @param control control interface for invoking commands to processing
     * @return object for registering interprocess communication with clients, NULL on error
     */
    InterProcessServer * initializeInstance(InterProcessServer::IModuleControlInterface & control);

    /**
     * @brief Launches this process' instance and connects to it
     * @return object for interprocess communication with the instance, NULL on error
     */
    InterProcessClient * launchInstance();

    /**
     * @brief Connects to this process' already running instance
     * @return object for interprocess communication with the instance, NULL on error
     */
    InterProcessClient * connectToInstance() const;

    /**
     * @brief Is this process's instance currently running
     * @return success
     */
    bool isInstanceRunning() const;

    //////////////////////////////////////////////////
    // getters - associated objects
    //////////////////////////////////////////////////

    /**
     * @brief Gets parent dataset object
     * @return dataset object (initialized)
     */
    Dataset *getParentDataset() const;

    /**
     * @brief Gets name of parent task object
     * @return parent task object name
     */
    std::string getParentTaskName() const;

    /**
     * @brief Gets parent task object
     * @return task object (initialized)
     */
    Task *getParentTask() const;

    /**
     * @brief Gets name of parent method object
     * @return parent method object name
     */
    std::string getParentMethodName() const;

    /**
     * @brief Gets parent method object
     * @return method object (initialized)
     */
    Method *getParentMethod() const;
    
    /**
     * @brief Loads sequences names which should be processed
     * @return sequences names vector
     */
    std::vector<std::string> loadAssignedSequencesNames() const;
    
    /**
     * @brief Loads sequences which should be processed
     * @return sequences object for iteration
     */
    Sequence *loadAssignedSequences() const;

    /**
     * @brief Loads image folders which should be processed
     * @return image folders object for iteration
     */
    ImageFolder *loadAssignedImageFolders() const;

    /**
     * @brief Loads videos which should be processed
     * @return videos object for iteration
     */
    Video *loadAssignedVideos() const;

    /**
     * @brief Acquires lock on sequence for processing by this process
     * Call updateIsDone(true) on TaskProgress object to mark sequence as done.
     * Call updateIsDone(false) on TaskProgress object to mark sequence as being processed.
     * Or delete TaskProgress object without calling updateIsDone() to unlock sequence.
     * @param seqname sequence name to lock
     * @return task progress object to update on sequence done, NULL on failed lock
     */
    TaskProgress *acquireSequenceLock(const std::string &seqname) const;

    //////////////////////////////////////////////////
    // getters - SELECT
    //////////////////////////////////////////////////
    
    /**
     * Gets a process ID
     * @return process ID
     */
    int getId() const;

    /**
     * Gets detailed process state
     * @return process state object
     */
    ProcessState getState() const;

    /**
     * @brief Gets system PID value of running instance
     * @return PID
     */
    int getInstancePID() const;

    /**
     * @brief Gets instance's base name for IPC
     * @return base instance name
     */
    std::string getInstanceName() const;
    
    /**
     * @brief Gets time when sequence was added to dataset
     * @return timestamp
     */
    std::chrono::system_clock::time_point getCreatedTime() const;

    //////////////////////////////////////////////////
    // updaters - UPDATE
    //////////////////////////////////////////////////

    /**
     * Sets process state
     * @param state process state
     * @return success
     */
    bool updateState(const ProcessState& state);

    /**
     * @brief Sets instance's PID
     * @param pid pid
     * @return success
     */
    bool updateInstancePID(int pid);

    /**
     * @brief Sets instance's base name for IPC
     * @param name IPC name
     * @return success
     */
    bool updateInstanceName(const std::string & name);

    //////////////////////////////////////////////////
    // filters/utilities
    //////////////////////////////////////////////////

    /**
     * Filters iteration via next() by task
     */
    void filterByTask(const std::string& taskname);

    /**
     * @brief Constructs unique process ID
     * @return unique ID
     */
    std::string constructUniqueName() const;

protected:
    bool preUpdate() override;

private:
    Process() = delete;
    Process& operator=(const Process&) = delete;
};

} // namespace vtapi
