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

#ifndef VTAPI_PROCESSCONTROL_H
#define	VTAPI_PROCESSCONTROL_H

#include "../common/vtapi_compat.h"
#include "vtapi_processstate.h"

namespace vtapi {

/**
 * @brief
 * 
 * Class encapsulation launcher<->client process connection and control commands
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
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
    
public:
    ProcessControl(const std::string& processName);
    ProcessControl(const std::string& processName, const compat::ProcessInstance& instance);
    virtual ~ProcessControl();

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
        explicit ComBase(const std::string& baseName);
        virtual ~ComBase();
        
    protected:
        class ThreadArgs
        {
        public:
            ThreadArgs(ComBase *pBase);
            void setReady(bool bSuccess);

        private:            
            ComBase *m_pBase;
            std::mutex m_mtxReady;
            std::condition_variable m_cvReady;
            bool m_bSuccess;
            
            friend class ComBase;
        };
        
        void send(boost::interprocess::message_queue &q, const std::string& msg, int priority);
        bool runThread(unsigned int timeout_ms);
        void waitForThread();
        static void threadProc(ThreadArgs *pArgs);
        virtual void threadMain(ThreadArgs &args) = 0;
        
        std::mutex m_mtxSend;
        std::string m_baseName;
        
    private:
        std::thread *m_pThread;
    };
    
    class Server : public ComBase
    {
    public:
        Server(const std::string& baseName);
        virtual ~Server();

        bool create(fServerCallback callback, void *context);
        void close();
        bool sendNotify(const ProcessState& state);

    protected:
        void threadMain(ThreadArgs &args);

    private:
        boost::interprocess::message_queue *m_pCommandQueue;
        std::vector<boost::interprocess::message_queue*> m_vctNotifyQueues;
        fServerCallback m_callback;
        void *m_pCallbackContext;
    };
    
    
    class Client : public ComBase
    {
    public:
        explicit Client(const std::string& baseName);
        Client(const std::string& baseName, const compat::ProcessInstance& serverInstance);
        virtual ~Client();

        bool create(unsigned int connectTimeout, fClientCallback callback, void *context);
        void close();
        bool sendCommand(COMMAND_T command);

    protected:
        void threadMain(ThreadArgs &args);
        
    private:
        int m_slot;
        boost::interprocess::message_queue *m_pCommandQueue;
        boost::interprocess::message_queue *m_pNotifyQueue;
        fClientCallback m_callback;
        void *m_pCallbackContext;
        compat::ProcessInstance m_serverInstance;
    };
    
    
    Server m_server;
    Client m_client;
} ;

}

#endif	/* VTAPI_PROCESSCONTROL_H */
