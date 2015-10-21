#pragma once

#include <Poco/Process.h>

namespace vtapi {
namespace compat {

bool isChildProcessRunning(Poco::ProcessHandle & handle);

}
}
