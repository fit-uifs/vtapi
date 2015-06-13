#pragma once

#include <string>
#include <map>
#include <utility>
#include "../common/vtapi_serialize.h"

namespace vtapi {

typedef enum
{
    PARAMTYPE_NONE,
    PARAMTYPE_STRING,
    PARAMTYPE_INT,
    PARAMTYPE_DOUBLE,
    PARAMTYPE_INTVECTOR,
    PARAMTYPE_DOUBLEVECTOR
} ProcessParamType;


class ProcessParamBase
{
public:
    virtual ~ProcessParamBase() { }

    ProcessParamType type()
    { return m_type; }
    
    virtual std::string toString() = 0;

protected:
    explicit ProcessParamBase(ProcessParamType type)
    : m_type(type) { }

private:
    ProcessParamType m_type;
};


template <typename T>
class ProcessParam : public ProcessParamBase
{
public:
    explicit ProcessParam(const T& value)
    : ProcessParamBase(val_type(value)), m_value(value) { }
    
    explicit ProcessParam(T&& value)
    : ProcessParamBase(val_type(value)), m_value(std::move(value)) { }    

    virtual ~ProcessParam() { }

    const T& value() const
    { return m_value; }
    
    std::string toString()
    { return vtapi::toString<T>(m_value); }

private:
    T m_value;
    
    ProcessParamType val_type(const std::string &val) const
    { val; return PARAMTYPE_STRING; }
    
    ProcessParamType val_type(int val) const
    { val; return PARAMTYPE_INT; }
    
    ProcessParamType val_type(double val) const
    { val; return PARAMTYPE_DOUBLE; }
    
    ProcessParamType val_type(const std::vector<int> &val) const
    { val; return PARAMTYPE_INTVECTOR; }
    
    ProcessParamType val_type(const std::vector<double> &val) const
    { val; return PARAMTYPE_DOUBLEVECTOR; }
};


class ProcessParams
{
public:
    ProcessParams();
    explicit ProcessParams(ProcessParams && other);
    explicit ProcessParams(const std::string& serialized);
    virtual ~ProcessParams();

    ProcessParams& operator=(ProcessParams&& other);
    
    // input process name

    bool hasInputProcessName() const;
    bool getInputProcess(std::string& value) const;
    void setInputProcessName(const std::string& value);

    // param existence queries

    bool hasString(const std::string& key) const
    { return has<std::string>(key); }

    bool hasInt(const std::string& key) const
    { return has<int>(key); }

    bool hasDouble(const std::string& key) const
    { return has<double>(key); }

    bool hasIntVector(const std::string& key) const
    { return has< std::vector<int> >(key); }

    bool hasDoubleVector(const std::string& key) const
    { return has< std::vector<double> >(key); }
    
    // param getters

    bool getString(const std::string& key, std::string& value) const
    { return get<std::string>(key, value); }
    
    bool getInt(const std::string& key, int& value) const
    { return get<int>(key, value); }
    
    bool getDouble(const std::string& key, double& value) const
    { return get<double>(key, value); }
    
    bool getIntVector(const std::string& key, std::vector<int>& value) const
    { return get< std::vector<int> >(key, value); }
    
    bool getDoubleVector(const std::string& key, std::vector<double>& value) const
    { return get< std::vector<double> >(key, value); }
    
    // param adders

    void addString(const std::string& key, const std::string& value)
    { add<std::string>(key, value); }
    
    void addString(const std::string& key, std::string&& value)
    { add<std::string>(key, std::move(value)); }
    
    void addInt(const std::string& key, int value)
    { add<int>(key, value); }
    
    void addDouble(const std::string& key, double value)
    { add<double>(key, value); }
    
    void addIntVector(const std::string& key, const std::vector<int>& value)
    { add< std::vector<int> >(key, value); }
    
    void addIntVector(const std::string& key, std::vector<int>&& value)
    { add< std::vector<int> >(key, std::move(value)); }
    
    void addDoubleVector(const std::string& key, const std::vector<double>& value)
    { add< std::vector<double> >(key, value); }
    
    void addDoubleVector(const std::string& key, std::vector<double>&& value)
    { add< std::vector<double> >(key, std::move(value)); }

    // other utilites
    
    /**
     * Is map empty?
     * @return boolean value
     */
    bool empty() const;
    
    /**
     * Clear map
     */
    void clear();

    /**
     * Serializes params for input to DB (JSON-like format)
     * Output string doesn't include input process name
     * @return 
     */
    std::string serialize() const;

    /**
     * Serializes params as process name postfix, eg.: inputProcess_0.2_myval
     * Includes input process name string
     * @return 
     */
    std::string serializeAsName() const;


    /**
     * Deserializes params params from DB input (JSON-like format)
     * Deletes all previously stored params
     * @param serialized serialized input string
     */
    void deserialize(const std::string& serialized);
    
private:
    std::map<std::string, ProcessParamBase *>  m_data;
    std::string m_inputProcessName;

    // templated stuff
    
    template <typename T>
    bool has(const std::string& key) const
    {
        const auto it = m_data.find(key);
        return
            it != m_data.end() &&
            dynamic_cast< ProcessParam<T>* > (it->second);
    }
    
    template <typename T>
    bool get(const std::string& key, T& value) const
    {
        const auto it = m_data.find(key);
        if (it != m_data.end()) {
            auto param = dynamic_cast< ProcessParam<T>* > (it->second);
            if (param) {
                value = param->value();
                return true;
            }
        }
        return false;
    }
    
    template <typename T>
    void add(const std::string& key, const T& value)
    {
        const auto it = m_data.find(key);
        if (it != m_data.end()) {
            delete it->second;
            it->second = new ProcessParam<T>(value);
        }
        else {
            m_data[key] = new ProcessParam<T>(value);
        }
    }
    
    // internal stuff

    void deserializeParam(std::string key, std::string type, std::string val);

} ;

}

