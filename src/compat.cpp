
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

#include <unistd.h>
#include <wait.h>

#elif defined(VTAPI_WIN)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#else
#error "VTApi is incompatible with this platform"
#endif




namespace vtapi {
namespace compat {


bool launchProcess(const std::string& bin, const ARGS_LIST& args, bool wait)
{
#if defined(VTAPI_POSIX)
    pid_t cpid = fork();
    
    if (cpid == 0) {
        const char ** argv = new const char *[args.size() + 2];
        size_t i = 0;

        argv[i++] = bin.c_str();
        for (auto& arg : args) {
            argv[i++] = arg.c_str();
        }
        argv[i] = NULL;
        
        execv(argv[0], (char **)argv);
        exit(1);
    }
    else if (cpid > 0) {
        if (wait) {
            int ret = 0;
            if (waitpid(cpid, &ret, 0) == -1) {
                //logger->error("waitpid failed :" + this->getName(), thisClass + "::run()");
                return false;
            }
            else if (WIFEXITED(ret) && WEXITSTATUS(ret) != 0) {
                //logger->warning("child process exited with non-zero status", thisClass + "::run()");
                return false;
            }
        }
    }
    else {
        //logger->error("fork failed :" + this->getName(), thisClass + "::run()");
        return false;
    }    
    
    return true;
    
#elif defined(VTAPI_WIN)
    return false;
#endif
    
}

}
}


