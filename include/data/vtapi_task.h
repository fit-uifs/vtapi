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

#include <string>
#include <list>
#include "vtapi_keyvalues.h"
#include "vtapi_process.h"
#include "vtapi_taskparams.h"

namespace vtapi {

class Task : public KeyValues
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
    
    bool next();
    
    /**
     * Gets this task's name
     * @return task name
     */
    std::string getName();

    /**
     * Gets tasks which should be completed before running this one
     * @return task object representing prerequisite tasks (for iteration), NULL on error
     */
    Task *getPrerequisiteTasks();
    
    /**
     * Gets object containing task parameters
     * @return task parameters map
     */
    TaskParams *getParams();
    
    /**
     * Gets output intervals of this process
     * @return output intervals
     */
    Interval *getOutputData();
    
    //TODO: hotove videa?
    
    /**
     * Creates new process for this task
     * @param seqnames names of sequences for which to process the task
     * @return pointer to the new Process object, NULL on error
     */
    Process* createProcess(const std::list<std::string>& seqnames);

    /**
     * Loads method's processes for iteration
     * @param id   process ID (0 = all processes)
     * @return pointer to the new Process object, NULL on error
     */
    Process* loadProcesses(int id = 0);

protected:
    virtual bool preUpdate();
    
private:
    Task() = delete;
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
};

}