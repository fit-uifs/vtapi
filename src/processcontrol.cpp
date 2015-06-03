
#include <common/vtapi_global.h>
#include <common/vtapi_serialize.h>
#include <data/vtapi_processcontrol.h>
//#include "backends/vtapi_backendbase.h"

#define DEF_CONNECT_ATTEMPTS    10
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
static const std::string g_quit       = "quit:";
static const std::string g_connect    = "connect:";
static const std::string g_disconnect = "disconnect:";
static const std::string g_ack        = "ack:";

ProcessControl::ProcessControl(const std::string& name)
    : m_server(name), m_client(name)
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

bool ProcessControl::ComBase::runThread()
{
    bool ret = true;

    do {
        if (m_pThread) break;

        try {
            ThreadArgs args(this);

            std::unique_lock<std::mutex> lk(args.m_mtxReady);
            m_pThread = new thread(threadProc, &args);
            args.m_cvReady.wait(lk);
            
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

        try {
            m_pCommandQueue = new message_queue(
                create_only,
                (m_baseName + DEF_COMMAND_POSTFIX).c_str(),
                DEF_MAX_QUEUE_SIZE,
                DEF_MAX_COMMAND_SIZE);
        }
        catch (interprocess_exception &ex) {
            ret = false;
            break;
        }

        m_vctNotifyQueues.resize(DEF_MAX_CLIENTS);
        memset(m_vctNotifyQueues.data(), 0, DEF_MAX_CLIENTS * sizeof(message_queue*));

        ret = runThread();
    } while(0);
    
    if (!ret) close();
    
    return ret;
}

void ProcessControl::Server::close()
{
    if (m_pCommandQueue) {
        m_mtxSend.lock();
        try
        {
            m_pCommandQueue->send(
                g_close.c_str(),
                g_close.length() + 1,
                DEF_QUIT_PRIORITY);
        }
        catch (interprocess_exception &ex) {}
        m_mtxSend.unlock();
    }
        
    waitForThread();
    
    if (m_pCommandQueue) {
        vt_destruct(m_pCommandQueue);
        message_queue::remove((m_baseName + DEF_COMMAND_POSTFIX).c_str());
    }
    
    m_vctNotifyQueues.clear();
    
    m_callback = NULL;
    m_pCallbackContext = NULL;
}

bool ProcessControl::Server::sendNotify(const ProcessState& state)
{
    bool ret = true;
    
    {
        std::unique_lock<std::mutex> lk(m_mtxSend);
        
        string notify = toString(state);
        for (auto queue : m_vctNotifyQueues) {
            try {
                if (queue) {
                    queue->send(
                        notify.c_str(),
                        notify.length() + 1,
                        DEF_COMMAND_PRIORITY);
                }
            }
            catch (interprocess_exception &ex) {
                cerr << ex.what() << endl;
                ret = false;
            }
        }
    }
    
    return ret;
}

void ProcessControl::Server::threadMain(ThreadArgs &args)
{
    bool bQuit = false;
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
                    bQuit = true;
                }
                // new client connected
                else if (g_connect.compare(0, g_connect.length(), buffer, g_connect.length()) == 0) {
                    int slot = atoi(&buffer[g_connect.length()]);
                    if (slot >= 0 && slot < DEF_MAX_CLIENTS) {
                        auto& queue = m_vctNotifyQueues[slot];
                        if (!queue) {
                            queue = new message_queue(
                                open_only,
                                (m_baseName + DEF_NOTIFY_POSTFIX + toString(slot)).c_str());
                            {
                                std::unique_lock<std::mutex> lk(m_mtxSend);
                                string ack = g_ack + buffer;
                                queue->send(ack.c_str(), ack.length()+1, DEF_ACK_PRIORITY);
                            }
                        }
                    }
                }
                // client disconnected
                else if (g_disconnect.compare(0, g_disconnect.length(), buffer, g_disconnect.length()) == 0) {
                    int slot = atoi(&buffer[g_disconnect.length()]);
                    if (slot >= 0 && slot < DEF_MAX_CLIENTS) {
                        auto& queue = m_vctNotifyQueues[slot];
                        if (queue) {
                            {
                                std::unique_lock<std::mutex> lk(m_mtxSend);
                                string ack = g_ack + buffer;
                                queue->send(ack.c_str(), ack.length() + 1, DEF_ACK_PRIORITY);
                            }
                            vt_destruct(queue);
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
            cerr << ex.what() << endl;
            bQuit = true;
        }
        
    } while(!bQuit);
    
    // disconnect all clients
    std::set<int> setPendingAcks;
    {
        std::unique_lock<std::mutex> lk(m_mtxSend);
        
        for (size_t i = 0; i < m_vctNotifyQueues.size(); i++) {
            auto& queue = m_vctNotifyQueues[i];
            if (queue) {
                try {
                    string quit = g_quit + toString(i);
                    queue->send(quit.c_str(), quit.length()+1, DEF_QUIT_PRIORITY);
                    setPendingAcks.insert(i);
                }
                catch (interprocess_exception &ex) {
                    cerr << ex.what() << endl;
                    break;
                }
            }
        }
    }
    
    // collect disconnect ACKs
    while (!setPendingAcks.empty()) {
        string ack_quit = g_ack + g_quit;
        try {
            message_queue::size_type recv_size = 0;
            unsigned int priority = 0;
            
            do {
                m_pCommandQueue->receive(buffer, sizeof (buffer), recv_size, priority);
            } while(priority != DEF_ACK_PRIORITY);
            
            if (recv_size > 0) {
                if (ack_quit.compare(0, ack_quit.length(), buffer, ack_quit.length()) == 0) {
                    int slot = atoi(&buffer[ack_quit.length()]);
                    if (slot >= 0 && slot < DEF_MAX_CLIENTS) {
                        vt_destruct(m_vctNotifyQueues[slot]);
                        setPendingAcks.erase(slot);
                    }
                }
            }
        }
        catch (interprocess_exception &ex) {
            cerr << ex.what() << endl;
            break;
        }
    }
}


////////////////////////////////////////////////
// internal client methods

ProcessControl::Client::Client(const std::string& baseName)
    : ComBase(baseName),
    m_pNotifyQueue(NULL), m_pCommandQueue(NULL),
    m_callback(NULL), m_pCallbackContext(NULL), m_slot(-1)
{ }

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

        // try opening server command queue
        for (int i = 0; i < DEF_CONNECT_ATTEMPTS; i++) {
            try {
                m_pCommandQueue = new message_queue(
                    open_only,
                    (m_baseName + DEF_COMMAND_POSTFIX).c_str());
                if (m_pCommandQueue) break;
            }
            catch (interprocess_exception &ex) {}
            
            std::this_thread::sleep_for(
                std::chrono::milliseconds(connectTimeout / DEF_CONNECT_ATTEMPTS));
        }
        if (!m_pCommandQueue) {
            ret = false;
            break;
        }

        // try to find free client slot
        for (m_slot = 0; m_slot < DEF_MAX_CLIENTS; m_slot++) {
            try {
                m_pNotifyQueue = new message_queue(
                    create_only,
                    (m_baseName + DEF_NOTIFY_POSTFIX + toString(m_slot)).c_str(),
                    DEF_MAX_QUEUE_SIZE,
                    DEF_MAX_NOTIFY_SIZE);
                if (m_pNotifyQueue) break;
            }
            catch (interprocess_exception &ex) {}
        }
        if (!m_pNotifyQueue) {
            ret = false;
            break;
        }

        ret = runThread();
    } while (0);

    if (!ret) close();

    return ret;
}

