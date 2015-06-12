
#include <common/vtapi_global.h>
#include <common/vtapi_serialize.h>
#include <data/vtapi_processcontrol.h>
//#include "backends/vtapi_backendbase.h"

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

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::thread;
using namespace boost::interprocess;

using namespace vtapi;

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

static const std::string g_close      = "close";
static const std::string g_connect    = "connect:";
static const std::string g_disconnect = "disconnect:";
static const std::string g_ack        = "ack:";
static const std::string g_slot       = "slot";
static const std::string g_pid        = "pid";



ProcessControl::ProcessControl(const std::string& processName)
    : m_server(processName), m_client(processName)
{ }

ProcessControl::ProcessControl(const std::string& processName, const compat::ProcessInstance& serverInstance)
    : m_server(processName), m_client(processName, serverInstance)
{
    
}

ProcessControl::~ProcessControl()
{
}

////////////////////////////////////////////////
// public client/server methods

bool ProcessControl::server(fServerCallback callback, void *context)
{
    return m_server.create(callback, context);
}

bool ProcessControl::client(unsigned int connectTimeout, fClientCallback callback, void *context)
{
    return m_client.create(connectTimeout, callback, context);
}

bool ProcessControl::control(COMMAND_T command)
{
    return m_client.sendCommand(command);
}

bool ProcessControl::notify(const ProcessState& state)
{
    return m_server.sendNotify(state);
}

void ProcessControl::close()
{
    m_server.close();
    m_client.close();
}

////////////////////////////////////////////////
// internal client/server base class

ProcessControl::ComBase::ComBase(const std::string& baseName)
    : m_baseName(baseName), m_pThread(NULL)
{ }

ProcessControl::ComBase::~ComBase()
{ }

void ProcessControl::ComBase::send(boost::interprocess::message_queue& q, const std::string& msg, int priority)
{
    std::unique_lock<std::mutex> lk(m_mtxSend);
    q.send(msg.c_str(), msg.length()+1, priority);
}

bool ProcessControl::ComBase::runThread(unsigned int timeout_ms)
{
    bool ret = true;

    do {
        if (m_pThread) break;

        try {
            ThreadArgs args(this);

            std::unique_lock<std::mutex> lk(args.m_mtxReady);
            m_pThread = new thread(threadProc, &args);
            if (timeout_ms == 0) {
                args.m_cvReady.wait(lk);
            }
            else {
                args.m_cvReady.wait_for(lk, std::chrono::milliseconds(timeout_ms));
            }
            
            ret = args.m_bSuccess;
        }
        catch (std::exception) {
            ret = false;
            break;
        }
    } while(0);

    return ret;
}

void ProcessControl::ComBase::threadProc(ThreadArgs *pArgs)
{
    if (pArgs) pArgs->m_pBase->threadMain(*pArgs);
}

void ProcessControl::ComBase::waitForThread()
{
    if (m_pThread) {
        m_pThread->join();
        vt_destruct(m_pThread);
    }
}

ProcessControl::ComBase::ThreadArgs::ThreadArgs(ComBase *pBase)
    : m_pBase(pBase)
{}

void ProcessControl::ComBase::ThreadArgs::setReady(bool bSuccess)
{
    std::unique_lock<std::mutex> lk(m_mtxReady);
    m_bSuccess = bSuccess;
    lk.unlock();
    m_cvReady.notify_one();
}


////////////////////////////////////////////////
// internal server class

ProcessControl::Server::Server(const std::string& processName)
    : ComBase(processName),
    m_pCommandQueue(NULL),
    m_callback(NULL), m_pCallbackContext(NULL)
{ }

ProcessControl::Server::~Server()
{
    close();
}

