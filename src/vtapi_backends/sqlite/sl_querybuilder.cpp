
#include <vtapi/common/global.h>
#include "sl_querybuilder.h"

#define DEF_NO_SCHEMA   "!NO_SCHEMA!"
#define DEF_NO_TABLE    "!NO_TABLE!"
#define DEF_NO_COLUMN   "!NO_COLUMN!"
#define DEF_NO_VALUES   "!NO_VALUES!"
#define DEF_NO_QUERY    "!NO_QUERY!"


using namespace std;

namespace vtapi {


void SLQueryBuilder::reset()
{
    destroyQueryParam(_pquery_param);
}

void *SLQueryBuilder::createQueryParam() const
{
    return (void*) new SLparam();
}

void SLQueryBuilder::destroyQueryParam(void *param) const
{
    if (param) {
        delete (SLparam *) param;
    }
}

void *SLQueryBuilder::duplicateQueryParam(void *param) const
{
    if (param) {
        SLparam *p = (SLparam *) createQueryParam();
        if (p) p->database = ((SLparam *) param)->database;
        return p;
    }
    else {
        return NULL;
    }
}

string SLQueryBuilder::getGenericQuery() const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getSelectQuery(const string &groupby, const string &orderby, int limit, int offset) const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getInsertQuery() const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getUpdateQuery() const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getDeleteQuery() const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getCountQuery() const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getBeginQuery() const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getCommitQuery() const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getRollbackQuery() const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getDatasetCreateQuery(const string &name, const string &location, const string &friendly_name, const string &description) const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getDatasetResetQuery(const string &name) const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getDatasetDeleteQuery(const string &name) const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getMethodCreateQuery(const string &name, const TaskKeyDefinitions &keys_definition, const TaskParamDefinitions &params_definition, const string &description) const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getMethodDeleteQuery(const string &name) const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getTaskCreateQuery(const string &name, const string &dsname, const string &mtname, const string &params, const string &prereq_task, const string &outputs) const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getTaskDeleteQuery(const string &dsname, const string &taskname) const
{
    throw RuntimeException("unimplemented");
    return string();
}

string SLQueryBuilder::getLastInsertedIdQuery() const
{
    throw RuntimeException("unimplemented");
    return string();
}

bool SLQueryBuilder::keyBool(const string &key, bool value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyChar(const string &key, char value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyString(const string &key, const string &value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyStringVector(const string &key, const std::vector<string> &values, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyInt(const string &key, int value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyIntVector(const string &key, const std::vector<int> &values, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyInt8(const string &key, long long value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyInt8Vector(const string &key, const std::vector<long long> &values, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyFloat(const string &key, float value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyFloatVector(const string &key, const std::vector<float> &values, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyFloat8(const string &key, double value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyFloat8Vector(const string &key, const std::vector<double> &values, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyTimestamp(const string &key, const chrono::_V2::system_clock::time_point &value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyCvMat(const string &key, const cv::Mat &value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyPoint(const string &key, Point value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyPointVector(const string &key, const std::vector<Point> &values, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyIntervalEvent(const string &key, const IntervalEvent &value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyEdfDescriptor(const string &key, const EyedeaEdfDescriptor &value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyProcessStatus(const string &key, ProcessState::Status value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyBlob(const string &key, const std::vector<char> &data, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keySeqtype(const string &key, const string &value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::keyInouttype(const string &key, const string &value, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}


bool SLQueryBuilder::whereKeyNull(const string &key, bool isnull, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereBool(const string &key, bool value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereChar(const string &key, char value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereString(const string &key, const string &value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereStringVector(const string &key, const std::vector<string> &values, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereInt(const string &key, int value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereIntVector(const string &key, const std::vector<int> &values, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereInt8(const string &key, long long value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereInt8Vector(const string &key, const std::vector<long long> &values, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereFloat(const string &key, float value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereFloatVector(const string &key, const std::vector<float> &values, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereFloat8(const string &key, double value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereFloat8Vector(const string &key, const std::vector<double> &values, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereTimestamp(const string &key, const chrono::_V2::system_clock::time_point &value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;

}

bool SLQueryBuilder::wherePoint(const string &key, Point value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;

}

bool SLQueryBuilder::wherePointVector(const string &key, const std::vector<Point> &values, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;

}

bool SLQueryBuilder::whereProcessStatus(const string &key, ProcessState::Status value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;

}

bool SLQueryBuilder::whereTimeRange(const string &key_start, const string &key_length, const chrono::_V2::system_clock::time_point &value_start, const chrono::_V2::system_clock::time_point &value_end, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;

}

bool SLQueryBuilder::whereRegion(const string &key, const Box &value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;

}

bool SLQueryBuilder::whereExpression(const string &expression, const string &value, const string &oper)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereSeqtype(const string &key, const string &value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereInouttype(const string &key, const string &value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereEvent(const string &key, const string &taskname, const vector<string> &seqnames, const EventFilter &filter, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}

bool SLQueryBuilder::whereEdfDescriptor(const string &key, const EyedeaEdfDescriptor &value, const string &oper, const string &from)
{
    throw RuntimeException("unimplemented");
    return false;
}



string SLQueryBuilder::escapeColumn(const string& key, const string& table)
{
    if (table.empty()) return escapeIdent(key);
    else return escapeIdent(table) + "." + escapeIdent(key);
}

string SLQueryBuilder::escapeIdent(const string& ident)
{
    return "[" + ident + "]";
}

string SLQueryBuilder::escapeLiteral(const string& ident)
{
    string escaped  = ident;
    for (int i = 0; i < escaped.length(); ++i) {
        if (escaped.at(i) == '\'') {
            escaped.insert(i, "\'");
            i++;
        }
    }
    return "\'" + escaped + "\'";
}


}
