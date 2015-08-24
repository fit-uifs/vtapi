
#include <vtapi/common/global.h>
#include <vtapi/data/process.h>
#include <vtapi/common/interproc.h>


#define DEF_CONNECT_ATTEMPTS    10
#define DEF_SERVER_ALIVE_CHECK_PERIOD_S 2
#define DEF_MAX_CLIENTS         15

using namespace std;

namespace vtapi {

InterProcessServer::InterProcessServer(int vt_prsid)
    : _vt_prsid(vt_prsid), _signals_installed(false), _port(0)
{
}

InterProcessServer::~InterProcessServer()
{
    uninstallSignalHandlers();
    stopListening();
}

bool InterProcessServer::installSignalHandlers(InterProcessServer::fnSignalCallback *callback)
{
    return true;
}

void InterProcessServer::uninstallSignalHandlers()
{

}

int InterProcessServer::startListening()
{
    return 0;
}

void InterProcessServer::stopListening()
{

}

int InterProcessServer::getListeningPort()
{
    return _port;
}

InterProcessClient::InterProcessClient(int vt_prsid, Poco::ProcessHandle hproc, int port)
    : _vt_prsid(vt_prsid), _phproc(new Poco::ProcessHandle(hproc)), _pid(hproc.id()), _port(port)
{
}

InterProcessClient::InterProcessClient(int vt_prsid, int pid, int port)
    : _vt_prsid(vt_prsid), _phproc(NULL), _pid(pid), _port(port)
{
}

InterProcessClient::~InterProcessClient()
{
    vt_destruct(_phproc);
}

bool InterProcessClient::isRunning()
{
    return true;
}

bool InterProcessClient::suspend()
{
    return true;
}

bool InterProcessClient::resume()
{
    return true;
}

bool InterProcessClient::stop()
{
    return true;
}

bool InterProcessClient::kill()
{
    return true;
}

bool InterProcessClient::wait()
{
    return true;
}


}
