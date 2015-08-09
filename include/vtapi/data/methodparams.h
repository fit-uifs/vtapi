#pragma once

#include <string>
#include <vector>
#include <utility>
#include "../common/baseparams.h"

namespace vtapi {

template<class T>
class MethodParam;

typedef MethodParam<std::string>    MethodParamString;
typedef MethodParam<int>            MethodParamInt;
typedef MethodParam<double>         MethodParamDouble;
typedef MethodParam< std::vector<int> >     MethodParamIntVector;
typedef MethodParam< std::vector<double> >  MethodParamDoubleVector;



/**
 * Class encapsulating method parameter definition,
 * not including extended attributes like default value, validation range etc.
 */
template<class T>
class MethodParam : public BaseParam
{
public:
    MethodParam(bool required)
    : BaseParam(toParamType(T())), m_flags(0)
    { if (required) m_flags |= FL_REQUIRED; }

    MethodParam(bool required, T&& default_val)
    :  BaseParam(toParamType(default_val)),
        m_default_val(std::move(default_val)),
        m_flags(FL_DEFAULT_VAL)
    { if (required) m_flags = FL_REQUIRED; }

    MethodParam(bool required, T&& min_val, T&& max_val)
    :  BaseParam(toParamType(min_val)),
        m_min_val(std::move(min_val)), m_max_val(std::move(max_val)),
        m_flags(FL_RANGE)
    { if (required) m_flags = FL_REQUIRED; }

    MethodParam(bool required, T&& default_val, T&& min_val, T&& max_val)
    :   BaseParam(toParamType(default_val)),
        m_default_val(std::move(default_val)),
        m_min_val(std::move(min_val)), m_max_val(std::move(max_val)),
        m_flags(FL_DEFAULT_VAL|FL_RANGE)
    { if (required) m_flags = FL_REQUIRED; }
    
    MethodParam(bool required, const std::string& default_val, const std::string& range)
    {
        //TODO
    }
    
    MethodParam(MethodParam && other)
    : BaseParam(other.type()),
        m_default_val(std::move(default_val)),
        m_min_val(std::move(min_val)), m_max_val(std::move(max_val)),
        m_flags(other.m_flags) {}
    
    ~MethodParam() { }

    MethodParam& operator=(MethodParam&& other)
    {
        m_default_val = std::move(other.m_default_val);
        m_min_val = std::move(other.m_min_val);
        m_max_val = std::move(other.m_max_val);
        m_flags = other.m_flags;
        
        return *this;
    }
    
    bool is_required() const
    { return m_flags & FL_REQUIRED; }

    bool has_default_val() const
    { return m_flags & FL_DEFAULT_VAL; }

    bool has_range() const
    { return m_flags & FL_RANGE; }

    const T& default_val() const
    { return m_default_val; }

    const T& min_val() const
    {  return m_min_val; }

    const T& max_val() const
    { return m_max_val; }
    
private:
    MethodParam() {}

    enum
    {
        FL_REQUIRED     = (1),
        FL_DEFAULT_VAL  = (1 << 1),
        FL_RANGE        = (1 << 2)
    } ;
    
    T m_default_val;
    T m_min_val;
    T m_max_val;
    unsigned char m_flags;
};


class MethodParams : public BaseParams
{
public:
    MethodParams() : BaseParams() {}
    ~MethodParams() {}

    template<class T>
    void addParam(const std::string &key, MethodParam<T> && param)
    {
        const auto it = m_data.find(key);
        if (it != m_data.end()) {
            delete it->second;
            it->second = new MethodParam<T>(std::move(param));
        }
        else {
            m_data[key] = new MethodParam<T>(std::move(param));
        }
    }
    
    template<class T>
    bool getParam(const std::string& key, MethodParam<T> & param)
    {
        const auto it = m_data.find(key);
        if (it != m_data.end()) {
            auto item = dynamic_cast< MethodParam<T>* > (it->second);
            if (item) {
                param = *item;
                return true;
            }
        }
        return false;
    }
    
    void clear()
    {
        for (auto &item : m_data) delete item.second;
        m_data.clear();
    }
    
};

}
