#pragma once

#include "../common/serialize.h"
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>

namespace vtapi {


class TaskParamValueBase;
class TaskParamDefinitionBase;

template <typename T> class TaskParamValue;
typedef TaskParamValue<std::string>         TaskParamValueString;
typedef TaskParamValue<int>                 TaskParamValueInt;
typedef TaskParamValue<double>              TaskParamValueDouble;
typedef TaskParamValue< std::vector<int> >  TaskParamValueIntVector;
typedef TaskParamValue< std::vector<double> > TaskParamValueDoubleVector;

template <typename T> class TaskParamDefinition;
typedef TaskParamDefinition<std::string>         TaskParamDefinitionString;
typedef TaskParamDefinition<int>                 TaskParamDefinitionInt;
typedef TaskParamDefinition<double>              TaskParamDefinitionDouble;
typedef TaskParamDefinition< std::vector<int> >  TaskParamDefinitionIntVector;
typedef TaskParamDefinition< std::vector<double> > TaskParamDefinitionDoubleVector;

class TaskParams;

typedef std::map< std::string,std::shared_ptr<TaskParamDefinitionBase> > TaskParamDefinitions;


class TaskParamValueBase
{
public:
    enum Type
    {
        PARAMVALUE_STRING,
        PARAMVALUE_INT,
        PARAMVALUE_DOUBLE,
        PARAMVALUE_INTVECTOR,
        PARAMVALUE_DOUBLEVECTOR
    };

    virtual ~TaskParamValueBase() { }

    /**
     * @brief Gets parameter value type
     * @return type
     */
    virtual Type getType() const = 0;

    /**
     * @brief Gets parameter value in serialized form
     * @return string value
     */
    virtual std::string getSerialized() const = 0;
};


class TaskParamDefinitionBase : public TaskParamValueBase
{
public:
    TaskParamDefinitionBase()
        : _required(false), _has_default_val(false), _has_range(false) {}

    virtual ~TaskParamDefinitionBase() {}

    bool isRequired() const
    { return _required; }

    bool hasDefaultVal() const
    { return _has_default_val; }

    bool hasRange() const
    { return _has_range; }

    void setRequired(bool required)
    { _required = required; }

protected:
    bool _required;
    bool _has_default_val;
    bool _has_range;
};


template <typename T>
class TaskParamValue : public TaskParamValueBase
{
public:
    explicit TaskParamValue(const T& value)
        : _value(value) { }

    explicit TaskParamValue(T && value)
        : _value(std::move(value)) { }

    const T& getValue() const
    { return _value; }

    TaskParamValueBase::Type getType() const override;

    std::string getSerialized() const override
    { return toString<T>(_value); }

private:
    T _value;

    TaskParamValue() = delete;
};


template <typename T>
class TaskParamDefinition : public TaskParamDefinitionBase
{
public:
    const T& getDefaultValue() const
    { return _default_value; }

    void getRange(T & min_value, T & max_value) const
    { min_value = _min_value; max_value = _max_value; }

    void setDefaultValue(const T & default_value)
    { _has_default_val = true; _default_value = default_value; }

    void setRange(const T & min_value, const T & max_value)
    { _has_range = true; _min_value = min_value; max_value = max_value; }

    TaskParamValueBase::Type getType() const override;

    std::string getSerialized() const override
    { return std::string(); }

private:
    T _default_value;
    T _min_value;
    T _max_value;

    TaskParamDefinition() = delete;
};

class TaskParams
{
public:
    typedef std::map< std::string,std::shared_ptr<TaskParamValueBase> > ParamsMap;

    /**
     * @brief Constructor
     */
    TaskParams() {}

    /**
     * @brief Construct params from serialized form
     * @param serialized serialized form
     */
    explicit TaskParams(const std::string &serialized)
    { deserialize(serialized); }

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
    bool validate(const TaskParamDefinitions& definitions);

    /**
     * @brief Return param data for iterating and querying
     * @return parameter map
     */
    const ParamsMap & data() const
    { return _data; }


    // param getters

    bool getString(const std::string& key, std::string& value) const;
    bool getInt(const std::string& key, int& value) const;
    bool getDouble(const std::string& key, double& value) const;
    bool getIntVector(const std::string& key, std::vector<int>& value) const;
    bool getDoubleVector(const std::string& key, std::vector<double>& value) const;

    // param adders

    void addString(const std::string& key, const std::string & value);
    void addString(const std::string& key, std::string && value);
    void addInt(const std::string& key, int value);
    void addDouble(const std::string& key, double value);
    void addIntVector(const std::string& key, const std::vector<int>& value);
    void addIntVector(const std::string& key, std::vector<int> && value);
    void addDoubleVector(const std::string& key, const std::vector<double>& value);
    void addDoubleVector(const std::string& key, std::vector<double> && value);

private:
    ParamsMap  _data;

    // templated stuff

    template <class T>
    bool get(const std::string& key, T& value) const;
    
    template <class T>
    void add(const std::string& key, const T & value);
    
    template <class T>
    void add(const std::string& key, T && value);
    
    // internal stuff

    void deserializeParam(const std::string& key,
                          const std::string& type,
                          const std::string& val);

} ;


}

