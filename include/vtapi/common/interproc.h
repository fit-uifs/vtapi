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

#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <Poco/Process.h>
#include <vtapi/data/process.h>

namespace vtapi {

class Process;


class InterProcessBase
{
protected:
    Process *_pprocess;

    InterProcessBase(const Process & process);
    ~InterProcessBase();

private:
    InterProcessBase() = delete;
};


class InterProcessServer : public InterProcessBase
{
public:
    typedef enum
    {
        CommandSuspend,
        CommandResume,
        CommandStop
    } Command;

    typedef void (*fnCommandCallback)(Command command);

    explicit InterProcessServer(const Process & process, fnCommandCallback callback);
    ~InterProcessServer();

    bool installSignalHandlers();
    void uninstallSignalHandlers();

    int startListening();
    void stopListening();

private:
    static fnCommandCallback _callback;
    static std::atomic_bool _signals_installed;
    static void sighandler(int sig);

    std::thread _server_thread;
    std::mutex _server_ready_mtx;
    std::condition_variable _server_ready_cv;
    std::atomic_bool _stop_server;
    int _server_port;

    static void serverProc(InterProcessServer *context);
    void serverLoop();

    InterProcessServer() = delete;
};



class InterProcessClient : public InterProcessBase
{
public:
    InterProcessClient(const Process & process, Poco::ProcessHandle hproc);
    explicit InterProcessClient(const Process & process);
    ~InterProcessClient();

    bool isRunning();
    bool suspend();
    bool resume();
    bool stop();
    bool kill();
    bool wait();

private:
    Poco::ProcessHandle *_phproc;

    InterProcessClient() = delete;
};


}
