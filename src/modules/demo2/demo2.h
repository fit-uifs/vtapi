#pragma once
#include <vtapi/plugins/module_interface.h>

namespace vtapi {


class Demo2Module : public IModuleInterface
{
public:
    Demo2Module();
    ~Demo2Module();

    void initialize(VTApi & vtapi) override;
    void uninitialize() noexcept override;
    void process(Process & process) override;
    void stop() noexcept override;

private:
    volatile bool _stop;
};


}
