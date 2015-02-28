/**
 * @file
 * @brief   Methods of TKey class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref Licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <common/vtapi_global.h>
#include <common/vtapi_serialize.h>
#include <common/vtapi_tkey.h>

using std::string;
using std::cout;
using std::endl;

using namespace vtapi;


string TKey::print() {
    string ret = "TKey type=" + type + ", key=" + key + ", size=" + toString(size) + ", from=" + from;
    cout << ret << endl;
    return (ret);
}
