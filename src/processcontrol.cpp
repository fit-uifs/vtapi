
#include <common/vtapi_global.h>
#include <data/vtapi_processcontrol.h>

using std::string;
using std::cout;
using std::endl;

using namespace vtapi;

static const struct
{
    ProcessControl::COMMAND_T command;
    const char *str;
}
command_map[] = {
    { ProcessControl::COMMAND_RESUME,   "resume" },
    { ProcessControl::COMMAND_SUSPEND,  "suspend" },
    { ProcessControl::COMMAND_STOP,     "stop" },
    { ProcessControl::COMMAND_NONE,     NULL },
};

ProcessControl::ProcessControl(const std::string& name)
{
    processName = name;
}

ProcessControl::~ProcessControl()
{
    close();
}

bool ProcessControl::server(fServerCallback callback, void *context)
{
    cout << "comm: SERVER connect: " << processName << " (NOT YET IMPLEMENTED)" << endl;
    return true;
}

bool ProcessControl::client(unsigned int connectTimeout, fClientCallback callback, void *callbackContext)
{
    cout << "comm: CLIENT connect: " << processName << " (NOT YET IMPLEMENTED)" << endl;
    return true;
}

bool ProcessControl::control(COMMAND_T command)
{
    cout << "comm: CONTROL " << toCommandString(command) << ' ' << processName << "(NOT YET IMPLEMENTED)" << endl;
    return true;
}

bool ProcessControl::notify(const ProcessState& state)
{
    cout << "comm: NOTIFY " << ProcessState::toStatusString(state.status) <<
    " (" << state.progress << "%) "<< processName << "(NOT YET IMPLEMENTED)" << endl;
    return true;
}

bool ProcessControl::close()
{
    return true;
}

ProcessControl::COMMAND_T ProcessControl::toCommandValue(const std::string& command_string)
{
    COMMAND_T command = COMMAND_NONE;

    for (int i = 0; command_map[i].str; i++) {
        if (command_string.compare(command_map[i].str) == 0) {
            command = command_map[i].command;
            break;
        }
    }

    return command;
}

std::string ProcessControl::toCommandString(COMMAND_T command)
{
    string command_string;

    for (int i = 0; command_map[i].str; i++) {
        if (command == command_map[i].command) {
            command_string = command_map[i].str;
            break;
        }
    }

    return command_string;
}