
#include <vtapi/plugins/module_interface.h>

namespace vtapi {


class Demo2Module : public IModuleInterface
{
public:
    Demo2Module();
    ~Demo2Module();

    bool initialize() override;

    void uninitialize() override;

    void process(Process & process,
                 Task & task,
                 Video & videos,
                 ImageFolder & imagefolders) override;
};


}
