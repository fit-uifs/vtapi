/**
 * @file
 * @brief   Declaration of EdfDescriptor class
 *
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 *
 * @copyright   &copy; 2011 &ndash; 2018, Brno University of Technology
 */

#pragma once

#include <vector>
#include <iostream>

namespace vtapi {


/**
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 *
 * @copyright   &copy; 2011 &ndash; 2018, Brno University of Technology
 */
class EyedeaEdfDescriptor
{
public:
    int version;
    std::vector<unsigned char> data;

    EyedeaEdfDescriptor()
        : version(0), data() {}

    EyedeaEdfDescriptor(int version, int size, unsigned char *data) {
        this->version = version;
        for (int i = 0; i < size; i++) {
            this->data.push_back(data[i]);
        }
    }
};


} // namespace vtapi
