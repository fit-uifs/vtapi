/**
 * @file
 * @brief   Declaration of TKey class
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include "serialize.h"
#include <string>
#include <vector>
#include <utility>

namespace vtapi {


class TKey;
typedef std::vector<TKey> TKeys;
template <typename T> class TKeyValue;
typedef std::vector<TKey *> TKeyValues;


/**
 * @brief Class represents a generic Key (of the Key-Value pair)
 * 
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class TKey
{
public:
    /**
     * Constructor with full specification of arguments
     * @param type   key data type
     * @param key    key name (column name)
     * @param from   additional key specification (eg. table)
     */
    TKey(const std::string& type,
         const std::string& key,
         const std::string& from)
        : _type(type), _key(key), _from(from) {}

    virtual ~TKey() {}

    TKey(TKey &&) = default;
    TKey & operator=(TKey &&) = default;

    /**
     * @brief type accessor
     * @return type
     */
    inline std::string type() const
    { return _type; }

    /**
     * @brief type accessor
     * @return type
     */
    inline std::string key() const
    { return _key; }

    /**
     * @brief type accessor
     * @return type
     */
    inline std::string from() const
    { return _from; }

    /**
     * Gets string representation of the first value
     * @return representation of value(s)
     */
    inline virtual std::string value() const
    { return std::string(); }

private:
    std::string _type;    /**< Name of the data type */
    std::string _key;     /**< Name of the column */
    std::string _from;    /**< The source (table) */

    TKey() = delete;
};


/**
 * @brief A generic class for storing a single Key-Value(s) pair
 *
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
    /**
     * Full constructor with single value
     * @param type    key data type
     * @param key     key name (column name)
     * @param value   single value
     * @param from    additional key specification (eg. table)
     */
    TKeyValue(const std::string& type,
              const std::string& key,
              T& value,
              const std::string& from)
        : TKey(type, key, from)
    {
        _values.push_back(value);
    }

    /**
     * Move constructor with single value
     * @param type    key data type
     * @param key     key name (column name)
     * @param value   single value
     * @param from    additional key specification (eg. table)
     */
    TKeyValue(const std::string& type,
              const std::string& key,
              T&& value,
              const std::string& from)
        : TKey(type, key, from)
    {
        _values.push_back(std::move(value));
    }

    /**
     * Full constructor with multiple values
     * @param type     key data type
     * @param key      key name (column name)
     * @param values   vector of values
     * @param from     additional key specification (eg. table)
     */
    TKeyValue (const std::string& type,
               const std::string& key,
               const std::vector<T> &values,
               const std::string& from)
        : TKey(type, key, from), _values(values) {}

    /**
     * Move constructor with multiple values
     * @param type     key data type
     * @param key      key name (column name)
     * @param values   vector of values
     * @param from     additional key specification (eg. table)
     */
    TKeyValue (const std::string& type,
               const std::string& key,
               std::vector<T> && values,
               const std::string& from)
        : TKey(type, key, from), _values(std::move(values)) {}

    TKeyValue(TKeyValue &&) = default;
    TKeyValue & operator=(TKeyValue &&) = default;

    /**
     * Gets string representation of the first value
     * @return representation of value(s)
     */
    inline std::string value() const override
    {
        if (_values.size() == 1)
            return toString<T>(_values[0]);
        else if (_values.size() > 1)
            return toString< std::vector<T> >(_values);
        else
            return std::string();
    }

private:
    std::vector<T> _values;    /**< values */

    TKeyValue() = delete;
};


}
