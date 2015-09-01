/**
 * @file
 * @brief   Declaration of Method class
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
#include "task.h"
#include "taskparams.h"

namespace vtapi {

class Task;
class TaskParams;

/**
 * @brief A class which represents methods and gets also their keys
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
class Method : protected KeyValues
{
public:
    /**
     * @brief Copy constructor
     * @param copy original object
     */
    Method(const Method& copy);

    /**
     * Construct method object for iterating through VTApi methods
     * If a specific name is set, object will represent one method only
     * @param commons base Commons object
     * @param name method name, empty for all methods
     */
    Method(const Commons& commons, const std::string& name = std::string());

    /**
     * Construct method object for iterating through VTApi methods
     * Object will represent set of methods specified by their names
     * @param commons base Commons object
     * @param names vector of method names
     */
    Method(const Commons& commons, const std::vector<std::string>& names);
    
    using KeyValues::count;
    using KeyValues::updateExecute;

    /**
     * Moves to a next method and set a method name and its methodkeys variables
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next() override;
    
    /**
     * Gets a name of the current method
     * @return string value with the name of the method
     */
    std::string getName() const;
    
    /**
     * Gets object containing method parameters definitions
     * @return task parameters map
     */
    TaskParamDefinitions getParamDefinitions() const;

    /**
     * Gets description of the current method
     * @return description of the current dataset
     */
    std::string getDescription() const;

    /**
     * @brief Gets absolute path to method library
     * @return path
     */
    std::string getPluginPath() const;

    /**
     * Sets method's description
     * @param description new description
     * @return success
     */
    bool updateDescription(const std::string& description);
    
    /**
     * @brief Creates new task for given dataset
     * @param dsname dataset name
     * @param params task parameters
     * @param prereq_task prerequisite task, empty for none
     * @param outputs optional output table
     * @return created task object, NULL on error
     */
    Task *createTask(const std::string& dsname,
                     const TaskParams& params,
                     const std::string& prereq_task,
                     const std::string& outputs = std::string()) const;
    
    /**
     * Loads method's processing tasks for iteration
     * @param name task name (no name = all tasks)
     * @return pointer to the new Task object, NULL on error
     */
    Task* loadTasks(const std::string& name = std::string()) const;

    /**
     * @brief Deletes task with given name
     * @param dsname dataset with given task
     * @param taskname task name to delete
     * @return succesful delete
     */
    bool deleteTask(const std::string &dsname, const std::string &taskname) const;

protected:
    bool preUpdate() override;
    
private:
    Method() = delete;
    Method& operator=(const Method&) = delete;
};

} // namespace vtapi
