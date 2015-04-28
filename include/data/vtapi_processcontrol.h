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
        COMMAND_RESUME,     /**< resume suspended process */
        COMMAND_SUSPEND,    /**< pause running process */
        COMMAND_STOP        /**< stop running process */
    } COMMAND_T;

    // Registerable callback type on server side, notifies on control command
    typedef void (*fServerCallback)(COMMAND_T command, void *context);

    // Registerable callback type on client side, notifies on process state update
    typedef void (*fClientCallback)(const ProcessState& state, void *context);
    
public:
    ProcessControl(const std::string& name);
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
     * Closes server/client connection end
     * @return success
     */
    bool close();

    static COMMAND_T toCommandValue(const std::string& command_string);
    static std::string toCommandString(COMMAND_T command);
    
protected:
    std::string processName;
} ;

}

#endif	/* VTAPI_PROCESSCONTROL_H */
