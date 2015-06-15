#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility>
#include "../common/vtapi_serialize.h"

namespace vtapi {


class BaseParam;
class BaseParams;

/**
 * Possible types of method params
 */
typedef enum
{
    PARAMTYPE_NONE,
    PARAMTYPE_STRING,
    PARAMTYPE_INT,
    PARAMTYPE_DOUBLE,
    PARAMTYPE_INTVECTOR,
    PARAMTYPE_DOUBLEVECTOR
} ParamType;


/**
 * Get param type from its value type
 * @param val param value
 * @return param type
 */
template<class T>
static ParamType toParamType(const T& val)
{ val; return PARAMTYPE_NONE; }

template<>
ParamType toParamType<std::string>(const std::string &val)
{ val; return vtapi::PARAMTYPE_STRING; }

template<>
ParamType toParamType<int>(const int &val)
{ val; return vtapi::PARAMTYPE_INT; }

template<>
ParamType toParamType<double>(const double &val)
{ val; return vtapi::PARAMTYPE_DOUBLE; }

template<>
ParamType toParamType< std::vector<int> >(const std::vector<int> &val)
{ val; return vtapi::PARAMTYPE_INTVECTOR; }

template<>
ParamType toParamType< std::vector<double> >(const std::vector<double> &val)
{ val; return vtapi::PARAMTYPE_DOUBLEVECTOR; }


/**
 * Base method/process param class, should be derived from
 */
class BaseParam
{
public:
    virtual ~BaseParam() { }

    ParamType type() const
    { return m_type; }
    
    virtual std::string toString() const = 0;

protected:
    explicit BaseParam(ParamType type)
    : m_type(type) { }

private:
    ParamType m_type;
};

/**
 * Base class encapsulating list of base params, should be derived from
 */
class BaseParams
{
public:
    BaseParams() {}
    virtual ~BaseParams() {}

    // param existence queries

    bool hasString(const std::string& key) const
    { return has(key, PARAMTYPE_STRING); }
    
    bool hasInt(const std::string& key) const
    { return has(key, PARAMTYPE_INT); }
    
    bool hasDouble(const std::string& key) const
    { return has(key, PARAMTYPE_DOUBLE); }
    
    bool hasIntVector(const std::string& key) const
    { return has(key, PARAMTYPE_INTVECTOR); }
    
    bool hasDoubleVector(const std::string& key) const
    { return has(key, PARAMTYPE_DOUBLEVECTOR); }
    
    /**
     * Is map empty?
     * @return boolean value
     */
    bool empty() const
    { return m_data.empty(); }
    
protected:
    std::map<std::string, BaseParam *>  m_data;

private:
    
    // templated stuff
    
    bool has(const std::string& key, ParamType type) const
    {
        const auto it = m_data.find(key);
        return it != m_data.end() && it->second->type() == type;
    }
    

};

}
