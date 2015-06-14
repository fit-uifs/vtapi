#pragma once

#include <string>
#include <utility>
#include "vtapi_methodparams.h"
#include "../common/vtapi_baseparams.h"


namespace vtapi {


class ProcessParams : public BaseParams
{
public:
    ProcessParams();
    explicit ProcessParams(ProcessParams && other);
    explicit ProcessParams(const std::string& serialized);
    ~ProcessParams();

    ProcessParams& operator=(ProcessParams&& other);
    
    // input process name

    bool hasInputProcessName() const;
    bool getInputProcess(std::string& value) const;
    void setInputProcessName(const std::string& value);

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
    
    /**
     * Validates and processes params using metadata in database.
     * Also adds new params which are required and have a default value.
     * @param definitions method param definitions including validation information
     * @return succesful validation
     */
    bool validate(const MethodParams& definitions);
    
private:
    std::string m_inputProcessName;
    
    // templated stuff
    template <class T>
    bool get(const std::string& key, T& value) const;
    
    template <class T>
    void add(const std::string& key, const T& value);
    
    // internal stuff

    void deserializeParam(
        const std::string& key,
        const std::string& type,
        const std::string& val);

} ;

}

