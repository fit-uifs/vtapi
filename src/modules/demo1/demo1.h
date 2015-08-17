#pragma once
#include <vtapi/plugins/module_interface.h>

namespace vtapi {


class Demo1Module : public IModuleInterface
{
public:
    Demo1Module();
    ~Demo1Module();

    void initialize() override;

    void uninitialize() override;

    void process(Process & process) override;

    void control(ControlCode code) override;

private:
    volatile bool _stop;
};


}