void ProcessControl::Client::close()
{
    if (m_pNotifyQueue) {
        m_mtxSend.lock();
        try {
            m_pNotifyQueue->send(
                g_close.c_str(),
                g_close.length() + 1,
                DEF_QUIT_PRIORITY);
        }
        catch (interprocess_exception &ex) {
            cerr << ex.what() << endl;
        }
        m_mtxSend.unlock();
    }

    waitForThread();

    if (m_pNotifyQueue) {
        vt_destruct(m_pNotifyQueue);
        message_queue::remove((m_baseName + DEF_NOTIFY_POSTFIX + toString(m_slot)).c_str());
    }

    vt_destruct(m_pCommandQueue);

    m_callback = NULL;
    m_pCallbackContext = NULL;
    m_slot = -1;
}

bool ProcessControl::Client::sendCommand(COMMAND_T command)
{
    bool ret = true;

    {
        std::unique_lock<std::mutex> lk(m_mtxSend);

        string cmd = toCommandString(command);
        try {
            m_pCommandQueue->send(
                cmd.c_str(),
                cmd.length() + 1,
                DEF_COMMAND_PRIORITY);
        }
        catch (interprocess_exception &ex) {
            cerr << ex.what() << endl;
            ret = false;
        }
    }

    return ret;
}

