#pragma once

#include <vtapi/common/exception.h>
#include <vtapi/data/process.h>

namespace vtapi {


class IModuleInterface
{
public:
    // module control codes
    enum ControlCode
    {
        ControlSuspend,     // suspend processing
        ControlResume,      // resume suspended processing
        ControlStop         // stop all processing and return
    };

    /**
     * @brief virtual destructor
     */
    virtual ~IModuleInterface() {}

    /**
     * @brief Module initialization
     * Called ALWAYS on plugin initialization
     * Throw vtapi::RuntimeModuleException on failure
     * @throws vtapi::RuntimeModuleException initialization error
     */
    virtual void initialize() = 0;

    /**
     * @brief Module uninitialization
     * Called ALWAYS on plugin uninitialization
     */
    virtual void uninitialize() noexcept = 0;

    /**
     * @brief Main processing function
     * Called when initialization ended without error
     * Throw vtapi::RuntimeModuleException on failure
     *
     * Typical processing:
     * 1. get task associated with process:
     *      task = process.getParentTask();
     * 2. load prerequisite tasks for our task:
     *      task_prereq = task.loadPrerequisiteTasks();
     *      task_prereq->next();
     * 3. get task parameters:
     *      params = task->getParams();
     * 4. get assigned videos:
     *      videos = process.loadAssignedVideos();
     * 5. iterate over videos:
     *      while(videos->next()) { ... }
     * 6. check if video has been processed by prerequisite tasks:
     *      task_prereq->isSequenceFinished(videos->getName());
     * 7. save outputs for each video:
     *      output = task.createIntervalOutput(video.getName());
     *      while (continue_processing) {
     *          // do some processing here
     *          // now save video interval
     *          interval = output->newInterval(...);
     *          interval->setXXX(...);
     *          interval->setYYY(...);
     *          ...
     *      }
     *      output->commit();
     * 8. update process status regularly:
     *      process.updateStateXXX(...);
     * @param process process object representing processing to be done
     * @throws vtapi::RuntimeModuleException processing error
     */
    virtual void process(Process & process) = 0;

    /**
     * @brief Module control implementation (suspending, resuming, stopping)
     * Called during process() function from different thread (!)
     * @param code control code
     */
    virtual void control(ControlCode code) = 0;
};


}
