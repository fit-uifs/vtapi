/**
 * @file
 * @brief   Declaration of ProcessComm class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 *
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include <Poco/Process.h>
#include <csignal>
#include <string>
#include <atomic>
#include <thread>

namespace vtapi {


class InterProcessBase
{
protected:
    const std::string _ipc_base_name;

    explicit InterProcessBase(const std::string & ipc_base_name)
        : _ipc_base_name(ipc_base_name) {}

private:
    InterProcessBase() = delete;
};


class InterProcessServer : public InterProcessBase
{
public:
    class IModuleControlInterface
    {
    public:
        virtual void stop() noexcept = 0;
    };


    explicit InterProcessServer(const std::string & ipc_base_name,
                                IModuleControlInterface & control);
    ~InterProcessServer();

private:
    std::atomic_bool _stop_loop;
    static std::atomic_bool _signal_flag;
    static std::atomic_bool _signals_installed;
    static void sighandler(int sig);

    IModuleControlInterface & _control;
    std::thread _stop_check_thread;

    void stopCheckLoop();

    InterProcessServer() = delete;
};



class InterProcessClient : public InterProcessBase
{
public:
    InterProcessClient(const std::string & ipc_base_name,
                       int pid,
                       Poco::ProcessHandle hproc);
    explicit InterProcessClient(const std::string & ipc_base_name, int pid);

    bool isRunning();
    void stop();
    void kill();
    void wait();

private:
    int _pid;
    std::shared_ptr<Poco::ProcessHandle> _phproc;

    InterProcessClient() = delete;
};


}
