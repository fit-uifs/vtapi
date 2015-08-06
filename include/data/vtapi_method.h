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

#include <string>
#include <list>
#include "vtapi_keyvalues.h"
#include "vtapi_task.h"
#include "vtapi_process.h"

namespace vtapi {

/**
 * @brief A class which represents methods and gets also their keys
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @note Error codes 35*
 * 
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Method : public KeyValues
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
    
    /**
     * Moves to a next method and set a method name and its methodkeys variables
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next();
    
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
     * Sets method's description
     * @param description new description
     * @return success
     */
    bool updateDescription(const std::string& description);
    
    /**
     * Creates new task for this method
     * @param params task parameters list
     * @param tasknames_prereq which tasks are prerequisities to the new task
     * @param outputTable destination table for task outputs (empty => <methodname>_out)
     * @return pointer to the new Task object, NULL on error
     */
    Task* createTask(
        const TaskParams& params,
        const std::list<std::string>& tasknames_prereq,
        const std::string& outputTable = std::string());
    
    /**
     * Loads method's processing tasks for iteration
     * @param name task name (no name = all tasks)
     * @return pointer to the new Task object, NULL on error
     */
    Task* loadTasks(const std::string& name = std::string());

    /**
     * Constructs a task name from input parameters
     * @param params container
     * @return process name
     */
    std::string constructTaskName(const TaskParams &params);
    
protected:
    virtual bool preUpdate();
    
private:
    Method() = delete;
    Method(const Method&) = delete;
    Method& operator=(const Method&) = delete;
};

} // namespace vtapi
