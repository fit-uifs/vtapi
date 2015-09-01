#pragma once

#include <vtapi/common/exception.h>
#include <vtapi/vtapi.h>

namespace vtapi {


class IModuleInterface
{
public:
    /**
     * @brief virtual destructor
     */
    virtual ~IModuleInterface() {}

    /**
     * @brief Module initialization
     * Called ALWAYS on plugin initialization
     * Throw vtapi::RuntimeModuleException on failure
     * @param vtapi main vtapi object to access VTApi for initialization purposes
     * @throws vtapi::RuntimeModuleException initialization error
     */
    virtual void initialize(VTApi & vtapi) = 0;

    /**
     * @brief Module uninitialization
     * Called ALWAYS on plugin uninitialization
     */
    virtual void uninitialize() noexcept = 0;

    /**
     * @brief Main processing function
     * Called after initialization ended without error
     * Throw vtapi::RuntimeModuleException on failure
     * Throw vtapi::ModuleUserAbortException on user abort
     * Proper processing may get a bit complicated, check demo modules
     * for example
     * @param process process object representing processing to be done
     * @throws vtapi::RuntimeModuleException processing error
     * @throws vtapi::ModuleUserAbortException processing error
     */
    virtual void process(Process & process) = 0;

    /**
     * @brief Call to this function should cause currently active processing
     * to throw a vtapi::ModuleUserAbortException
     * It is called during process() function from a different thread (!)
     * It should return ASAP and not wait for processing end
     */
    virtual void stop() noexcept = 0;
};


}
