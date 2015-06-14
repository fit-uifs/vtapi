
#include <vtapi_global.h>
#include <common/vtapi_compat.h>

#if defined(_WIN32)
#   define VTAPI_WIN
#elif defined(__unix__) || defined(__unix) || defined(__CYGWIN__) || (defined(__APPLE__) && defined(__MACH__))
#   include <unistd.h>
#   if defined(_POSIX_VERSION)
#       define VTAPI_POSIX
#   endif
#endif

#if defined(VTAPI_POSIX)

#include <signal.h>
#include <wait.h>
#include <unistd.h>

#elif defined(VTAPI_WIN)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#else
#error "VTApi is incompatible with this platform"
#endif

using namespace std;

namespace vtapi {
namespace compat {


///////////////////////////////////////////////////////////////
#if defined(VTAPI_POSIX)
///////////////////////////////////////////////////////////////

int pid()
{
    return getpid();
}


ProcessInstance::ProcessInstance()
{
    m_handle.pid = 0;
}

ProcessInstance::ProcessInstance(const ProcessInstance& orig)
{
    m_handle.pid = orig.m_handle.pid;
    m_bChild = orig.m_bChild;
}

ProcessInstance::~ProcessInstance()
{
    close();
}

bool ProcessInstance::launch(const string& exec, const Args& args, bool wait)
{
    if (isValid() || exec.empty())
        return false;
    
    pid_t cpid = fork();
    
    if (cpid == 0) {
        const char ** argv = new const char *[args.size() + 2];
        size_t i = 0;

        argv[i++] = exec.c_str();
        for (auto& arg : args) {
            argv[i++] = arg.c_str();
        }
        argv[i] = NULL;
        
        execv(argv[0], (char **)argv);
        exit(1);
    }
    else if (cpid > 0) {
        m_bChild = true;
        m_handle.pid = cpid;
        if (wait) {
            int status = 0;
            if (waitpid(cpid, &status, 0) == -1)
                return false;
        }
    }
    else {
        return false;
    }

    return true;
}

bool ProcessInstance::open(int pid)
{
    if (isValid() || pid == 0) {
        return false;
    }
    else {
        m_bChild = false;
        m_handle.pid = pid;
        if (!isRunning()) {
            close();
            return false;
        }
        return true;
    }
}

bool ProcessInstance::isRunning()
{
    if (isValid()) {
        if (m_bChild) {
            int status = 0;
            return (waitpid(m_handle.pid, &status, WNOHANG) != m_handle.pid);
        }
        else {
            return (kill(m_handle.pid, 0) == 0);
        }
    }
    else {
        return false;
    }
}

bool ProcessInstance::isValid()
{
    return (m_handle.pid > 0);
}

void ProcessInstance::close(bool wait)
{
    if (isValid()) {
        if (m_bChild) {
            int status = 0;
            waitpid(m_handle.pid, &status, wait ? 0 : WNOHANG);
        }
        else if (wait) {
            while (kill(m_handle.pid, 0) == 0)
                sleep(1);
        }
        m_handle.pid = 0;
    }
}

ProcessInstance& ProcessInstance::operator=(const ProcessInstance& orig)
{
    m_handle.pid = orig.m_handle.pid;
    m_bChild = orig.m_bChild;

    return *this;
}

///////////////////////////////////////////////////////////////
#elif defined(VTAPI_WIN)
///////////////////////////////////////////////////////////////

int pid()
{
    return (int)GetProcessId(NULL);
}

ProcessInstance::ProcessInstance()
{
    m_handle.ptr = NULL;
}

ProcessInstance::ProcessInstance(const ProcessInstance& orig)
{
    close();
    
    DuplicateHandle(
        GetCurrentProcess(),
        orig.m_handle,
        GetCurrentProcess(),
        &m_handle.ptr,
        0,
        FALSE,
        DUPLICATE_SAME_ACCESS);
}

ProcessInstance::~ProcessInstance()
{
    close();
}

bool ProcessInstance::launch(const string& exec, const Args& args, bool wait)
{
    //TODO: implement
    return false;
}

bool ProcessInstance::open(int pid)
{
    if (isValid() || pid == 0) {
        return false;
    }
    else {
        m_handle.ptr = OpenProcess(PROCESS_DUP_HANDLE | SYNCHRONIZE, FALSE, pid);
        return isValid();
    }
}

bool ProcessInstance::isRunning()
{
    return isValid() && (WaitForSingleObject(m_handle.ptr, 0) == WAIT_TIMEOUT);
}

bool ProcessInstance::isValid()
{
    return (m_handle.ptr != NULL);
}

void ProcessInstance::close(bool wait)
{
    if (isValid()) {
        if (wait) WaitForSingleObject(m_handle.ptr, INIFINITE);
        CloseHandle(m_handle.ptr);
        m_handle.ptr = NULL;
    }
}

ProcessParams& operator=(const ProcessInstance& copy)
{
    close();

    DuplicateHandle(
        GetCurrentProcess(),
        orig.m_handle,
        GetCurrentProcess(),
        &m_handle.ptr,
        0,
        FALSE,
        DUPLICATE_SAME_ACCESS);
}

#endif

}
}