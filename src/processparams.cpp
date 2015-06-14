
#include <common/vtapi_global.h>
#include <data/vtapi_processparams.h>

using namespace std;

namespace vtapi {


ProcessParams::ProcessParams()
{ }

ProcessParams::ProcessParams(ProcessParams && other)
{
    m_inputProcessName = move(other.m_inputProcessName);
    m_data = move(other.m_data);
}

ProcessParams::ProcessParams(const string& serialized)
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
    m_inputProcessName = move(other.m_inputProcessName);
    m_data = move(other.m_data);
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

bool ProcessParams::empty() const
{
    return m_data.empty();
}

void ProcessParams::clear()
{
    for (auto& kv : m_data) delete kv.second;
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

void ProcessParams::deserializeParam(std::string key, std::string type, std::string val)
{
    try {
        switch ((ProcessParamType) std::stoi(type))
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

}
