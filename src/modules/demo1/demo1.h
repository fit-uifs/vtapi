
#include <vtapi/plugins/module_interface.h>

namespace vtapi {


class Demo1Module : public IModuleInterface
{
public:
    Demo1Module();
    ~Demo1Module();

    bool initialize() override;

    void uninitialize() override;

    void process(Process & process,
                 Task & task,
                 Video & videos,
                 ImageFolder & imagefolders) override;
};


}
