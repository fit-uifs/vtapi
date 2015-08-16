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

#include <list>
#include "keyvalues.h"
#include "dataset.h"
#include "sequence.h"
#include "interval.h"
#include "method.h"
#include "process.h"
#include "taskparams.h"
#include "intervaloutput.h"

namespace vtapi {

class Dataset;
class Sequence;
class Interval;
class Method;
class Process;
class TaskParams;
class IntervalOutput;


class Task : protected KeyValues
{
public:
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
     * @param names list of method tasks
     */
    Task(const Commons& commons, const std::list<std::string>& names);

    ~Task();
    
    using KeyValues::count;
    using KeyValues::print;
    using KeyValues::printAll;
    using KeyValues::printKeys;

    /**
     * @brief Iterates to next task
     * @return success on existing task
     */
    bool next() override;
    
    //////////////////////////////////////////////////
    // getters - associated objects
    //////////////////////////////////////////////////

    /**
     * @brief Gets parent dataset object
     * @return dataset object (initialized)
     */
    Dataset *getParentDataset();

    /**
     * @brief Gets parent method name
     * @return method name
     */
    std::string getParentMethodName();

    /**
     * @brief Gets parent method object
     * @return method object (initialized)
     */
    Method *getParentMethod();

    /**
     * @brief Gets specified sequence object and marks is as "in progress"
     * @param seqname sequence name
     * @return sequence object (initialized), NULL on failed lock
     */
    Sequence *getSequenceLock(const std::string& seqname);

    /**
     * Gets tasks which should be completed before running this one
     * @return task object representing prerequisite tasks for iteration
     */
    Task *loadPrerequisiteTasks();

    /**
     * @brief Gets output data table name for this task
     * @return table name
     */
    std::string getOutputDataTable();

    /**
     * Gets output intervals of this process
     * @return output intervals for iteration
     */
    Interval *loadOutputData();

    /**
     * @brief Loads sequences "in progress" for iteration
     * @return sequence object for iteration
     */
    Sequence *loadSequencesInProgress();

    /**
     * @brief Loads finished sequences for iteration
     * @return sequence object for iteration
     */
    Sequence *loadSequencesFinished();

    /**
     * Loads method's processes for iteration
     * @param id   process ID (0 = all processes)
     * @return process object for iteration
     */
    Process *loadProcesses(int id = 0);

    //////////////////////////////////////////////////
    // getters - SELECT
    //////////////////////////////////////////////////

    /**
     * Gets this task's name
     * @return task name
     */
    std::string getName();
    
    /**
     * Gets object containing task parameters
     * @return task parameters map
     */
    TaskParams *getParams();

    
    //////////////////////////////////////////////////
    // create - INSERT
    //////////////////////////////////////////////////

    /**
     * Creates new process for this task
     * @param seqnames names of sequences for which to process the task
     * @return pointer to the new Process object, NULL on error
     */
    Process* createProcess(const std::list<std::string>& seqnames);

    /**
     * @brief Creates new object for outputting data
     * @param seqname output data for this sequence
     * @return interval output object, NULL on error
     */
    IntervalOutput *createIntervalOutput(const std::string &seqname);

    //////////////////////////////////////////////////
    // misc
    //////////////////////////////////////////////////

    /**
     * Constructs a task name from input parameters
     * @param mtname method name of this task
     * @param params container
     * @return process name
     */
    static std::string constructName(const std::string &mtname, const TaskParams &params);

protected:
    virtual bool preUpdate();
    
private:
    Task() = delete;
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
};

}
