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
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <boost/interprocess/ipc/message_queue.hpp>
#include "compat.h"
#include "../data/processstate.h"

namespace vtapi {

/**
 * @brief
 *
 * Class encapsulation launcher<->client process connection and control commands
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 *
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class ProcessControl
{
public:
    // Possible commands to send to server process
    typedef enum _COMMAND_T
    {
        COMMAND_NONE,       /**< invalid command */
        COMMAND_STOP,       /**< stop running process */
        COMMAND_RESUME,     /**< resume suspended process */
        COMMAND_SUSPEND     /**< pause running process */
    } COMMAND_T;

    // Registerable callback type on server side, notifies on control command
    typedef void (*fServerCallback)(COMMAND_T command, void *context);

    // Registerable callback type on client side, notifies on process state update
    typedef void (*fClientCallback)(const ProcessState& state, void *context);

    /**
     * Constructs inter-process communication object
     * @param processId ID of the connected process
     */
    explicit ProcessControl(int processId);

    /**
     * Constructs inter-process communication object. Specifies server instance
     * to which this client will try to connect.
     * @param processId ID of the connected process
     * @param instance server instance which will be checked during communication if it's alive
     */
    ProcessControl(int processId, const compat::ProcessInstance& instance);

    /**
     * Destructor
     */
    ~ProcessControl();

    /**
     * Starts server communication end.
     * Server is process instance which is doing the work.
     * It listens on server port to commands (resume, pause, stop) and distributes
     * notifications about its state change to connected clients
     * @param callback callback function to call on control command
     * @param context context to supply with callback
     * @return successful bind
     */
    bool server(fServerCallback callback = NULL, void *context = NULL);

    /**
     * Starts client communication end.
     * Client is non-working instance sending control messages to server
     * and listening for notifications about server's state change.
     * @param connectTimeout for how long to attempt connecting to server [ms]
     * @param callback callback function to call on state change
     * @param context context to supply with callback
     * @return successful connection
     */
    bool client(unsigned int connectTimeout = 2500, fClientCallback callback = NULL, void *context = NULL);

    /**
     * Sends control command to server process.
     * You should rather use process->controlXXX()
     * @param command command
     * @return succesful send
     */
    bool control(COMMAND_T command);

    /**
     * Sends state change notification to client process.
     * You should rather use process->updateStateXXX()
     * @param state updated process state
     * @return success
     */
    bool notify(const ProcessState& state);

    /**
     * Closes server/client connection
     */
    void close();

    static COMMAND_T toCommandValue(const std::string& command_string);
    static std::string toCommandString(COMMAND_T command);

private:
    class ComBase
    {
    public:
        explicit ComBase(int processId);
        virtual ~ComBase();

    protected:
        class ThreadArgs
        {
        public:
            ThreadArgs(ComBase *pBase);
            void setReady(bool bSuccess);

        private:
            ComBase *_pBase;
            std::mutex _mtxReady;
            std::condition_variable _cvReady;
            bool _bSuccess;

            friend class ComBase;
        };

        void send(boost::interprocess::message_queue &q, const std::string& msg, int priority);
        bool runThread(unsigned int timeout_ms);
        void waitForThread();
        static void threadProc(ThreadArgs *pArgs);
        virtual void threadMain(ThreadArgs &args) = 0;

        std::mutex _mtxSend;
        int _processId;

    private:
        std::thread *_pThread;
    };

    class Server : public ComBase
    {
    public:
        Server(int processId);
        virtual ~Server();

        bool create(fServerCallback callback, void *context);
        void close();
        bool sendNotify(const ProcessState& state);

    protected:
        void threadMain(ThreadArgs &args);

    private:
        boost::interprocess::message_queue *_pCommandQueue;
        std::vector<boost::interprocess::message_queue*> _vctNotifyQueues;
        fServerCallback _callback;
        void *_pCallbackContext;
    };


    class Client : public ComBase
    {
    public:
        explicit Client(int processId);
        Client(int processId, const compat::ProcessInstance& serverInstance);
        virtual ~Client();

        bool create(unsigned int connectTimeout, fClientCallback callback, void *context);
        void close();
        bool sendCommand(COMMAND_T command);

    protected:
        void threadMain(ThreadArgs &args);

    private:
        int _slot;
        boost::interprocess::message_queue *_pCommandQueue;
        boost::interprocess::message_queue *_pNotifyQueue;
        fClientCallback _callback;
        void *_pCallbackContext;
        compat::ProcessInstance _serverInstance;
    };


    Server _server;
    Client _client;
};

}
