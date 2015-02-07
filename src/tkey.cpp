/**
 * @file    tkey.cpp
 * @author  VTApi Team, FIT BUT, CZ
 * @author  Petr Chmelar, chmelarp@fit.vutbr.cz
 * @author  Vojtech Froml, xfroml00@stud.fit.vutbr.cz
 * @author  Tomas Volf, ivolf@fit.vutbr.cz
 *
 * @brief   Methods of TKey class
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
