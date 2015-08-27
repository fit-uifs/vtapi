#pragma once

#include <string>
#include "methodparams.h"
#include "../common/baseparams.h"


namespace vtapi {

template <typename T>
class TaskParam : public BaseParam
{
public:

    explicit TaskParam(const T& value)
        : BaseParam(toParamType(value)), m_value(value) { }

    explicit TaskParam(T&& value)
        : BaseParam(toParamType(value)), m_value(std::move(value)) { }

    virtual ~TaskParam() { }

    const T& value() const
    {
        return m_value;
    }

    std::string toString() const
    {
        return vtapi::toString<T>(m_value);
    }

private:
    T m_value;
} ;


class TaskParams : public BaseParams
{
public:
    TaskParams();
    explicit TaskParams(TaskParams && other);
    explicit TaskParams(const std::string& serialized);
    ~TaskParams();

    TaskParams& operator=(TaskParams&& other);

    // param getters

    bool getString(const std::string& key, std::string& value) const;    
    bool getInt(const std::string& key, int& value) const;    
    bool getDouble(const std::string& key, double& value) const;    
    bool getIntVector(const std::string& key, std::vector<int>& value) const;    
    bool getDoubleVector(const std::string& key, std::vector<double>& value) const; 
    
    // param adders

    void addString(const std::string& key, const std::string& value);    
    void addString(const std::string& key, std::string&& value);    
    void addInt(const std::string& key, int value);    
    void addDouble(const std::string& key, double value);    
    void addIntVector(const std::string& key, const std::vector<int>& value);    
    void addIntVector(const std::string& key, std::vector<int>&& value);    
    void addDoubleVector(const std::string& key, const std::vector<double>& value);    
    void addDoubleVector(const std::string& key, std::vector<double>&& value);

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
     * Deserializes params params from DB input (JSON-like format)
     * Deletes all previously stored params
     * @param serialized serialized input string
     */
    void deserialize(const std::string& serialized);
    
    /**
     * Validates and processes params using metadata in database.
     * Also adds new params which are required and have a default value.
     * @param definitions method param definitions including validation information
     * @return succesful validation
     */
    bool validate(const MethodParams& definitions);
    
private:
    // templated stuff
    template <class T>
    bool get(const std::string& key, T& value) const;
    
    template <class T>
    void add(const std::string& key, const T& value);
    
    template <class T>
    void add(const std::string& key, T&& value);
    
    // internal stuff

    void deserializeParam(
        const std::string& key,
        const std::string& type,
        const std::string& val);

} ;

}

