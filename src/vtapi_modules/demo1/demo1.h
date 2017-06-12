#pragma once
#include <vtapi/plugins/module_interface.h>

namespace vtapi {


class Demo1Module : public IModuleInterface
{
public:
    Demo1Module();
    ~Demo1Module();

    void initialize(VTApi & vtapi) override;
    void uninitialize() noexcept override;
    void process(Process & process) override;
    void stop() noexcept override;

private:
    volatile bool _stop;
};


}
