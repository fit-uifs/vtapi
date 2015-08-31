
#include <vtapi/common/global.h>
#include <vtapi/data/processstate.h>

using namespace std;

namespace vtapi {


static const struct
{
    ProcessState::Status status;
    const char *str;
}
status_map[] = {
    { ProcessState::STATUS_CREATED,     "created" },
    { ProcessState::STATUS_RUNNING,     "running" },
    { ProcessState::STATUS_SUSPENDED,   "suspended" },
    { ProcessState::STATUS_FINISHED,    "finished" },
    { ProcessState::STATUS_ERROR,       "error" }
};


ProcessState::ProcessState(const std::string& stateString)
{
    size_t pos = stateString.find('(', 0);
    size_t pos2  = stateString.find(',', pos + 1);
    size_t pos3 = stateString.find(',', pos2 + 1);
    size_t pos4 = stateString.find(',', pos3 + 1);
    size_t pos5 = stateString.find(')', pos4 + 1);
    if (pos != string::npos && pos2 != string::npos &&
        pos3 != string::npos && pos4 != string::npos &&
        pos5 != string::npos)
    {
        status      = toStatusValue(stateString.substr(pos+1,pos2-pos-1));
        progress    = atof(stateString.substr(pos2+1,pos3-pos2-1).c_str());
        current_item = stateString.substr(pos3+1, pos4-pos3-1);
        last_error   = stateString.substr(pos4+1, pos5-pos4-1);
    }        
}

ProcessState::Status ProcessState::toStatusValue(const string& status_string)
{
    Status status = STATUS_CREATED;
    
    for (int i = 0; status_map[i].str; i++) {
        if (status_string.compare(status_map[i].str) == 0) {
            status = status_map[i].status;
            break;
        }
    }
    
    return status;
}

string ProcessState::toStatusString(Status status)
{
    string status_string;

    for (int i = 0; status_map[i].str; i++) {
        if (status == status_map[i].status) {
            status_string = status_map[i].str;
            break;
        }
    }

    return status_string;
}

}
