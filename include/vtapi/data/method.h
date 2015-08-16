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

#include <list>
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
     * @param names list of method names
     */
    Method(const Commons& commons, const std::list<std::string>& names);

    /**
     * Destructor
     */
    virtual ~Method();
    
    using KeyValues::count;
    using KeyValues::print;
    using KeyValues::printAll;
    using KeyValues::printKeys;

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
    std::string getName();
    
    /**
     * Gets description of the current method
     * @return description of the current dataset
     */
    std::string getDescription();

    /**
     * @brief Gets absolute path to method library
     * @return path
     */
    std::string getPluginPath();

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
                     const std::string& outputs = std::string());
    
    /**
     * Loads method's processing tasks for iteration
     * @param name task name (no name = all tasks)
     * @return pointer to the new Task object, NULL on error
     */
    Task* loadTasks(const std::string& name = std::string());
    
protected:
    virtual bool preUpdate();
    
private:
    Method() = delete;
    Method(const Method&) = delete;
    Method& operator=(const Method&) = delete;
};

} // namespace vtapi
