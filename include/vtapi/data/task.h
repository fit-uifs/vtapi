/**
 * @file
 * @brief   Declaration of Task class
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
#include "interval.h"
#include "method.h"
#include "process.h"
#include "taskparams.h"
#include "taskprogress.h"
#include "intervaloutput.h"

namespace vtapi {

class Dataset;
class Sequence;
class Interval;
class Method;
class Process;
class TaskParams;
class TaskProgress;
class IntervalOutput;


class Task : protected KeyValues
{
public:
    /**
     * @brief Copy constructor
     * @param copy original object
     */
    Task(const Task& copy);

    /**
     * Construct task object for iterating through VTApi tasks
     * If a specific name is set, object will represent one task only
     * @param commons base Commons object
     * @param name task name, empty for all tasks
     */
    Task(const Commons& commons, const std::string& name = std::string());

    /**
     * Construct task object for iterating through VTApi tasks
     * Object will represent set of tasks specified by their names
     * @param commons base Commons object
     * @param names vector of method tasks
     */
    Task(const Commons& commons, const std::vector<std::string>& names);

    using KeyValues::count;
    using KeyValues::updateExecute;

    /**
     * @brief Iterates to next task
     * @return success on task existing
     */
    bool next() override;

    //////////////////////////////////////////////////
    // getters - associated objects
    //////////////////////////////////////////////////

    /**
     * @brief Gets parent dataset object
     * @return dataset object (initialized)
     */
    Dataset *getParentDataset() const;

    /**
     * @brief Gets parent method name
     * @return method name
     */
    std::string getParentMethodName() const;

    /**
     * @brief Gets parent method object
     * @return method object (initialized)
     */
    Method *getParentMethod() const;

    /**
     * @brief Gets output data table name for this task
     * @return table name
     */
    std::string getOutputDataTable() const;

    /**
     * @brief Gets current task's process ID (only for methods where progress is not based on sequences)
     * @note only for methods where progress is not based on sequences
     * @return process ID
     */
    int getProcessId() const;

    /**
     * @brief Gets current task's process object (only for methods where progress is not based on sequences)
     * @note only for methods where progress is not based on sequences
     * @return process object
     */
    Process *getProcess() const;

    /**
     * @brief Gets information on task's progress for all sequences
     * @return progress object for iteration
     */
    TaskProgress *loadTaskProgress() const;

    /**
     * @brief Gets information on task's progress for specified sequences
     * @param seqnames names of sequences to check for progress
     * @return progress object for iteration
     */
    TaskProgress *loadTaskProgress(const std::vector<std::string>& seqnames) const;

    /**
     * Gets tasks which should be completed before running this one
     * @return task object representing prerequisite tasks for iteration
     */
    Task *loadPrerequisiteTasks() const;

    /**
     * Gets output intervals of this process
     * @return output intervals for iteration
     */
    Interval *loadOutputData() const;

    /**
     * Gets output intervals of specific output data table
     * @return output intervals for iteration
     */
    Interval *loadOutputData(std::string outputDataTable) const;

    /**
     * Loads method's processes for iteration
     * @param id   process ID (0 = all processes)
     * @return process object for iteration
     */
    Process *loadProcesses(int id = 0) const;

    //////////////////////////////////////////////////
    // getters - SELECT
    //////////////////////////////////////////////////

    /**
     * Gets this task's name
     * @return task name
     */
    std::string getName() const;

    /**
     * Gets object containing task parameters
     * @return task parameters map
     */
    TaskParams getParams() const;

    /**
     * @brief Gets time when sequence was added to dataset
     * @return timestamp
     */
    std::chrono::system_clock::time_point getCreatedTime() const;

    //////////////////////////////////////////////////
    // create - INSERT
    //////////////////////////////////////////////////

    /**
     * Creates new process for this task
     * @param seqnames names of sequences for which to process the task
     * @return pointer to the new Process object, NULL on error
     */
    Process* createProcess(const std::vector<std::string>& seqnames) const;

    /**
     * @brief Creates new object for outputting data
     * @param seqname output data for this sequence
     * @return interval output object, NULL on error
     */
    IntervalOutput *createIntervalOutput(const std::string &seqname) const;

    //////////////////////////////////////////////////
    // delete data - DELETE
    //////////////////////////////////////////////////

    /**
     * @brief Deletes output data for one or all sequences
     * @param seqname sequence name for which to delete data, empty for all
     * @return success
     */
    bool deleteOutputData(const std::string &seqname = std::string()) const;

    /**
     * @brief Deletes output data for specified set of sequences
     * @param seqnames sequence names for which to delete data
     * @return success
     */
    bool deleteOutputData(const std::vector<std::string> &seqnames) const;

    /**
     * @brief Deletes one event from output data
     * @param intervalID ID of event to delete
     * @return success
     */
    bool deleteOutputData(int intervalID) const;

    /**
     * @brief Deletes multiple events from output data
     * @param intervalIDs IDs of events to delete
     * @return success
     */
    bool deleteOutputData(const std::vector<int> & intervalIDs) const;

    //////////////////////////////////////////////////
    // misc
    //////////////////////////////////////////////////

    /**
     * Constructs a task name from input parameters
     * @param mtname method name of this task
     * @param params container
     * @return process name
     */
    static std::string constructName(const std::string &mtname,
                                     const TaskParams &params);

protected:
    bool preUpdate() override;

private:
    Task() = delete;
    Task& operator=(const Task&) = delete;
};

}
