#pragma once

#include <vtapi/common/interproc.h>
#include <vtapi/plugins/module_interface.h>

namespace vtapi {


class VTModule
{
public:
    /**
     * @brief Helper class for passing stop signal to module
     */
    class ModuleControl : public InterProcessServer::IModuleControlInterface
    {
    public:
        explicit ModuleControl(IModuleInterface & module)
            : _module(module) {}

        void stop() noexcept override
        { _module.stop(); }

    private:
        IModuleInterface & _module;
    };


    int main(int argc, char *argv[]);
};


}
