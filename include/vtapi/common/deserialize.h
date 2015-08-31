/**
 * @file
 * @brief   Definition of deserialization functions
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
#include <sstream>
#include <iomanip>
#include <vector>
#include <utility>

namespace vtapi {


/**
 * @brief Generic conversion from string to vector representation
 * @param buffer   input string
 * @return vector of values
 */
template<class T>
inline std::vector<T> deserializeVector(const std::string & buffer)
{
    std::vector<T> ret;

    if ((buffer.find('[', 0) == 0) && (buffer.find(']', 0) == buffer.length()-1)) {
        size_t leftPos  = 1;
        size_t nextPos  = 1;
        int size        = 1;
        leftPos         = buffer.find(',', 1);
        while ( leftPos != std::string::npos) {
            size++;
            leftPos     = buffer.find(',', leftPos+1);
        }

        leftPos = 1;
        for (int i = 0; i < size; i++) {
            T val;
            nextPos = buffer.find_first_of(",]", leftPos+1);
            std::stringstream(buffer.substr(leftPos, nextPos-leftPos)) >> val;
            ret.push_back(std::move(val));
            leftPos = nextPos + 1;
        }

    }
    else {
        T val;
        std::stringstream(buffer) >> val;
        ret.push_back(std::move(val));
    }

    return ret;
}

}
