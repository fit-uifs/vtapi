/**
 * @file
 * @brief   Methods of TKey class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <common/vtapi_global.h>
#include <common/vtapi_serialize.h>
#include <common/vtapi_tkey.h>

using namespace std;

namespace vtapi {


string TKey::print() {
    string ret = "TKey type=" + m_type + ", key=" + m_key + ", size=" + toString(m_size) + ", from=" + m_from;
    cout << ret << endl;
    return (ret);
}

}
