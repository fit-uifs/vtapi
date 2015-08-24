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
#include <Poco/Process.h>

namespace vtapi {


class InterProcessServer
{
public:
    typedef enum
    {
        StateRunning,
        StateSuspended,
        StateStopping
    } State;

    typedef void (*fnSignalCallback)(State new_state);

    explicit InterProcessServer(int vt_prsid);
    ~InterProcessServer();

    bool installSignalHandlers(fnSignalCallback *callback);
    void uninstallSignalHandlers();

    int startListening();
    void stopListening();
    int getListeningPort();

private:
    int _vt_prsid;
    bool _signals_installed;
    int _port;

    InterProcessServer() = delete;
};

class InterProcessClient
{
public:
    InterProcessClient(int vt_prsid, Poco::ProcessHandle hproc, int port);
    InterProcessClient(int vt_prsid, int pid, int port);
    ~InterProcessClient();

    bool isRunning();
    bool suspend();
    bool resume();
    bool stop();
    bool kill();
    bool wait();

private:
    int _vt_prsid;
    Poco::ProcessHandle *_phproc;
    Poco::Process::PID _pid;
    int _port;

    InterProcessClient() = delete;
};


}
