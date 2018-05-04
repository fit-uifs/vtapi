#include <vtapi/common/global.h>
#include <vtapi/common/deserialize.h>
#include <vtapi/data/taskparams.h>
#include <utility>

using namespace std;

namespace vtapi {



template <typename T>
bool TaskParams::get(const string& key, T& value) const
{
    const auto it = _data.find(key);
    if (it != _data.end()) {
        auto param = dynamic_cast< TaskParamValue<T>* > (it->second.get());
        if (param) {
            value = param->getValue();
            return true;
        }
    }
    return false;
}

template <typename T>
void TaskParams::add(const string& key, const T& value)
{
    const auto it = _data.find(key);
    if (it != _data.end())
        it->second = std::make_shared< TaskParamValue<T> >(value);
    else
        _data[key] = std::make_shared< TaskParamValue<T> >(value);
}

template <typename T>
void TaskParams::add(const string& key, T&& value)
{
    const auto it = _data.find(key);
    if (it != _data.end())
        it->second = std::make_shared< TaskParamValue<T> >(std::move(value));
    else
        _data[key] = std::make_shared< TaskParamValue<T> >(std::move(value));
}

bool TaskParams::getString(const string& key, string& value) const
{ return get<string>(key, value); }

bool TaskParams::getInt(const string& key, int& value) const
{ return get<int>(key, value); }

bool TaskParams::getFloat(const string& key, float& value) const {
    double doubleValue;
    bool returnValue;

    returnValue = get<double>(key, doubleValue);
    value = (float) doubleValue;

    return returnValue;
}

bool TaskParams::getDouble(const string& key, double& value) const
{ return get<double>(key, value); }

bool TaskParams::getIntVector(const string& key, vector<int>& value) const
{ return get< vector<int> >(key, value); }

bool TaskParams::getDoubleVector(const string& key, vector<double>& value) const
{ return get< vector<double> >(key, value); }


template<>
TaskParamValueBase::Type TaskParamValueString::getType() const
{ return TaskParamValueBase::PARAMVALUE_STRING; }

template<>
TaskParamValueBase::Type TaskParamValueInt::getType() const
{ return TaskParamValueBase::PARAMVALUE_INT; }

template<>
TaskParamValueBase::Type TaskParamValueDouble::getType() const
{ return TaskParamValueBase::PARAMVALUE_DOUBLE; }

template<>
TaskParamValueBase::Type TaskParamValueIntVector::getType() const
{ return TaskParamValueBase::PARAMVALUE_INTVECTOR; }

template<>
TaskParamValueBase::Type TaskParamValueDoubleVector::getType() const
{ return TaskParamValueBase::PARAMVALUE_DOUBLEVECTOR; }


template<>
TaskParamValueBase::Type TaskParamDefinitionString::getType() const
{ return TaskParamValueBase::PARAMVALUE_STRING; }

template<>
TaskParamValueBase::Type TaskParamDefinitionInt::getType() const
{ return TaskParamValueBase::PARAMVALUE_INT; }

template<>
TaskParamValueBase::Type TaskParamDefinitionDouble::getType() const
{ return TaskParamValueBase::PARAMVALUE_DOUBLE; }

template<>
TaskParamValueBase::Type TaskParamDefinitionIntVector::getType() const
{ return TaskParamValueBase::PARAMVALUE_INTVECTOR; }

template<>
TaskParamValueBase::Type TaskParamDefinitionDoubleVector::getType() const
{ return TaskParamValueBase::PARAMVALUE_DOUBLEVECTOR; }



// param adders

void TaskParams::addString(const string& key, const string & value)
{ return add<string>(key, value); }

void TaskParams::addString(const string& key, string && value)
{ return add<string>(key, std::move(value)); }

void TaskParams::addInt(const string& key, int value)
{ return add<int>(key, value); }

void TaskParams::addDouble(const string& key, double value)
{ return add<double>(key, value); }

void TaskParams::addIntVector(const string& key, const vector<int>& value)
{ return add< vector<int> >(key, value); }

void TaskParams::addIntVector(const string& key, vector<int> && value)
{ return add< vector<int> >(key, std::move(value)); }

void TaskParams::addDoubleVector(const string& key, const vector<double>& value)
{ return add< vector<double> >(key, value); }

void TaskParams::addDoubleVector(const string& key, vector<double> && value)
{ return add< vector<double> >(key, std::move(value)); }


string TaskParams::serialize() const
{
    string ret;

    ret += '{';

    for (auto& kv : _data) {
        if (ret.length() > 1) ret += ',';
        ret += kv.first;
        ret += ':';
        ret += toString<int>(kv.second->getType());
        ret += ':';
        ret += '\"';
        ret += kv.second->getSerialized();
        ret += '\"';
    }

    ret += '}';

    return ret;
}

void TaskParams::deserialize(const string& serialized)
{
    _data.clear();

    if (!serialized.empty() &&
        serialized[0] == '{' &&
        serialized[serialized.length() - 1] == '}')
    {
        size_t keyPos = 1;
        size_t maxPos = serialized.length() - 1;

        do  {
            // find value type, get key length
            size_t typePos = serialized.find(':', keyPos);
            if (typePos == string::npos) break;
            size_t keyLen = typePos - keyPos;
            typePos++;

            // find value, get value type length
            size_t valPos = serialized.find(':', typePos);
            if (valPos == string::npos) break;
            size_t typeLen = valPos - typePos;
            valPos++;

            // all values should be quoted
            if (serialized[valPos] != '\"') break;

            // find value length and next key position
            size_t valEndPos = serialized.find('\"', ++valPos);
            if (valEndPos == string::npos) break;
            size_t valLen = valEndPos - valPos;

            size_t nextKeyPos = valEndPos + 2;

            deserializeParam(serialized.substr(keyPos, keyLen),
                             serialized.substr(typePos, typeLen),
                             serialized.substr(valPos, valLen));

            keyPos = nextKeyPos;
        } while (keyPos < maxPos);
    }
}

void TaskParams::deserializeParam(const string& key,
                                  const string& type,
                                  const string& val)
{
    try {
        switch (static_cast<TaskParamValueBase::Type>(std::stoi(type)))
        {
        case TaskParamValueBase::PARAMVALUE_STRING:
            addString(key, val);
            break;
        case TaskParamValueBase::PARAMVALUE_INT:
            addInt(key, std::stoi(val));
            break;
        case TaskParamValueBase::PARAMVALUE_DOUBLE:
            addDouble(key, std::stod(val));
            break;
        case TaskParamValueBase::PARAMVALUE_INTVECTOR:
        {
            addIntVector(key, vtapi::deserializeVector<int>(val));
            break;
        }
        case TaskParamValueBase::PARAMVALUE_DOUBLEVECTOR:
        {
            addDoubleVector(key, vtapi::deserializeVector<double>(val));
            break;
        }
        }
    }
    catch (std::invalid_argument) {
        std::cerr << "INVALID PARAM : key:" << key << ";type:" << type << ";val:" << val << std::endl;
    }
}

bool TaskParams::validate(const TaskParamDefinitions& definitions)
{
    return true;
}



}
