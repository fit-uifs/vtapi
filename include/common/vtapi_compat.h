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

#include <list>


namespace vtapi {
namespace compat {

typedef std::list<std::string> ARGS_LIST;

bool launchProcess(const std::string& bin, const ARGS_LIST& args, bool wait);



}
}

#endif
