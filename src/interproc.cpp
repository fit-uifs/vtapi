
#include <csignal>
#include <vtapi/common/interproc.h>
#include <vtapi/common/global.h>
#include <vtapi/common/exception.h>
#include <vtapi/common/compat.h>

#define DEF_STOP_CHECK_INTERVAL_MS      250
#define DEF_CONNECT_ATTEMPTS    10
#define DEF_SERVER_ALIVE_CHECK_PERIOD_S 2
#define DEF_MAX_CLIENTS         15

using namespace std;

namespace vtapi {


Poco::Event InterProcessServer::_stop_event_local(false);
std::atomic_bool InterProcessServer::_signals_installed(false);

void InterProcessServer::sighandler(int sig)
{
    switch (sig)
    {
    case SIGINT:
    case SIGTERM:
        _stop_event_local.set();
        break;
    default:
        break;
    }
}


InterProcessServer::InterProcessServer(const string & ipc_base_name,
                                       IModuleControlInterface & control)
    : InterProcessBase(ipc_base_name), _control(control)
{
    if (_signals_installed)
        throw InterProcessException("InterProcess server already running");

    std::signal(SIGINT, InterProcessServer::sighandler);
    std::signal(SIGTERM, InterProcessServer::sighandler);
    _signals_installed = true;

    _stopped_by_user = false;
    _stop_check_thread = std::thread(&InterProcessServer::stopCheckLoop, this);
    _stop_wait_thread = std::thread(&InterProcessServer::stopWaitProc, this);
}

InterProcessServer::~InterProcessServer()
{
    if (_signals_installed) {
        std::signal(SIGINT, SIG_DFL);
        std::signal(SIGTERM, SIG_DFL);
        _signals_installed = false;
    }

    _stop_event_local.set();
    _stop_check_thread.join();
    _stop_event_global.set();
    _stop_wait_thread.join();
}

void InterProcessServer::stopCheckLoop()
{
    for(;;) {
        if (_stop_event_local.tryWait(DEF_STOP_CHECK_INTERVAL_MS)) {
            if (_stopped_by_user) {
                VTLOG_DEBUG("interproc : server stopped: " + _ipc_base_name);
                _control.stop();
            }
            break;
        }
    }
}

void InterProcessServer::stopWaitProc()
{
    _stop_event_global.wait();
    if (!_stop_event_local.tryWait(0)) {
        _stopped_by_user = true;
        _stop_event_local.set();
    }
}


InterProcessClient::InterProcessClient(const string & ipc_base_name,
                                       int pid,
                                       Poco::ProcessHandle hproc)
    : InterProcessBase(ipc_base_name), _pid(pid)
{
    _phproc = std::make_shared<Poco::ProcessHandle>(hproc);
}

InterProcessClient::InterProcessClient(const string & ipc_base_name, int pid)
    : InterProcessBase(ipc_base_name), _pid(pid)
{
}

bool InterProcessClient::isRunning()
{
    if (_phproc)
        return compat::isChildProcessRunning(*_phproc);
    else
        throw InterProcessException("Invalid client instance");
}

void InterProcessClient::stop()
{
    VTLOG_DEBUG("interproc : attempting to stop by event: " + _ipc_base_name);
    _stop_event_global.set();
}

void InterProcessClient::kill()
{
    if (_phproc) {
        VTLOG_DEBUG("interproc : killing by handle: " + _ipc_base_name);
        try
        {
            Poco::Process::kill(*_phproc);
        }
        catch (Poco::Exception) {}
    }
    else if (_pid > 0) {
        VTLOG_DEBUG("interproc : killing by PID: " + _ipc_base_name);
        try
        {
            Poco::Process::kill(_pid);
        }
        catch (Poco::Exception) {}
    }
    else {
        throw InterProcessException("Failed to kill(): invalid client instance");
    }
}

void InterProcessClient::wait()
{
    if (_phproc) {
        VTLOG_DEBUG("interproc : waiting for finish: " + _ipc_base_name);
        try
        {
            Poco::Process::wait(*_phproc);
        }
        catch (Poco::Exception) {}
    }
    else {
        throw InterProcessException("Failed to wait(): must be child process");
    }
}




}
