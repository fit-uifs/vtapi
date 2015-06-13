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
    ProcessParams() { }
    
    explicit ProcessParams(const std::string& serialized)
    { deserialize(serialized); }
    
    ProcessParams(ProcessParams && other)
    {
        m_inputProcessName = std::move(other.m_inputProcessName);
        m_data = std::move(other.m_data);
    }

    virtual ~ProcessParams()
    { clear(); }

    ProcessParams& operator=(ProcessParams&& other)
    {
        clear();
        m_inputProcessName = std::move(other.m_inputProcessName);
        m_data = std::move(other.m_data);
        return *this;
    }
    
    // input process name

    bool getInputProcess(std::string& value) const
    {
        if (!m_inputProcessName.empty()) {
            value = m_inputProcessName;
            return true;
        }
        else {
            return false;
        }
    }
    void setInputProcessName(const std::string& value)
    {
        m_inputProcessName = value;
    }
    bool hasInputProcessName() const
    {
        return !m_inputProcessName.empty();
    }
    
    // param adders

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
    void addString(const std::string& key, const std::string& value)
    {
        add<std::string>(key, value);
    }
    void addInt(const std::string& key, int value)
    {
        add<int>(key, value);
    }
    void addDouble(const std::string& key, double value)
    {
        add<double>(key, value);
    }
    void addIntVector(const std::string& key, const std::vector<int>& value)
    {
        add< std::vector<int> >(key, value);
    }
    void addIntVector(const std::string& key, const std::vector<int>&& value)
    {
        add< std::vector<int> >(key, std::move(value));
    }
    void addDoubleVector(const std::string& key, const std::vector<double>& value)
    {
        add< std::vector<double> >(key, value);
    }
    void addDoubleVector(const std::string& key, const std::vector<double>&& value)
    {
        add< std::vector<double> >(key, std::move(value));
    }
    
    // param getters

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
    bool getString(const std::string& key, std::string& value) const
    {
        return get<std::string>(key, value);
    }
    bool getInt(const std::string& key, int& value) const
    {
        return get<int>(key, value);
    }
    bool getDouble(const std::string& key, double& value) const
    {
        return get<double>(key, value);
    }
    bool getIntVector(const std::string& key, std::vector<int>& value) const
    {
        return get< std::vector<int> >(key, value);
    }
    bool getDoubleVector(const std::string& key, std::vector<double>& value) const
    {
        return get< std::vector<double> >(key, value);
    }
    
    // param existence queries

    template <typename T>
    bool keyExists(const std::string& key) const
    {
        const auto it = m_data.find(key);
        return
        it != m_data.end() &&
        dynamic_cast< ProcessParam<T>* > (it->second);
    }
    bool keyExistsString(const std::string& key) const
    {
        return keyExists<std::string>(key);
    }
    bool keyExistsInt(const std::string& key) const
    {
        return keyExists<int>(key);
    }
    bool keyExistsDouble(const std::string& key) const
    {
        return keyExists<double>(key);
    }
    bool keyExistsIntVector(const std::string& key) const
    {
        return keyExists< std::vector<int> >(key);
    }
    bool keyExistsDoubleVector(const std::string& key) const
    {
        return keyExists< std::vector<double> >(key);
    }
    
    // other utilites
    
    bool empty()
    {
        return m_data.empty();
    }
    void clear()
    {
        for (auto& kv : m_data) delete kv.second;
        m_data.clear();
        m_inputProcessName.clear();
    }

    // serialize params for input to DB
    // skip process input params, which are supplied differently
    std::string serialize() const
    {
        std::string ret;

        ret += '{';

        for (auto& kv : m_data) {
            if (ret.length() > 1) ret += ',';
            ret += kv.first;
            ret += ':';
            ret += toString(kv.second->type());
            ret += ':';
            ret += '\"';
            ret += kv.second->toString();
            ret += '\"';
        }

        ret += '}';

        return ret;
    }
    
    std::string serializeAsName() const
    {
        std::string ret;
        
        if (!m_inputProcessName.empty()) {
            ret = m_inputProcessName;
        }
        for (auto& kv : m_data) {
            if (!ret.empty()) ret += '_';
            ret += kv.second->toString();
        }
        
        return ret;
    }

    // deserialize params from DB input
    void deserialize(const std::string& serialized)
    {
        clear();

        if (!serialized.empty() &&
        serialized[0] == '{' &&
        serialized[serialized.length() - 1] == '}') {
            size_t keyPos = 1;
            size_t maxPos = serialized.length() - 1;

            do  {
                // find value type, get key length
                size_t typePos = serialized.find(':', keyPos);
                if (typePos == std::string::npos) break;
                size_t keyLen = typePos - keyPos;
                typePos++;

                // find value, get value type length
                size_t valPos = serialized.find(':', typePos);
                if (valPos == std::string::npos) break;
                size_t typeLen = valPos - typePos;
                valPos++;

                // all values should be quoted
                if (serialized[valPos] != '\"') break;
                
                // find value length and next key position
                size_t valEndPos = serialized.find('\"', ++valPos);
                if (valEndPos == std::string::npos) break;
                size_t valLen = valEndPos - valPos;
                
                size_t nextKeyPos = valEndPos + 2;

                deserializeParam(
                    serialized.substr(keyPos, keyLen),
                    serialized.substr(typePos, typeLen),
                    serialized.substr(valPos, valLen));

                keyPos = nextKeyPos;
            }
            while (keyPos < maxPos);
        }
    }
    
private:
    std::map<std::string, ProcessParamBase *>  m_data;
    std::string m_inputProcessName;
    


    void deserializeParam(std::string key, std::string type, std::string val)
    {
        try
        {
            switch((ProcessParamType)std::stoi(type))
            {
            case PARAMTYPE_NONE:
                break;
            case PARAMTYPE_STRING:
                addString(key, val);
                break;
            case PARAMTYPE_INT:
                addInt(key, std::stoi(val));
                break;
            case PARAMTYPE_DOUBLE:
                addDouble(key, std::stod(val));
                break;
            case PARAMTYPE_INTVECTOR:
            {
                std::vector<int> *vals = vtapi::deserializeV<int>(val.c_str());
                if (vals) {
                    addIntVector(key, std::move(*vals));
                    delete vals;
                }
                break;
            }
            case PARAMTYPE_DOUBLEVECTOR:
            {
                std::vector<double> *vals = vtapi::deserializeV<double>(val.c_str());
                if (vals) {
                    addDoubleVector(key, std::move(*vals));
                    delete vals;
                }
                break;
            }
            }
        }
        catch (std::invalid_argument) {
            std::cerr << "INVALID PARAM : key:" << key << ";type:" << type << ";val:" << val << std::endl;
        }
    }
} ;

}

