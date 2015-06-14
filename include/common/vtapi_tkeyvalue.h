/**
 * @file
 * @brief   Declaration and also defition of TKeyValue class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include <string>
#include <vector>
#include "vtapi_serialize.h"
#include "vtapi_tkey.h"

namespace vtapi {

/**
 * TKeyValues is an abbreviation for a vector of TKey pointers
 */
typedef std::vector<TKey *>     TKeyValues;


/**
 * @brief A generic class for storing a single Key-Value(s) pair
 *
 *
 * @warning
 *     - use PDOs only ... http://en.wikipedia.org/wiki/Plain_old_data_structure <br>
 *     - if you use pointers, you shouldn't free them
 * @note You can use size=-1 for NULL :)
 *
 * @see http://en.wikipedia.org/wiki/Plain_old_data_structure
 * @see http://www.cplusplus.com/doc/tutorial/templates/
 * @see http://stackoverflow.com/questions/2627223/c-template-class-constructor-with-variable-arguments
 * @see http://www.cplusplus.com/reference/std/typeinfo/type_info/
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
template <typename T>
class TKeyValue : public TKey
{
public:
    T *values;    /**< values */

    /**
     * Default constructor
     */
    TKeyValue()
    : TKey(), values(NULL) {}

    /**
     * Full constructor with single value
     * @param type    key data type
     * @param key     key name (column name)
     * @param value   single value
     * @param from    additional key specification (eg. table)
     */
    TKeyValue(
        const std::string& type,
        const std::string& key,
        T value,
        const std::string& from)
    : TKey(type, key, 1, from)
    {
        this->values = new T[1];
        this->values[0] = value;
    }
    /**
     * Full constructor with multiple values
     * @param type     key data type
     * @param key      key name (column name)
     * @param values   array of values
     * @param size     size of array of values
     * @param from     additional key specification (eg. table)
     */
    TKeyValue (
        const std::string& type,
        const std::string& key, T* values,
        const int size,
        const std::string& from)
    : TKey(type, key, size, from)
    {
        if (m_size > 0) {
            this->values = new T[m_size];
            std::copy(values, values+m_size, this->values);
        }
    }

    /**
     * Destructor
     */
    ~TKeyValue ()
    { if (values) delete values; }

    /**
     * Gets string representation of the first value
     * @return representation of value(s)
     */
    std::string getValue();
    
    /**
     * Gets string representation of first limit values
     * @param limit   how many values, 0 (or less) = unlimited
     * @return representation of value(s)
     */
    std::string getValues(const int limit = 0);
};


template <class T>
std::string TKeyValue<T>::getValue()
{
    if (values && m_size > 0) {
        return toString(values[0]);
    }
    else {
        return "";
    }
}

template <class T>
std::string TKeyValue<T>::getValues(const int limit)
{
    std::string ret;
    if (values && m_size > 0) {
        if (m_size > 1) ret += "[";
        for(int i = 0; (i < m_size) && (limit <= 0 || i < limit); ++i) {
            ret += toString(values[i]) + ",";
        }
        if (limit <= 0) ret = ret.erase(ret.length()-1);
        if (m_size > 1) ret += "]";
    }
    return ret;
}


} // namespace vtapi