bool ProcessControl::Server::create(fServerCallback callback, void* context)
{
    bool ret = true;

    do {
        m_callback = callback;
        m_pCallbackContext = context;

        string qname = m_baseName + DEF_COMMAND_POSTFIX;
        try {
            m_pCommandQueue = new message_queue(
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

        m_vctNotifyQueues.resize(DEF_MAX_CLIENTS);
        memset(m_vctNotifyQueues.data(), 0, DEF_MAX_CLIENTS * sizeof(message_queue*));

        ret = runThread(0);

    } while(0);
    
    if (!ret) close();
    
    return ret;
}

void ProcessControl::Server::close()
{
    if (m_pCommandQueue) {
        try {
            send(*m_pCommandQueue, g_close, DEF_QUIT_PRIORITY);
        }
        catch (interprocess_exception &ex) {
            cerr << g_close << ':' << ex.what() << endl;
        }
    }

    waitForThread();
    
    if (m_pCommandQueue) {
        vt_destruct(m_pCommandQueue);
        string qname = m_baseName + DEF_COMMAND_POSTFIX;
        message_queue::remove(qname.c_str());
    }
    
    for (int slot = 0; slot < m_vctNotifyQueues.size(); slot++) {
        if (m_vctNotifyQueues[slot]) {
            string qname = m_baseName + DEF_NOTIFY_POSTFIX + toString(slot);
            message_queue::remove(qname.c_str());
        }
    }
    m_vctNotifyQueues.clear();
    
    m_callback = NULL;
    m_pCallbackContext = NULL;
}

bool ProcessControl::Server::sendNotify(const ProcessState& state)
{
    bool ret = true;
    string notify = toString(state);

    try {
        for (auto queue : m_vctNotifyQueues) {
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
    std::map<int,compat::ProcessInstance> mapActiveClients;

    char buffer[DEF_MAX_COMMAND_SIZE];

    args.setReady(true);
    
    do {
        try {
            message_queue::size_type recv_size = 0;
            unsigned int priority = 0;
            m_pCommandQueue->receive(buffer, sizeof(buffer), recv_size, priority);
            //cout << "server: received " << buffer << endl;

            if (recv_size > 0) {
                // close server signal
                if (g_close.compare(buffer) == 0) {
                    if (!bQuitPending) {
                        bQuitPending = true;

                        // try to disconnect all clients
                        for (size_t i = 0; i < m_vctNotifyQueues.size(); i++) {
                            auto queue = m_vctNotifyQueues[i];
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
                        auto& queue = m_vctNotifyQueues[slot];
                        if (!queue) {
                            // open client notify queue and send ack:connect
                            string qname = m_baseName + DEF_NOTIFY_POSTFIX + toString(slot);
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
                        auto& queue = m_vctNotifyQueues[slot];
                        if (queue) {
                            string ack = g_ack + buffer;
                            send(*queue, ack, DEF_ACK_PRIORITY);
                            vt_destruct(queue);

                            string qname = m_baseName + DEF_NOTIFY_POSTFIX + toString(slot);
                            message_queue::remove(qname.c_str());

                            mapActiveClients.erase(slot);
                        }
                    }
                }
                // regular command
                else {
                    COMMAND_T cmd = toCommandValue(buffer);
                    if (cmd != COMMAND_NONE && m_callback)
                        m_callback(cmd, m_pCallbackContext);
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
                    send(*m_pCommandQueue, g_close, DEF_QUIT_PRIORITY);
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

ProcessControl::Client::Client(const std::string& baseName)
    : ComBase(baseName),
    m_pNotifyQueue(NULL), m_pCommandQueue(NULL),
    m_callback(NULL), m_pCallbackContext(NULL), m_slot(-1)
{ }

ProcessControl::Client::Client(const std::string& baseName, const compat::ProcessInstance& serverInstance)
    : ComBase(baseName),
    m_pNotifyQueue(NULL), m_pCommandQueue(NULL),
    m_callback(NULL), m_pCallbackContext(NULL), m_slot(-1),
    m_serverInstance(serverInstance)
{}


ProcessControl::Client::~Client()
{
    close();
}

bool ProcessControl::Client::create(unsigned int connectTimeout, fClientCallback callback, void* context)
{
    bool ret = true;

    do {
        m_callback = callback;
        m_pCallbackContext = context;

        // if this should be client of specific instance, check it
        if (m_serverInstance.isValid()) {
            ret = m_serverInstance.isRunning();
            if (!ret) break;
        }
        
        // try opening server command queue
        string qname = m_baseName + DEF_COMMAND_POSTFIX;
        std::stringstream ssError;
        for (int i = 0; i < DEF_CONNECT_ATTEMPTS; i++) {
            try {
                m_pCommandQueue = new message_queue(open_only, qname.c_str());
                if (m_pCommandQueue) break;
            }
            catch (interprocess_exception &ex) {
                ssError << qname << ':' << ex.what() << endl;
            }
            
            std::this_thread::sleep_for(
                std::chrono::milliseconds(connectTimeout / DEF_CONNECT_ATTEMPTS));
        }
        if (!m_pCommandQueue) {
            cerr << ssError.str();
            ret = false;
            break;
        }

        // try to find free client slot
        std::stringstream ssError2;
        for (m_slot = 0; m_slot < DEF_MAX_CLIENTS; m_slot++) {
            string qname2 = m_baseName + DEF_NOTIFY_POSTFIX + toString(m_slot);
            try {
                m_pNotifyQueue = new message_queue(
                    create_only,
                    qname2.c_str(),
                    DEF_MAX_QUEUE_SIZE,
                    DEF_MAX_NOTIFY_SIZE);
                if (m_pNotifyQueue) break;
            }
            catch (interprocess_exception &ex) {
                ssError2 << qname2 << ':' << ex.what() << endl;
            }
        }
        if (!m_pNotifyQueue) {
            cerr << ssError2.str();
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
    if (m_pNotifyQueue) {
        try {
            send(*m_pNotifyQueue, g_close, DEF_QUIT_PRIORITY);
        }
        catch (interprocess_exception &ex) {
            cerr << g_close << ':' << ex.what() << endl;
        }
    }
    
    waitForThread();

    if (m_pNotifyQueue) {
        vt_destruct(m_pNotifyQueue);
        string qname2 = m_baseName + DEF_NOTIFY_POSTFIX + toString(m_slot);
        message_queue::remove(qname2.c_str());
    }

    vt_destruct(m_pCommandQueue);
    string qname = m_baseName + DEF_COMMAND_POSTFIX;
    message_queue::remove(qname.c_str());

    m_callback = NULL;
    m_pCallbackContext = NULL;
    m_slot = -1;
}

bool ProcessControl::Client::sendCommand(COMMAND_T command)
{
    bool ret = true;
    string cmd = toCommandString(command);
    
    try {
        send(*m_pCommandQueue, cmd, DEF_COMMAND_PRIORITY);
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

    string connect = g_connect + g_slot + toString(m_slot) + g_pid + toString(compat::pid());
    try {
        send(*m_pCommandQueue, connect, DEF_COMMAND_PRIORITY);
        
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
            if (!m_pNotifyQueue->timed_receive(buffer, sizeof(buffer), recv_size, priority, next_check)) {
                if (m_serverInstance.isValid() && !m_serverInstance.isRunning())
                    break;
                next_check = second_clock::universal_time() + seconds(DEF_SERVER_ALIVE_CHECK_PERIOD_S);
            }
            else {
                //cout << "client: received " << buffer << endl;
            }

            if (recv_size > 0) {
                if (g_close.compare(buffer) == 0) {
                    if (!bDisconnectPending) {
                        string disconnect = g_disconnect + g_slot + toString(m_slot);
                        send(*m_pCommandQueue, disconnect, DEF_COMMAND_PRIORITY);
                        
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
                    if (state.status != ProcessState::STATUS_NONE && m_callback)
                        m_callback(state, m_pCallbackContext);
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

ProcessControl::COMMAND_T ProcessControl::toCommandValue(const std::string& command_string)
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

std::string ProcessControl::toCommandString(COMMAND_T command)
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
