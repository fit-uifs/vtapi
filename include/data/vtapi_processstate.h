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

namespace vtapi {

/**
 * @brief
 * 
 * Class representing current process state
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
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
    typedef enum _STATUS_T
    {
        STATUS_NONE,        /**< invalid status */
        STATUS_CREATED,     /**< process has been newly registered but not yet started */
        STATUS_RUNNING,     /**< process is currently working */
        STATUS_SUSPENDED,   /**< process is currently in paused state */
        STATUS_FINISHED,    /**< process has finished succesfully */
        STATUS_ERROR        /**< process has finished with an error */
    } STATUS_T;
    

    ProcessState();
    explicit ProcessState(const std::string& stateString);
    explicit ProcessState(STATUS_T status, float progress = 0, const std::string& item = "");
    virtual ~ProcessState();

    static STATUS_T toStatusValue(const std::string& status_string);
    static std::string toStatusString(STATUS_T status);

    
    STATUS_T status;            /**< current process status */
    float progress;             /**< process progress (0-100) */
    std::string currentItem;    /**< currently processed item */
    std::string lastError;      /**< last error message */
};

}
