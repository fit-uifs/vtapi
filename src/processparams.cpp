
#include <common/vtapi_global.h>
#include <common/vtapi_serialize.h>
#include <data/vtapi_processparams.h>

using namespace std;

namespace vtapi {

template <typename T>
class ProcessParam : public BaseParam
{
public:

    explicit ProcessParam(const T& value)
        : BaseParam(toParamType(value)), m_value(value) { }

    explicit ProcessParam(T&& value)
        : BaseParam(toParamType(value)), m_value(std::move(value)) { }

    virtual ~ProcessParam() { }

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


ProcessParams::ProcessParams()
: BaseParams()
{ }

ProcessParams::ProcessParams(ProcessParams && other)
: BaseParams(), m_inputProcessName(std::move(other.m_inputProcessName))
{
    m_data = std::move(other.m_data);
}

ProcessParams::ProcessParams(const string& serialized)
: BaseParams()
{
    deserialize(serialized);
}

ProcessParams::~ProcessParams()
{
    clear();
}

ProcessParams& ProcessParams::operator=(ProcessParams&& other)
{
    clear();
    m_inputProcessName = std::move(other.m_inputProcessName);
    m_data = std::move(other.m_data);
    return *this;
}

bool ProcessParams::hasInputProcessName() const
{
    return !m_inputProcessName.empty();
}

bool ProcessParams::getInputProcess(string& value) const
{
    if (!m_inputProcessName.empty()) {
        value = m_inputProcessName;
        return true;
    }
    else {
        return false;
    }
}

void ProcessParams::setInputProcessName(const string& value)
{
    m_inputProcessName = value;
}

template <typename T>
bool ProcessParams::get(const std::string& key, T& value) const
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

bool ProcessParams::getString(const std::string& key, std::string& value) const
{
    return get<std::string>(key, value);
}

bool ProcessParams::getInt(const std::string& key, int& value) const
{
    return get<int>(key, value);
}

bool ProcessParams::getDouble(const std::string& key, double& value) const
{
    return get<double>(key, value);
}

bool ProcessParams::getIntVector(const std::string& key, std::vector<int>& value) const
{
    return get< std::vector<int> >(key, value);
}

bool ProcessParams::getDoubleVector(const std::string& key, std::vector<double>& value) const
{
    return get< std::vector<double> >(key, value);
}

void ProcessParams::addString(const std::string& key, const std::string& value)
{
    add<std::string>(key, value);
}

void ProcessParams::addString(const std::string& key, std::string&& value)
{
    add<std::string>(key, std::move(value));
}

template <typename T>
void ProcessParams::add(const std::string& key, const T& value)
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

void ProcessParams::addInt(const std::string& key, int value)
{
    add<int>(key, value);
}

void ProcessParams::addDouble(const std::string& key, double value)
{
    add<double>(key, value);
}

void ProcessParams::addIntVector(const std::string& key, const std::vector<int>& value)
{
    add< std::vector<int> >(key, value);
}

void ProcessParams::addIntVector(const std::string& key, std::vector<int>&& value)
{
    add< std::vector<int> >(key, std::move(value));
}

void ProcessParams::addDoubleVector(const std::string& key, const std::vector<double>& value)
{
    add< std::vector<double> >(key, value);
}

void ProcessParams::addDoubleVector(const std::string& key, std::vector<double>&& value)
{
    add< std::vector<double> >(key, std::move(value));
}

void ProcessParams::clear()
{
    for (auto &item : m_data) delete item.second;
    m_data.clear();
    m_inputProcessName.clear();
}

string ProcessParams::serialize() const
{
    string ret;

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

string ProcessParams::serializeAsName() const
{
    string ret;

    if (!m_inputProcessName.empty()) {
        ret = m_inputProcessName;
    }
    for (auto& kv : m_data) {
        if (!ret.empty()) ret += '_';
        ret += kv.second->toString();
    }

    return ret;
}

void ProcessParams::deserialize(const string& serialized)
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

            deserializeParam(
                serialized.substr(keyPos, keyLen),
                serialized.substr(typePos, typeLen),
                serialized.substr(valPos, valLen));

            keyPos = nextKeyPos;
        } while (keyPos < maxPos);
    }
}

void ProcessParams::deserializeParam(
    const string& key,
    const string& type,
    const string& val)
{
    try {
        switch ((ParamType) std::stoi(type))
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

bool ProcessParams::validate(const MethodParams& definitions)
{
    return true;
}



}
