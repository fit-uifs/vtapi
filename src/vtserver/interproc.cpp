#include "interproc.h"
#include <chrono>

namespace vtserver {


Interproc::Interproc()
    : _stop(false)
{
    _thread = std::thread(&Interproc::threadLoop, this);

}

Interproc::~Interproc()
{
    _stop = true;
    _thread.join();

    // try stopping clients gently
    for (auto ipc : _clients)
        ipc->stop();

    // give clients time to exit
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    while (!_clients.empty()) {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now-start) >= std::chrono::milliseconds(_shutdown_timeout_ms))
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(_check_period_ms));
        checkClients();
    }

    // kill rest with fire
    for (auto ipc : _clients) {
        ipc->kill();
        delete ipc;
    }
}

void Interproc::addClientInstance(vtapi::InterProcessClient *ipc)
{
    std::lock_guard<std::mutex> lk(_mtx);
    _clients.push_back(ipc);
}

void Interproc::threadLoop()
{
    // periodically check child processes if they exist
    while(!_stop) {
        {
            std::lock_guard<std::mutex> lk(_mtx);
            checkClients();
        }
        if(!_stop)
            std::this_thread::sleep_for(std::chrono::milliseconds(_check_period_ms));
    }
}

void Interproc::checkClients()
{
    for (auto it = _clients.begin(); it != _clients.end();) {
        if (!(*it)->isRunning()) {
            (*it)->wait();
            delete *it;
            auto it2 = it++;
            _clients.erase(it2);
        }
        else {
            it++;
        }
    }
}


}
