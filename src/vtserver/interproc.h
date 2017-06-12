#pragma once

#include <vtapi/vtapi.h>
#include <list>
#include <thread>
#include <mutex>

namespace vtserver {


class Interproc
{
public:
    Interproc();
    ~Interproc();

    void addClientInstance(vtapi::InterProcessClient *ipc);

private:
    const int _check_period_ms = 250;
    const int _shutdown_timeout_ms = 5000;

    std::thread _thread;
    std::mutex _mtx;
    std::atomic_bool _stop;
    std::list<vtapi::InterProcessClient*> _clients;


    void threadLoop();
    void checkClients();
};


}
