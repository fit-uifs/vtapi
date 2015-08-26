
#include <csignal>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/ServerSocket.h>
#include <vtapi/common/global.h>
#include <vtapi/data/process.h>
#include <vtapi/common/interproc.h>

static const int def_server_port = 32684;

#define DEF_CONNECT_ATTEMPTS    10
#define DEF_SERVER_ALIVE_CHECK_PERIOD_S 2
#define DEF_MAX_CLIENTS         15

using namespace std;

namespace vtapi {


InterProcessBase::InterProcessBase(const Process &process)
    : _pprocess(new Process(process))
{
}

InterProcessBase::~InterProcessBase()
{
    vt_destruct(_pprocess);
}


InterProcessServer::fnCommandCallback InterProcessServer::_callback(NULL);
std::atomic_bool InterProcessServer::_signals_installed(false);

void InterProcessServer::sighandler(int sig)
{
    switch (sig)
    {
    case SIGINT:
    case SIGTERM:
        _callback(CommandStop);
        break;
    default:
        break;
    }
}


InterProcessServer::InterProcessServer(const Process & process, fnCommandCallback callback)
    : InterProcessBase(process), _stop_server(false), _server_port(0)
{
}

InterProcessServer::~InterProcessServer()
{
    uninstallSignalHandlers();
    stopListening();
}

bool InterProcessServer::installSignalHandlers()
{
    if (!_signals_installed) {
        std::signal(SIGINT, InterProcessServer::sighandler);
        std::signal(SIGTERM, InterProcessServer::sighandler);
        _signals_installed = true;

        return true;
    }
    else {
        return false;
    }
}

void InterProcessServer::uninstallSignalHandlers()
{
    if (_signals_installed) {
        std::signal(SIGINT, SIG_DFL);
        std::signal(SIGTERM, SIG_DFL);
        _signals_installed = false;
    }
}

int InterProcessServer::startListening()
{
    if (!_server_thread.joinable()) {
        std::unique_lock<std::mutex> lk(_server_ready_mtx);
        _server_port = 0;
        _server_thread = std::thread(serverProc, this);
        _server_ready_cv.wait(lk);
        if (_server_port == 0) {
            _server_thread.join();
        }
        else {
            return _server_port;
        }
    }

    return 0;
}

void InterProcessServer::stopListening()
{
    if (_server_thread.joinable()) {
        _stop_server = true;
        _server_thread.join();
    }
}

void InterProcessServer::serverProc(InterProcessServer *context)
{
    context->serverLoop();
}

void InterProcessServer::serverLoop()
{
    Poco::Net::ServerSocket srv;

    {
        std::lock_guard<std::mutex> lk(_server_ready_mtx);
        int port = def_server_port;

        do {
            try
            {
                srv.bind(Poco::Net::SocketAddress("127.0.0.1", port));
                _server_port = port;
            }
            catch(...)
            {
                port++;
            }
        } while (_server_port == 0);
    }

    srv.listen();

    _server_ready_cv.notify_all();

//    for (;;) {
//        Poco::Net::Socket::SocketList readList, writeList, exceptList;
//        readList.push_back(srv);

//        if (srv.select(readList, writeList, exceptList, Poco::Timespan(0,200*1000)) > 0) {
//            srv.acceptConnection()
//        }
//        else {

//        }


//    }


    //    Poco::Net::ServerSocket srv(8080); // does bind + listen
    //    for (;;)
    //    {
    //    Poco::Net::StreamSocket ss = srv.acceptConnection();
    //    Poco::Net::SocketStream str(ss);
    //    str << "HTTP/1.0 200 OK\r\n"
    //           "Content-Type: text/html\r\n"
    //           "\r\n"
    //           "<html><head><title>My 1st Web Server</title></head>"
    //           "<body><h1>Hello, world!</h1></body></html>"
    //        << std::flush;
    //    }
}


InterProcessClient::InterProcessClient(const Process & process, Poco::ProcessHandle hproc)
    : InterProcessBase(process), _phproc(new Poco::ProcessHandle(hproc))
{
}

InterProcessClient::InterProcessClient(const Process & process)
    : InterProcessBase(process), _phproc(NULL)
{
}

InterProcessClient::~InterProcessClient()
{
    vt_destruct(_phproc);
}

bool InterProcessClient::isRunning()
{
    if (_phproc) {
        return Poco::Process::isRunning(*_phproc);
    }
    else {
        int pid = _pprocess->getInstancePID();
        if (pid > 0) {
            return Poco::Process::isRunning(pid);
        }
    }

    return false;
}

bool InterProcessClient::suspend()
{
    // via net

    //    Poco::Net::SocketAddress sa("www.appinf.com", 80);
    //    Poco::Net::StreamSocket socket(sa)
    //    Poco::Net::SocketStream str(socket);
    //    str << "GET / HTTP/1.1\r\n"
    //           "Host: www.appinf.com\r\n"
    //           "\r\n";
    //    str.flush();

    return false;
}

bool InterProcessClient::resume()
{
    // via net

    return false;
}

bool InterProcessClient::stop()
{
    // via net

    return false;
}

bool InterProcessClient::kill()
{
    if (_phproc) {
        Poco::Process::kill(*_phproc);
        return true;
    }
    else {
        int pid = _pprocess->getInstancePID();
        if (pid > 0) {
            Poco::Process::kill(pid);
            return true;
        }
    }

    return false;
}

bool InterProcessClient::wait()
{
    if (_phproc) {
        Poco::Process::wait(*_phproc);
        return true;
    }

    return false;
}




}
