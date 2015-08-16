#pragma once

#include <vtapi/data/process.h>
#include <vtapi/data/task.h>
#include <vtapi/data/sequence.h>

namespace vtapi {


class IModuleInterface
{
public:
    virtual ~IModuleInterface() {}

    /**
     * @brief Called always on plugin initialization
     * @return true => continue processing, false => quit
     */
    virtual bool initialize() = 0;

    /**
     * @brief Called always on plugin uninitialization
     */
    virtual void uninitialize() = 0;

    /**
     * @brief Main processing function
     * Process list of videos and image folders:
     * 1. iterate over them using next()
     * 2. video.openVideo() to open video, video.getCapture() to get OpenCV capture
     * 3. don't use image folders yet :)
     * 4. output = task.createIntervalOutput(video.getName()) to open output data
     * 5. interval = output->newInterval(...) to create new output event
     * 6. interval->setXXX(...) multiple times to set output event data
     * 7. repeat 5. and 6. many times
     * 8. output->commit() to save data to database
     * @param task task to be performed (initialized)
     * @param videos assigned videos to be processed
     * @param imagefolders assigned image folders to be processed (not working now)
     */
    virtual void process(Process & process,
                         Task & task,
                         Video & videos,
                         ImageFolder & imagefolders) = 0;
};


}
