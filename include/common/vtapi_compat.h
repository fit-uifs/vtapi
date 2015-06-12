/**
 * @file
 * @brief   Multi-platform compatibility layer for VTApi
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef VTAPI_COMPAT_H
#define	VTAPI_COMPAT_H

#include <string>
#include <list>


namespace vtapi {
namespace compat {


int pid();

class ProcessInstance
{
public:
    typedef std::list<std::string> Args;

    ProcessInstance();
    explicit ProcessInstance(const ProcessInstance& orig);
    ~ProcessInstance();
    
    bool launch(const std::string& exec, const Args& args, bool wait);
    bool open(int pid);
    bool isRunning();
    bool isValid();
    void close(bool wait = false);
    
    ProcessInstance& operator=(const ProcessInstance& orig);
    
private:
    union {
        pid_t pid;
        void *ptr;
    } m_handle;
    bool m_bChild;
};


}
}

#endif