bool ProcessControl::Client::sendConnect()
{
    bool ret = true;
    
    {
        std::unique_lock<std::mutex> lk(m_mtxSend);
        
        string connect = g_connect + toString(m_slot);
        try {
            m_pCommandQueue->send(
                connect.c_str(),
                connect.length() + 1,
                DEF_COMMAND_PRIORITY);
            
            char buffer[DEF_MAX_NOTIFY_SIZE];
            message_queue::size_type recv_size = 0;
            unsigned int priority = 0;
            m_pNotifyQueue->receive(buffer, sizeof(buffer), recv_size, priority);
            
            if (recv_size > 0) {
                string ack = g_ack + connect;
                ret = (ack.compare(buffer) == 0);
            }
        }
        catch (interprocess_exception &ex) {
            cerr << ex.what() << endl;
            ret = false;
        }
    }

    return ret;
}

bool ProcessControl::Client::sendDisconnect()
{
    bool ret = true;

    {
        std::unique_lock<std::mutex> lk(m_mtxSend);

        string disconnect = g_disconnect + toString(m_slot);
        try {
            m_pCommandQueue->send(
                disconnect.c_str(),
                disconnect.length() + 1,
                DEF_COMMAND_PRIORITY);

            char buffer[DEF_MAX_NOTIFY_SIZE];
            message_queue::size_type recv_size = 0;
            unsigned int priority = 0;
            
            do {
                m_pNotifyQueue->receive(buffer, sizeof (buffer), recv_size, priority);
            } while(priority != DEF_ACK_PRIORITY);

            if (recv_size > 0) {
                string ack = g_ack + disconnect;
                ret = (ack.compare(buffer) == 0);
            }
        }
        catch (interprocess_exception &ex) {
            cerr << ex.what() << endl;
            ret = false;
        }
    }

    return ret;
}

void ProcessControl::Client::threadMain(ThreadArgs &args)
{
    bool bQuit = false;
    char buffer[DEF_MAX_NOTIFY_SIZE];

    args.setReady(sendConnect());
    
    do {
        try
        {
            message_queue::size_type recv_size = 0;
            unsigned int priority = 0;
            m_pNotifyQueue->receive(buffer, sizeof(buffer), recv_size, priority);
            //cout << "client: received " << buffer << endl;

            if (recv_size > 0) {
                if (g_close.compare(buffer) == 0) {
                    sendDisconnect();
                    bQuit = true;
                }
                else if (g_quit.compare(0, g_quit.length(), buffer, g_quit.length()) == 0) {
                    std::unique_lock<std::mutex> lk(m_mtxSend);
                    string ack = g_ack + buffer;
                    m_pCommandQueue->send(ack.c_str(), ack.length() + 1, DEF_ACK_PRIORITY);
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
            cerr << ex.what() << endl;
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
