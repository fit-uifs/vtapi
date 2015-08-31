/**
 * @file
 * @brief   Declaration of ProcessState class
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

namespace vtapi {

/**
 * @brief
 * 
 * Class representing current process state
 * 
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class ProcessState
{
public:
    enum Status
    {
        STATUS_CREATED,     /**< process has been newly registered but not yet started */
        STATUS_RUNNING,     /**< process is currently working */
        STATUS_SUSPENDED,   /**< process is currently in paused state */
        STATUS_FINISHED,    /**< process has finished succesfully */
        STATUS_ERROR        /**< process has finished with an error */
    };
    

    //TODO: process state header only

    ProcessState()
        : status(STATUS_CREATED), progress(0) {}

    explicit ProcessState(Status status,
                          double progress = 0.0,
                          const std::string& item = std::string())
        : status(status), progress(progress)
    {
        if (status == STATUS_RUNNING)
            current_item = item;
        else if (status == STATUS_ERROR)
            last_error = item;
    }

    explicit ProcessState(const std::string& stateString);

    static Status toStatusValue(const std::string& status_string);
    static std::string toStatusString(Status status);

    
    Status status;              /**< current process status */
    double progress;            /**< process progress (0-100) */
    std::string current_item;   /**< currently processed item */
    std::string last_error;     /**< last error message */
};

}
