#pragma once
#include <vtapi/plugins/module_interface.h>

namespace vtapi {


class DemoModule : public IModuleInterface
{
public:
    DemoModule();
    ~DemoModule();

    virtual void install(VTApi & vtapi) override;
    virtual void uninstall(VTApi & vtapi) override;
    void initialize(VTApi & vtapi) override;
    void uninitialize() noexcept override;
    void process(Process & process) override;
    void stop() noexcept override;

private:
    volatile bool _stop;
};


}
