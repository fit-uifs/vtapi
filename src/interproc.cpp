
#include <vtapi/common/global.h>
#include <vtapi/common/compat.h>
#include <vtapi/common/serialize.h>
#include <vtapi/common/interproc.h>


#define DEF_CONNECT_ATTEMPTS    10
#define DEF_SERVER_ALIVE_CHECK_PERIOD_S 2
#define DEF_MAX_CLIENTS         15
#define DEF_MAX_QUEUE_SIZE      100
#define DEF_MAX_COMMAND_SIZE    128
#define DEF_MAX_NOTIFY_SIZE     1024
#define DEF_QUIT_PRIORITY       1
#define DEF_ACK_PRIORITY        2
#define DEF_COMMAND_PRIORITY    3
#define DEF_COMMAND_POSTFIX     "_cmd"
#define DEF_NOTIFY_POSTFIX      "_not"

using namespace std;
using namespace boost::interprocess;

namespace vtapi {


static const struct
{
    ProcessControl::COMMAND_T command;
    const char *str;
}
command_map[] = {
    { ProcessControl::COMMAND_STOP,     "stop" },
    { ProcessControl::COMMAND_RESUME,   "resume" },
    { ProcessControl::COMMAND_SUSPEND,  "suspend" },
    { ProcessControl::COMMAND_NONE,     NULL },
};

static const string g_close      = "close";
static const string g_connect    = "connect:";
static const string g_disconnect = "disconnect:";
static const string g_ack        = "ack:";
static const string g_slot       = "slot";
static const string g_pid        = "pid";



ProcessControl::ProcessControl(int processId)
    : _server(processId), _client(processId)
{ }

ProcessControl::ProcessControl(int processId, const compat::ProcessInstance& serverInstance)
    : _server(processId), _client(processId, serverInstance)
{
    
}

ProcessControl::~ProcessControl()
{
}

////////////////////////////////////////////////
// public client/server methods

bool ProcessControl::server(fServerCallback callback, void *context)
{
    return _server.create(callback, context);
}

bool ProcessControl::client(unsigned int connectTimeout, fClientCallback callback, void *context)
{
    return _client.create(connectTimeout, callback, context);
}

bool ProcessControl::control(COMMAND_T command)
{
    return _client.sendCommand(command);
}

bool ProcessControl::notify(const ProcessState& state)
{
    return _server.sendNotify(state);
}

void ProcessControl::close()
{
    _server.close();
    _client.close();
}

////////////////////////////////////////////////
// internal client/server base class

ProcessControl::ComBase::ComBase(int processId)
    : _processId(processId), _pThread(NULL)
{ }

ProcessControl::ComBase::~ComBase()
{ }

void ProcessControl::ComBase::send(boost::interprocess::message_queue& q, const string& msg, int priority)
{
    unique_lock<mutex> lk(_mtxSend);
    q.send(msg.c_str(), msg.length()+1, priority);
}

bool ProcessControl::ComBase::runThread(unsigned int timeout_ms)
{
    bool ret = true;

    do {
        if (_pThread) break;

        try {
            ThreadArgs args(this);

            unique_lock<mutex> lk(args._mtxReady);
            _pThread = new thread(threadProc, &args);
            if (timeout_ms == 0) {
                args._cvReady.wait(lk);
            }
            else {
                args._cvReady.wait_for(lk, chrono::milliseconds(timeout_ms));
            }
            
            ret = args._bSuccess;
        }
        catch (exception) {
            ret = false;
            break;
        }
    } while(0);

    return ret;
}

void ProcessControl::ComBase::threadProc(ThreadArgs *pArgs)
{
    if (pArgs) pArgs->_pBase->threadMain(*pArgs);
}

void ProcessControl::ComBase::waitForThread()
{
    if (_pThread) {
        _pThread->join();
        vt_destruct(_pThread);
    }
}

ProcessControl::ComBase::ThreadArgs::ThreadArgs(ComBase *pBase)
    : _pBase(pBase)
{}

void ProcessControl::ComBase::ThreadArgs::setReady(bool bSuccess)
{
    unique_lock<mutex> lk(_mtxReady);
    _bSuccess = bSuccess;
    lk.unlock();
    _cvReady.notify_one();
}


////////////////////////////////////////////////
// internal server class

ProcessControl::Server::Server(int processId)
    : ComBase(processId),
    _pCommandQueue(NULL),
    _callback(NULL), _pCallbackContext(NULL)
{ }

ProcessControl::Server::~Server()
{
    close();
}

bool ProcessControl::Server::create(fServerCallback callback, void* context)
{
    bool ret = true;

    do {
        _callback = callback;
        _pCallbackContext = context;

        string qname = toString<int>(_processId) + DEF_COMMAND_POSTFIX;
        try {
            message_queue::remove(qname.c_str());
            _pCommandQueue = new message_queue(
                create_only,
                qname.c_str(),
                DEF_MAX_QUEUE_SIZE,
                DEF_MAX_COMMAND_SIZE);
            
        }
        catch (interprocess_exception &ex) {
            cerr << qname << ':' << ex.what() << endl;
            ret = false;
            break;
        }

        _vctNotifyQueues.resize(DEF_MAX_CLIENTS);
        memset(_vctNotifyQueues.data(), 0, DEF_MAX_CLIENTS * sizeof(message_queue*));

        ret = runThread(0);

    } while(0);
    
    if (!ret) close();
    
    return ret;
}

void ProcessControl::Server::close()
{
    if (_pCommandQueue) {
        try {
            send(*_pCommandQueue, g_close, DEF_QUIT_PRIORITY);
        }
        catch (interprocess_exception &ex) {
            cerr << g_close << ':' << ex.what() << endl;
        }
    }

    waitForThread();
    
    if (_pCommandQueue) {
        vt_destruct(_pCommandQueue);
        string qname = toString<int>(_processId) + DEF_COMMAND_POSTFIX;
        message_queue::remove(qname.c_str());
    }
    
    for (int slot = 0; slot < _vctNotifyQueues.size(); slot++) {
        auto queue = _vctNotifyQueues[slot];
        if (queue) {
            vt_destruct(queue);

            string qname = toString<int>(_processId) + DEF_NOTIFY_POSTFIX + toString<int>(slot);
            message_queue::remove(qname.c_str());
        }
    }
    _vctNotifyQueues.clear();
    
    _callback = NULL;
    _pCallbackContext = NULL;
}

bool ProcessControl::Server::sendNotify(const ProcessState& state)
{
    bool ret = true;
    string notify = toString(state);

    try {
        for (auto queue : _vctNotifyQueues) {
            if (queue) send(*queue, notify, DEF_COMMAND_PRIORITY);
        }
    }
    catch (interprocess_exception &ex) {
        cerr << notify << ':' << ex.what() << endl;
        ret = false;
    }
    
    return ret;
}

void ProcessControl::Server::threadMain(ThreadArgs &args)
{
    bool bQuitPending = false;
    map<int,compat::ProcessInstance> mapActiveClients;

    char buffer[DEF_MAX_COMMAND_SIZE];

    args.setReady(true);
    
    do {
        try {
            message_queue::size_type recv_size = 0;
            unsigned int priority = 0;
            _pCommandQueue->receive(buffer, sizeof(buffer), recv_size, priority);
            //cout << "server: received " << buffer << endl;

            if (recv_size > 0) {
                // close server signal
                if (g_close.compare(buffer) == 0) {
                    if (!bQuitPending) {
                        bQuitPending = true;

                        // try to disconnect all clients
                        for (size_t i = 0; i < _vctNotifyQueues.size(); i++) {
                            auto queue = _vctNotifyQueues[i];
                            if (queue) send(*queue, g_close, DEF_QUIT_PRIORITY);
                        }
                    }
                }
                // new client connected
                else if (g_connect.compare(0, g_connect.length(), buffer, g_connect.length()) == 0) {
                    char *cSlot = strstr(&buffer[g_connect.length()-1], g_slot.c_str());
                    char *cPid = strstr(&buffer[g_connect.length()-1], g_pid.c_str());
                    if (!cSlot || !cPid) continue;
                    
                    int slot = atoi(cSlot + g_slot.length());
                    int pid = atoi(cPid + g_pid.length());

                    if (slot >= 0 && slot < DEF_MAX_CLIENTS && pid > 0) {
                        // client slot should be empty
                        auto& queue = _vctNotifyQueues[slot];
                        if (!queue) {
                            // open client notify queue and send ack:connect
                            string qname = toString<int>(_processId) + DEF_NOTIFY_POSTFIX + toString<int>(slot);
                            queue = new message_queue(open_only, qname.c_str());
                            
                            // save client instance for monitoring
                            compat::ProcessInstance& client = mapActiveClients[slot];
                            if (client.open(pid)) {
                                // send connect ACK
                                string ack = g_ack + buffer;
                                send(*queue, ack, DEF_ACK_PRIORITY);

                                // try to disconnect immediately if quit is pending
                                if (bQuitPending) send(*queue, g_close, DEF_QUIT_PRIORITY);
                            }
                        }
                    }
                }
                // client disconnected
                else if (g_disconnect.compare(0, g_disconnect.length(), buffer, g_disconnect.length()) == 0) {
                    char *cSlot = strstr(&buffer[g_disconnect.length()-1], g_slot.c_str());
                    if (!cSlot) continue;

                    int slot = atoi(cSlot + g_slot.length());
                    
                    if (slot >= 0 && slot < DEF_MAX_CLIENTS) {
                        // client slot should be occupied
                        auto& queue = _vctNotifyQueues[slot];
                        if (queue) {
                            string ack = g_ack + buffer;
                            send(*queue, ack, DEF_ACK_PRIORITY);
                            vt_destruct(queue);

                            string qname = toString<int>(_processId) + DEF_NOTIFY_POSTFIX + toString<int>(slot);
                            message_queue::remove(qname.c_str());

                            mapActiveClients.erase(slot);
                        }
                    }
                }
                // regular command
                else {
                    COMMAND_T cmd = toCommandValue(buffer);
                    if (cmd != COMMAND_NONE && _callback)
                        _callback(cmd, _pCallbackContext);
                }
            }
        }
        catch (interprocess_exception &ex) {
            cerr << "ProcessControl::Server:" << ex.what() << endl;
            if (bQuitPending) {
                break;
            }
            else {
                try {
                    send(*_pCommandQueue, g_close, DEF_QUIT_PRIORITY);
                }
                catch(interprocess_exception &ex) {
                    cerr << g_close << ':' << ex.what() << endl;
                    break;
                }
            }
        }
        
    } while(!bQuitPending || !mapActiveClients.empty());

}


////////////////////////////////////////////////
// internal client methods

ProcessControl::Client::Client(int processId)
    : ComBase(processId),
    _pNotifyQueue(NULL), _pCommandQueue(NULL),
    _callback(NULL), _pCallbackContext(NULL), _slot(-1)
{ }

ProcessControl::Client::Client(int processId, const compat::ProcessInstance& serverInstance)
    : ComBase(processId),
    _pNotifyQueue(NULL), _pCommandQueue(NULL),
    _callback(NULL), _pCallbackContext(NULL), _slot(-1),
    _serverInstance(serverInstance)
{}


ProcessControl::Client::~Client()
{
    close();
}

bool ProcessControl::Client::create(unsigned int connectTimeout, fClientCallback callback, void* context)
{
    bool ret = true;

    do {
        _callback = callback;
        _pCallbackContext = context;

        // if this should be client of specific instance, check it
        if (_serverInstance.isValid()) {
            ret = _serverInstance.isRunning();
            if (!ret) break;
        }
        
        // try opening server command queue
        string qname = toString<int>(_processId) + DEF_COMMAND_POSTFIX;
        string errmsg;
        for (int i = 0; i < DEF_CONNECT_ATTEMPTS; i++) {
            try {
                _pCommandQueue = new message_queue(open_only, qname.c_str());
                if (_pCommandQueue) break;
            }
            catch (interprocess_exception &ex) {
                errmsg = qname + ':' + ex.what();
            }
            
            this_thread::sleep_for(
                chrono::milliseconds(connectTimeout / DEF_CONNECT_ATTEMPTS));
        }
        if (!_pCommandQueue) {
            cerr << errmsg << endl;
            ret = false;
            break;
        }

        // try to find free client slot
        bool remove = false;
        for (_slot = 0; _slot < DEF_MAX_CLIENTS; _slot++) {
            string qname2 = toString<int>(_processId) + DEF_NOTIFY_POSTFIX + toString<int>(_slot);
            try {
                if (remove) message_queue::remove(qname2.c_str());
                _pNotifyQueue = new message_queue(
                    create_only,
                    qname2.c_str(),
                    DEF_MAX_QUEUE_SIZE,
                    DEF_MAX_NOTIFY_SIZE);
                if (_pNotifyQueue) break;
            }
            catch (interprocess_exception &ex) {
                errmsg = qname2 + ':' + ex.what();
            }
            if (_slot == DEF_MAX_CLIENTS - 1) {
                if (!remove) {
                    remove = true;
                    _slot = -1;
                }
            }
        }
        if (!_pNotifyQueue) {
            cerr << errmsg << endl;
            ret = false;
            break;
        }

        ret = runThread(connectTimeout);
    } while (0);

    if (!ret) close();

    return ret;
}

void ProcessControl::Client::close()
{
    if (_pNotifyQueue) {
        try {
            send(*_pNotifyQueue, g_close, DEF_QUIT_PRIORITY);
        }
        catch (interprocess_exception &ex) {
            cerr << g_close << ':' << ex.what() << endl;
        }
    }
    
    waitForThread();

    if (_pNotifyQueue) {
        vt_destruct(_pNotifyQueue);
        string qname2 = toString<int>(_processId) + DEF_NOTIFY_POSTFIX + toString<int>(_slot);
        message_queue::remove(qname2.c_str());
    }

    vt_destruct(_pCommandQueue);
    string qname = toString<int>(_processId) + DEF_COMMAND_POSTFIX;
    //message_queue::remove(qname.c_str());

    _callback = NULL;
    _pCallbackContext = NULL;
    _slot = -1;
}

bool ProcessControl::Client::sendCommand(COMMAND_T command)
{
    bool ret = true;
    string cmd = toCommandString(command);
    
    try {
        send(*_pCommandQueue, cmd, DEF_COMMAND_PRIORITY);
    }
    catch (interprocess_exception &ex) {
        cerr << cmd << ':' << ex.what() << endl;
        ret = false;
    }

    return ret;
}

void ProcessControl::Client::threadMain(ThreadArgs &args)
{
    using namespace boost::posix_time;
    
    bool bQuit = false;
    bool bConnectPending = false;
    bool bDisconnectPending = false;
    string connectAck;
    string disconnectAck;
    char buffer[DEF_MAX_NOTIFY_SIZE];

    string connect = g_connect + g_slot + toString<int>(_slot) + g_pid + toString<int>(compat::pid());
    try {
        send(*_pCommandQueue, connect, DEF_COMMAND_PRIORITY);
        
        connectAck = g_ack + connect;
        bConnectPending = true;
    }
    catch(interprocess_exception &ex) {
        cerr << connect << ':' << ex.what() << endl;
        return;
    }

    ptime next_check = second_clock::universal_time() + seconds(DEF_SERVER_ALIVE_CHECK_PERIOD_S);

    do {
        try
        {
            message_queue::size_type recv_size = 0;
            unsigned int priority = 0;
            
            // check queue and if server is still running
            if (!_pNotifyQueue->timed_receive(buffer, sizeof(buffer), recv_size, priority, next_check)) {
                if (_serverInstance.isValid() && !_serverInstance.isRunning()) {
                    ProcessState state(ProcessState::STATUS_ERROR, 0, "Server exited unexpectedly");
                    if (_callback) _callback(state, _pCallbackContext);
                    break;
                }
                next_check = second_clock::universal_time() + seconds(DEF_SERVER_ALIVE_CHECK_PERIOD_S);
            }
            else {
                //cout << "client: received " << buffer << endl;
            }

            if (recv_size > 0) {
                if (g_close.compare(buffer) == 0) {
                    if (!bDisconnectPending) {
                        string disconnect = g_disconnect + g_slot + toString<int>(_slot);
                        send(*_pCommandQueue, disconnect, DEF_COMMAND_PRIORITY);
                        
                        disconnectAck = g_ack + disconnect;
                        bDisconnectPending = true;
                    }
                }
                else if (bConnectPending && connectAck.compare(buffer) == 0) {
                    bConnectPending = false;
                    args.setReady(true);
                }
                else if (bDisconnectPending && disconnectAck.compare(buffer) == 0) {
                    bQuit = true;
                }
                else {
                    ProcessState state(buffer);
                    if (state.status != ProcessState::STATUS_NONE && _callback)
                        _callback(state, _pCallbackContext);
                }
            }
        }
        catch (interprocess_exception &ex) {
            cerr << "ProcessControl::Client:" << ex.what() << endl;
            bQuit = true;
        }
        
    } while(!bQuit);
}


//////////////////////////////
// convert methods

ProcessControl::COMMAND_T ProcessControl::toCommandValue(const string& command_string)
{
    COMMAND_T command = COMMAND_NONE;

    for (int i = 0; command_map[i].str; i++) {
        if (command_string.compare(command_map[i].str) == 0) {
            command = command_map[i].command;
            break;
        }
    }

    return command;
}

string ProcessControl::toCommandString(COMMAND_T command)
{
    string command_string;

    for (int i = 0; command_map[i].str; i++) {
        if (command == command_map[i].command) {
            command_string = command_map[i].str;
            break;
        }
    }

    return command_string;
}

}
