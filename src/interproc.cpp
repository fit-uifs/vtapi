
#include <csignal>
#include <vtapi/common/interproc.h>
#include <vtapi/common/global.h>
#include <vtapi/common/exception.h>

#define DEF_STOP_CHECK_INTERVAL_MS      250
#define DEF_CONNECT_ATTEMPTS    10
#define DEF_SERVER_ALIVE_CHECK_PERIOD_S 2
#define DEF_MAX_CLIENTS         15

using namespace std;

namespace vtapi {


std::atomic_bool InterProcessServer::_signal_flag(false);
std::atomic_bool InterProcessServer::_signals_installed(false);

void InterProcessServer::sighandler(int sig)
{
    switch (sig)
    {
    case SIGINT:
    case SIGTERM:
        _signal_flag = true;
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

    _stop_loop = false;
    _stop_check_thread = std::thread(&InterProcessServer::stopCheckLoop, this);
}

InterProcessServer::~InterProcessServer()
{
    if (_signals_installed) {
        std::signal(SIGINT, SIG_DFL);
        std::signal(SIGTERM, SIG_DFL);
        _signals_installed = false;
    }

    _stop_loop = true;
    _stop_check_thread.join();
}

void InterProcessServer::stopCheckLoop()
{
    while(!_stop_loop) {
        bool stop = false;
        if (_mtx.tryLock()) {
            _mtx.unlock();
            if (_signal_flag) {
                stop = true;
                VTLOG_DEBUG("interproc : server stopped by signal: " + _ipc_base_name);
            }
        }
        else {
            stop = true;
            VTLOG_DEBUG("interproc : server stopped by mutex: " + _ipc_base_name);
        }
        if (stop) _control.stop();
        this_thread::sleep_for(chrono::milliseconds(DEF_STOP_CHECK_INTERVAL_MS));
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
        return Poco::Process::isRunning(*_phproc);
    else if (_pid > 0)
        return Poco::Process::isRunning(_pid);
    else
        throw InterProcessException("Invalid client instance");
}

void InterProcessClient::stop()
{
    if (_mtx.tryLock()) {
        VTLOG_DEBUG("interproc : attempting to stop by mutex: " + _ipc_base_name);
        this_thread::sleep_for(chrono::milliseconds(DEF_STOP_CHECK_INTERVAL_MS*2));
        _mtx.unlock();
    }
}

void InterProcessClient::kill()
{
    if (_phproc)
        Poco::Process::kill(*_phproc);
    else if (_pid > 0)
        Poco::Process::kill(_pid);
    else
        throw InterProcessException("Failed to kill(): invalid client instance");
}

void InterProcessClient::wait()
{
    if (_phproc)
        Poco::Process::wait(*_phproc);
    else
        throw InterProcessException("Failed to wait(): must be child process");
}




}
