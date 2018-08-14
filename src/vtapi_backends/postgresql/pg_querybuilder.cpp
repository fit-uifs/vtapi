
#include <set>
#include <vtapi/common/global.h>
#include <vtapi/common/defs.h>
#include "pg_querybuilder.h"

#define DEF_NO_SCHEMA   "!NO_SCHEMA!"
#define DEF_NO_TABLE    "!NO_TABLE!"
#define DEF_NO_COLUMN   "!NO_COLUMN!"
#define DEF_NO_QUERY    "!NO_QUERY!"


using namespace std;

namespace vtapi {


void PGQueryBuilder::reset()
{
    _listMain.clear();
    _listWhere.clear();
    _cnt_param = 0;
    destroyQueryParam(_pquery_param);
}

void *PGQueryBuilder::createQueryParam() const
{
    return (void*)PQparamCreate((PGconn *)_connection.getConnectionObject());
}

void PGQueryBuilder::destroyQueryParam(void *param) const
{
    if (param) PQparamClear((PGparam *)param);
}

void *PGQueryBuilder::duplicateQueryParam(void *param) const
{
    if (param)
        return PQparamDup((PGparam *) param);
    else
        return NULL;
}

string PGQueryBuilder::getGenericQuery() const
{
    if (_init_string.empty())
        VTLOG_WARNING("Empty query");

    return _init_string;
}

string PGQueryBuilder::getSelectQuery(const string& groupby, const string& orderby,
                                      int limit, int offset) const
{
    string tablesStr;
    string columnsStr;
    set<string> setTables;

    // select * from default table if not specified otherwise
    if (_listMain.empty()) {
        columnsStr = '*';
        tablesStr = constructTable(_defaultTable);
    }
    // or go through keys, construct tables/columns
    else {
        for (const MainItem & item : _listMain) {
            // add column after SELECT
            if (!columnsStr.empty())
                columnsStr += ',';
            columnsStr += constructColumn(item._key, item._table);
            columnsStr += " AS " + constructAlias(item._key);

            // add table after FROM
            string tab = constructTable(item._table);
            if (setTables.count(tab) == 0) {
                if (!tablesStr.empty())
                    tablesStr += ',';
                tablesStr += tab;
                setTables.insert(tab);
            }
        }
    }

    string queryString = "SELECT " + columnsStr;
    queryString += "\nFROM " + tablesStr;
    queryString += constructWhereClause();
    if (!groupby.empty())
        queryString += "\nGROUP BY " + groupby;
    if (!orderby.empty())
        queryString += "\nORDER BY " + orderby;
    if (limit > 0)
        queryString += "\nLIMIT " + toString<int>(limit);
    if (offset > 0)
        queryString += "\nOFFSET " + toString<int>(offset);
    queryString += ';';

    return queryString;
}

string PGQueryBuilder::getInsertQuery() const
{
    string tabStr = constructTable(_defaultTable);
    string intoStr;
    string valuesStr;

    if (_listMain.empty()) {
        VTLOG_ERROR("No values for INSERT query on table: " + tabStr);
        return DEF_NO_QUERY;
    }
    else {
        // construct columns/values part of the query
        for (const MainItem & item : _listMain) {
            if (!item._table.empty())
                tabStr = constructTable(item._table);

            if (!intoStr.empty()) {
                intoStr += ',';
                valuesStr += ',';
            }
            intoStr += escapeIdent(item._key);
            valuesStr += '$' + toString<unsigned int>(item._id_param);
        }

        // construct query
        string queryString = "INSERT INTO " + tabStr;
        queryString += '(' + intoStr + ')';
        queryString += "\nVALUES(" + valuesStr + ");";

        return queryString;
    }
}

string PGQueryBuilder::getUpdateQuery() const
{
    // otherwise use keys to construct tables/columns part of the query
    string tabStr = constructTable(_defaultTable);
    string setStr;

    if (_listMain.empty()) {
        VTLOG_ERROR("No columns to SET in UPDATE query on table: " + tabStr);
        return DEF_NO_QUERY;
    }
    else {
        // construct set part of the query
        for (const MainItem & item : _listMain) {
            if (!item._table.empty())
                tabStr = constructTable(item._table);

            if (!setStr.empty())
                setStr += ',';

            setStr += constructColumnNoTable(item._key);
            setStr += "=$";
            setStr += toString<unsigned int>(item._id_param);
        }

        // disallow accidental set on all rows
        string where = constructWhereClause();
        if (where.empty()) {
            VTLOG_ERROR("UPDATE query without WHERE on table: " + tabStr);
            return DEF_NO_QUERY;
        }
        else {
            string queryString = "UPDATE " + tabStr;
            queryString += "\nSET " + setStr;
            queryString += constructWhereClause();
            queryString += ';';

            return queryString;
        }
    }
}

string PGQueryBuilder::getDeleteQuery() const
{
    string tabStr = constructTable(_defaultTable);

    // disallow accidental wipe
    string where = constructWhereClause();
    if (where.empty()) {
        VTLOG_ERROR("DELETE query without WHERE on table: " + tabStr);
        return DEF_NO_QUERY;
    }
    else {
        string queryString = "DELETE FROM " + tabStr;
        queryString += where;
        queryString += ';';

        return queryString;
    }
}

string PGQueryBuilder::getCountQuery() const
{
    string queryString = getSelectQuery(string(), string(), 0, 0);
    size_t fromPos = queryString.find("\nFROM ");

    if (fromPos != string::npos) {
        return "SELECT COUNT(*) AS count" + queryString.substr(fromPos);
    }
    else {
        VTLOG_ERROR("Failed to get COUNT query");
        return DEF_NO_QUERY;
    }
}

string PGQueryBuilder::getBeginQuery() const
{
    return "BEGIN;";
}

string PGQueryBuilder::getCommitQuery() const
{
    return "COMMIT;";
}

string PGQueryBuilder::getRollbackQuery() const
{
    return "ROLLBACK;";
}

string PGQueryBuilder::getDatasetCreateQuery(const string& name,
                                             const string& location,
                                             const string& friendly_name,
                                             const string& description) const
{
    //SELECT public.VT_dataset_create('demo', 'demo/', 'Pre-generated dataset', 'This dataset is for demonstration purposes only');

    string q;
    q += "SELECT ";
    q += def_fnc_ds_create;
    q += '(';
    q += escapeLiteral(name);
    q += ',';
    q += escapeLiteral(location);
    q += ',';
    q += escapeLiteral(friendly_name);
    q += ',';
    q += escapeLiteral(description);
    q += ");";

    return q;
}

string PGQueryBuilder::getDatasetResetQuery(const string& name) const
{
    //SELECT public.VT_dataset_truncate('demo');

    string q;
    q += "SELECT ";
    q += def_fnc_ds_reset;
    q += '(';
    q += escapeLiteral(name);
    q += ");";

    return q;
}

string PGQueryBuilder::getDatasetDeleteQuery(const string& name) const
{
    //SELECT public.VT_dataset_drop('demo');

    string q;
    q += "SELECT ";
    q += def_fnc_ds_delete;
    q += '(';
    q += escapeLiteral(name);
    q += ");";

    return q;
}

string PGQueryBuilder::getMethodCreateQuery(const string& name,
                                            const TaskKeyDefinitions &keys_definition,
                                            const TaskParamDefinitions &params_definition,
                                            const string &description) const
{
    //TODO: creating methods
    return "";
}

string PGQueryBuilder::getMethodDeleteQuery(const string& name) const
{
    string q;
    q += "SELECT ";
    q += def_fnc_ds_delete;
    q += '(';
    q += escapeLiteral(name);
    q += ',';
    q += "TRUE";
    q += ");";

    return q;
}

string PGQueryBuilder::getTaskCreateQuery(const string &name,
                                          const string& dsname,
                                          const string& mtname,
                                          const string &params,
                                          const string& prereq_task,
                                          const string &outputs) const
{
    string q;
    q += "SELECT ";
    q += def_fnc_task_create;
    q += '(';
    q += escapeLiteral(name);
    q += ',';
    q += escapeLiteral(mtname);
    q += ',';
    q += escapeLiteral(params);
    q += ',';
    q += prereq_task.empty() ? "NULL" : escapeLiteral(prereq_task);
    q += ',';
    q += outputs.empty() ? "NULL" : escapeLiteral(outputs);
    q += ',';
    q += escapeLiteral(dsname);
    q += ");";

    return q;
}

string PGQueryBuilder::getTaskDeleteQuery(const string &dsname,
                                          const string &taskname) const
{
    string q;
    q += "SELECT ";
    q += def_fnc_task_delete;
    q += '(';
    q += escapeLiteral(taskname);
    q += ',';
    q += "TRUE";
    q += ',';
    q += escapeLiteral(dsname);
    q += ");";

    return q;
}

string PGQueryBuilder::getLastInsertedIdQuery() const
{
    return "SELECT lastval();";
}

template<typename T>
bool PGQueryBuilder::keySingleValue(const string& key,
                                    const T& value,
                                    const char *type,
                                    const string& from)
{
    uint idParam = 0;

    do {
        if (key.empty()) break;

        idParam = addToParam(type, value);
        if (!idParam) break;

        _listMain.push_back(MainItem(key, from, idParam));
    } while (0);

    return (idParam > 0);
}

template<typename T>
bool PGQueryBuilder::keyVector(const string& key,
                              const vector<T>& values,
                              const char *type,
                              const char* type_arr,
                              const string& from)
{
    uint idParam = 0;
    PGarray arr = { 0 };

    do {
        if (key.empty() || values.empty()) break;

        arr.param = PQparamCreate((PGconn *)_connection.getConnectionObject());
        if (!arr.param) break;

        // put the array elements
        for (int i = 0; i < values.size(); ++i) {
            PQputf(arr.param, type, values[i]);
        }

        idParam = addToParam(type_arr, &arr);
        if (!idParam) break;

        _listMain.push_back(MainItem(key, from, idParam));
    } while (0);

    if (arr.param) PQparamClear(arr.param);

    return (idParam > 0);
}

bool PGQueryBuilder::keyFrom(const string& table, const string& column)
{
    if (column.empty()) {
        return false;
    }
    else {
        if (_defaultTable.empty())
            _defaultTable = table;
        _listMain.push_back(MainItem(column, table, 0));
        return true;
    }
}

bool PGQueryBuilder::keyBool(const string &key, bool value, const string &from)
{
    return keySingleValue(key, value, "%bool", from);
}

bool PGQueryBuilder::keyChar(const string &key, char value, const string &from)
{
    return keySingleValue(key, value, "%char", from);
}

bool PGQueryBuilder::keyString(const string& key, const string& value, const string& from)
{
    return keySingleValue(key, value.c_str(), "%varchar", from);
}

bool PGQueryBuilder::keyStringVector(const string& key, const vector<string>& values, const string& from)
{
    //TODO: string vector
    return false;
    //return keyVector(key, values, "%varchar", from);
}

bool PGQueryBuilder::keyInt(const string& key, int value, const string& from)
{
    return keySingleValue(key, value, "%int4", from);
}

bool PGQueryBuilder::keyIntVector(const string& key, const vector<int> &values, const string& from)
{
    return keyVector(key, values, "%int4", "%int4[]", from);
}

bool PGQueryBuilder::keyInt8(const string& key, long long value, const string& from)
{
    return keySingleValue(key, value, "%int8", from);
}

bool PGQueryBuilder::keyInt8Vector(const string& key, const vector<long long> &values, const string& from)
{
    return keyVector(key, values, "%int8", "%int8[]", from);
}

bool PGQueryBuilder::keyFloat(const string& key, float value, const string& from)
{
    return keySingleValue(key, value, "%float4", from);
}

bool PGQueryBuilder::keyFloatVector(const string& key, const vector<float> &values, const string& from)
{
    return keyVector(key, values, "%float4", "%float4[]", from);
}

bool PGQueryBuilder::keyFloat8(const string& key, double value, const string& from)
{
    return keySingleValue(key, value, "%float8", from);
}

bool PGQueryBuilder::keyFloat8Vector(const string& key, const vector<double> &values, const string& from)
{
    return keyVector(key, values, "%float8", "%float8[]", from);
}

bool PGQueryBuilder::keyTimestamp(const string& key, const std::chrono::system_clock::time_point &value, const string& from)
{
    PGtimestamp ts = UnixTimeToTimestamp(value);
    return keySingleValue(key, &ts, "%timestamp", from);
}

bool PGQueryBuilder::keyCvMat(const string& key, const cv::Mat& value, const string& from)
{
    bool ret = true;
    PGparam *cvmat = NULL;
    PGarray mat_dims = { 0 };

    do {
        // create dimensions array
        mat_dims.param = PQparamCreate((PGconn *)_connection.getConnectionObject());
        if (!mat_dims.param) {
            ret = false;
            break;
        }

        for (int i = 0; i < value.dims; i++) {
            ret &= (0 != PQputf(mat_dims.param, "%int4", value.size[i]));
        }
        if (!ret) break;

        // matrix data
        PGbytea mat_data = { (int) (value.dataend - value.datastart), (char*) value.data };

        // create cvmat composite
        cvmat = PQparamCreate((PGconn *) _connection.getConnectionObject());
        if (!cvmat) {
            ret = false;
            break;
        }

        ret = (0 != PQputf(cvmat, "%int4 %int4[] %bytea*",
                                (PGint4) value.type(), &mat_dims, &mat_data));
        if (!ret) break;

        ret = keySingleValue(key, cvmat, "%public.cvmat", from);
    } while (0);

    if (cvmat) PQparamClear(cvmat);
    if (mat_dims.param) PQparamClear(mat_dims.param);

    return ret;
}

bool PGQueryBuilder::keyPoint(const string& key, Point value, const string& from)
{
    PGpoint pt = { value.x, value.y };
    return keySingleValue(key, &pt, "%point", from);
}

bool PGQueryBuilder::keyPointVector(const string& key, const vector<Point> &values, const string& from)
{
    //TODO: point vector (must pass pointers)
    vector<PGpoint*>pts;
//    for (size_t i = 0; i < values.size(); i++)
//        pts[i] = { values[i].x, values[i].y };
    return keyVector(key, pts, "%point", "%point[]", from);
}

bool PGQueryBuilder::keyIntervalEvent(const string& key, const IntervalEvent& value, const string& from)
{
    bool ret = true;
    PGparam * event = NULL;

    do {
        // user data
        PGbytea data = { (int) value.user_data.size(), (char*) value.user_data.data() };

        // create interval event composite
        event = PQparamCreate((PGconn *) _connection.getConnectionObject());
        if (!event) {
            ret = false;
            break;
        }

        ret = (0 != PQputf(event, "%int4 %int4 %bool %box %float8 %bytea*",
                                (PGint4) value.group_id, (PGint4) value.class_id, (PGbool) value.is_root,
                                (PGbox *) & value.region, (PGfloat8) value.score, &data));
        if (!ret) break;

        ret = keySingleValue(key, event, "%public.vtevent", from);
    } while (0);

    if (event) PQparamClear(event);

    return ret;
}


bool PGQueryBuilder::keyEdfDescriptor(const string &key, const EyedeaEdfDescriptor &value, const string &from)
{
    bool ret = true;
    PGparam *edfdesc = NULL;

    do {
        // user data
        PGbytea data = { (int) value.data.size(), (char*) value.data.data() };

        // create interval event composite
        edfdesc = PQparamCreate((PGconn *) _connection.getConnectionObject());
        if (! edfdesc) {
            ret = false;
            break;
        }

        ret = (0 != PQputf(edfdesc, "%int4 %bytea*",
                                (PGint4) value.version, &data));

        if (! ret) break;

        ret = keySingleValue(key, edfdesc, "%public.eyedea_edfdescriptor", from);
    } while (0);

    if (edfdesc) PQparamClear(edfdesc);

    return ret;
}


bool PGQueryBuilder::keyProcessStatus(const string& key, ProcessState::Status value, const string& from)
{
    return keySingleValue(key, ProcessState::toStatusString(value).c_str(), "%public.pstatus", from);
}

bool PGQueryBuilder::keyBlob(const string& key, const vector<char> &data, const string &from)
{
    PGbytea bytea = { static_cast<int>(data.size()), const_cast<char*>(data.data()) };
    return keySingleValue(key, &bytea, "%bytea", from);
}

bool PGQueryBuilder::keySeqtype(const string& key, const string& value, const string& from)
{
    return checkSeqtype(value) && keySingleValue(key, value.c_str(), "%public.seqtype", from);
}

bool PGQueryBuilder::keyInouttype(const string& key, const string& value, const string& from)
{
    return checkInouttype(value) && keySingleValue(key, value.c_str(), "%public.inouttype", from);
}

template<typename T>
bool PGQueryBuilder::whereSingleValue(const string& key, const T& value, const char *type, const string& oper, const string& from)
{
    uint idParam = 0;

    do {
        if (key.empty()) break;

        idParam = addToParam(type, value);
        if (!idParam) break;

        _listWhere.push_back(WhereItem(key, from, oper, idParam));
    } while (0);

    return (idParam > 0);
}

template<typename T>
string PGQueryBuilder::serializeVector(const vector<T>& values)
{
    string listval;
    listval += '(';
    for (size_t i = 0; i < values.size(); i++) {
        if (listval.length() > 1) listval += ',';
        listval += escapeLiteral(toString(values[i]));
    }
    if (listval.length() < 2) listval += "\'\'";
    listval += ')';

    return listval;
}

bool PGQueryBuilder::whereBool(const string &key, bool value, const string &oper, const string &from)
{
    return whereSingleValue(key, value, "%bool", oper, from);
}

bool PGQueryBuilder::whereChar(const string &key, char value, const string &oper, const string &from)
{
    return whereSingleValue(key, value, "%char", oper, from);
}

bool PGQueryBuilder::whereString(const string& key, const string& value, const string& oper, const string& from)
{
    return whereSingleValue(key, value.c_str(), "%varchar", oper, from);
}

bool PGQueryBuilder::whereStringVector(const string &key, const vector<string> &values, const string &oper, const string &from)
{
    string exp = constructColumn(key, from);
    string listval = serializeVector(values);
    _listWhere.push_back(WhereItem(exp, oper, listval));
}

bool PGQueryBuilder::whereInt(const string& key, int value, const string& oper, const string& from)
{
    return whereSingleValue(key, value, "%int4", oper, from);
}

bool PGQueryBuilder::whereIntVector(const string &key, const vector<int> &values, const string &oper, const string &from)
{
    string exp = constructColumn(key, from);
    string listval = serializeVector(values);
    _listWhere.push_back(WhereItem(exp, oper, listval));
}

bool PGQueryBuilder::whereInt8(const string& key, long long value, const string& oper, const string& from)
{
    return whereSingleValue(key, value, "%int8", oper, from);
}

bool PGQueryBuilder::whereInt8Vector(const string &key, const vector<long long> &values, const string &oper, const string &from)
{
    string exp = constructColumn(key, from);
    string listval = serializeVector(values);
    _listWhere.push_back(WhereItem(exp, oper, listval));
}

bool PGQueryBuilder::whereFloat(const string& key, float value, const string& oper, const string& from)
{
    return whereSingleValue(key, value, "%float4", oper, from);
}

bool PGQueryBuilder::whereFloatVector(const string &key, const vector<float> &values, const string &oper, const string &from)
{
    string exp = constructColumn(key, from);
    string listval = serializeVector(values);
    _listWhere.push_back(WhereItem(exp, oper, listval));
}

bool PGQueryBuilder::whereFloat8(const string& key, double value, const string& oper, const string& from)
{
    return whereSingleValue(key, value, "%float8", oper, from);
}

bool PGQueryBuilder::whereFloat8Vector(const string &key, const vector<double> &values, const string &oper, const string &from)
{
    string exp = constructColumn(key, from);
    string listval = serializeVector(values);
    _listWhere.push_back(WhereItem(exp, oper, listval));
}

bool PGQueryBuilder::whereTimestamp(const string& key, const std::chrono::system_clock::time_point& value, const string& oper, const string& from)
{
    PGtimestamp ts = UnixTimeToTimestamp(value);
    return whereSingleValue(key, &ts, "%timestamp", oper, from);
}

bool PGQueryBuilder::wherePoint(const string& key, Point value, const string& oper, const string& from)
{
    PGpoint pt = { value.x, value.y };
    return whereSingleValue(key, &pt, "%point", oper, from);
}

bool PGQueryBuilder::wherePointVector(const string &key, const vector<Point> &values, const string &oper, const string &from)
{
    //TODO: point vector
   return false;
}

bool PGQueryBuilder::whereProcessStatus(const string& key, ProcessState::Status value, const string& oper, const string& from)
{
    return whereSingleValue(key, ProcessState::toStatusString(value).c_str(), "%public.pstatus", oper, from);
}

bool PGQueryBuilder::whereTimeRange(const string& key_start,
                                    const string& key_length,
                                    const chrono::system_clock::time_point &value_start,
                                    const chrono::system_clock::time_point &value_end,
                                    const string& oper,
                                    const string& from)
{
    bool bRet = true;

    do {
        if (key_start.empty() || key_length.empty()) {
            bRet = false;
            break;
        }

        string exp = "'[" + toString(value_start) + ',' + toString(value_end) + "]'";
        string value = "public.tsrange(" +
                constructColumn(key_start, from) + ',' +
                constructColumn(key_length, from) + ')';

        _listWhere.push_back(WhereItem(exp, oper, value));
    } while (0);

    return bRet;
}

bool PGQueryBuilder::whereRegion(const string& key, const Box& value, const string& oper, const string& from)
{
    PGbox box = { { value.high.x, value.high.y }, { value.low.x, value.low.y } };
    return whereSingleValue(key, &box, "%box", oper, from);
}

bool PGQueryBuilder::whereExpression(const string& expression, const string& value, const string& oper)
{
    if (!expression.empty() && !value.empty()) {
        _listWhere.push_back(WhereItem(expression, oper, value));
        return true;
    }
    else {
        return false;
    }
}

bool PGQueryBuilder::whereSeqtype(const string& key, const string& value, const string& oper, const string& from)
{
    return checkSeqtype(value) && whereSingleValue(key, value.c_str(), "%public.seqtype", oper, from);
}

bool PGQueryBuilder::whereInouttype(const string& key, const string& value, const string& oper, const string& from)
{
    return checkInouttype(value) && whereSingleValue(key, value.c_str(), "%public.inouttype", oper, from);
}

bool PGQueryBuilder::whereEvent(const string &key, const string &taskname, const vector<string> &seqnames, const EventFilter &filter, const string& from)
{
//id = ANY (public.VT_filtered_events('demo.demo2_out', 'event', 'task_demo2_1', 'video1', '(,,,,,,"(0,0),(0,0)")'))
    string val = "ANY (" + def_fnc_event_filter + "(" +
            escapeLiteral(constructTable(from)) + "," + escapeLiteral(key) + "," +
            escapeLiteral(taskname) + "," + escapeLiteralArray(seqnames) + "," +
            "\'" + toString<EventFilter>(filter) + "\'))";
    return whereExpression("(" + key + ").group_id", val, "=");
}


template<typename T>
unsigned int PGQueryBuilder::addToParam(const char* type, const T& value)
{
    uint ret = 0;

    do {
        if (!_pquery_param && !(_pquery_param = createQueryParam())) break;

        if (PQputf((PGparam *)_pquery_param, type, value) == 0) {
            VTLOG_WARNING("Failed to add value to query: " + toString<T>(value) + " (" + PQgeterror() + ")");
            break;
        }

        ret = ++_cnt_param;
    } while (0);

    return ret;
}

string PGQueryBuilder::constructTable(const string& table, const string& schema) const
{
    const string& tab = (!table.empty() ? table : _defaultTable);

    // no table specified
    if (tab.empty()) {
        VTLOG_ERROR("No table was specified for query");
        return DEF_NO_TABLE;
    }
    // no DB schema in table name, use function arg or default schema
    else if (tab.find('.') == string::npos) {
        if (!schema.empty()) {
            return schema + '.' + tab;
        }
        else if (!_defaultSchema.empty()) {
            return _defaultSchema + '.' + tab;
        }
        else {
            VTLOG_ERROR("No schema was specified for query");
            return DEF_NO_SCHEMA + '.' + tab;
        }
    }
    // DB schema in table name
    else {
        return tab;
    }
}

string PGQueryBuilder::constructColumn(const string& column, const string& table) const
{
    if (column.empty()) {
        VTLOG_ERROR("No column was specified for query");
        return DEF_NO_COLUMN;
    }
    else if (column[0] == '*') {
        return constructTable(table) + ".*";
    }
    else {
        // get column table first
        string tab;
        size_t dotPos = column.find('.');
        if (dotPos == string::npos) {
            tab = constructTable(table);
        }
        else {
            tab = constructTable(column.substr(0, dotPos));
        }

        // check if column has unescapable part
        size_t startPos = (dotPos == string::npos ? 0 : dotPos + 1);
        size_t charPos  = column.find_first_of(":[(,", startPos);

        // escape full column
        if (charPos == string::npos) {
            if (startPos == 0) {
                return tab + '.' + escapeIdent(column.c_str());
            }
            else {
                string col = column.substr(startPos, string::npos);
                if (col.empty()) {
                    VTLOG_ERROR("No column was specified for query");
                    col = DEF_NO_COLUMN;
                }

                return tab + '.' + escapeIdent(col);
            }
        }
        // escape column part
        else {
            string col;
            if (startPos == 0) {
                col = column.substr(0, charPos);
            }
            else {
                col = column.substr(startPos, charPos - startPos);
            }
            if (col.empty()) {
                VTLOG_ERROR("No column was specified for query");
                col = DEF_NO_COLUMN;
            }

            // composite members are accessed through comma, special escape
            if (column[charPos] == ',') {
                string rest = column.substr(charPos + 1, string::npos);
                return '(' + tab + '.' + escapeIdent(col) + ')' + '.' + escapeIdent(rest);
            }
                // escape column part only
            else {
                string rest = column.substr(charPos, string::npos);
                return tab + '.' + escapeIdent(col) + rest;
            }
        }
    }
}

string PGQueryBuilder::constructColumnNoTable(const string& column) const
{
    size_t charPos = column.find_first_of(",.");
    if (charPos > 0 && charPos < column.length() - 1) {
        return escapeIdent(column.substr(0, charPos)) +
                '.' +
                escapeIdent(column.substr(charPos + 1, string::npos));
    }
    else {
        return escapeIdent(column);
    }
}

string PGQueryBuilder::constructAlias(const string& column) const
{
    return column.substr(0, column.find_first_of(":[(,."));
}

string PGQueryBuilder::escapeIdent(const string& ident) const
{
    char *escaped = PQescapeIdentifier((PGconn *) _connection.getConnectionObject(),
                                       ident.c_str(),
                                       ident.length());
    string ret = escaped;
    PQfreemem(escaped);
    return ret;
}

string PGQueryBuilder::escapeLiteral(const string& literal) const
{
    char *escaped = PQescapeLiteral((PGconn *)_connection.getConnectionObject(),
                                    literal.c_str(),
                                    literal.length());
    string ret = escaped;
    PQfreemem(escaped);
    return ret;
}

string PGQueryBuilder::escapeLiteralArray(const vector<string> &literals) const
{
    std::string str;
    str += '{';
    for (size_t i = 0; i < literals.size(); i++) {
        if (i > 0) str += ',';
        str += escapeLiteral(literals[i]);
    }
    str += '}';

    std::replace(str.begin(), str.end(), '\'', '\"');

    return "\'" + str + "\'";
}

string PGQueryBuilder::constructWhereClause() const
{
    string where;

    if (!_listWhere.empty()) {
        for (const WhereItem & item : _listWhere) {

            if (!where.empty()) where += "\nAND\n";

            // bind key to PGparam value
            if (item._id_param > 0) {
                where += constructColumn(item._key, item._table);
                where += ' ' + item._oper + ' ';
                where += '$' + toString<unsigned int>(item._id_param);
            }
            // use key as custom expression
            else {
                where += item._key;
                where += ' ' + item._oper + ' ';
                where += item._value;
            }
        }

        if (!where.empty())
            where = "\nWHERE\n" + where;
    }

    return where;
}

PGtimestamp PGQueryBuilder::UnixTimeToTimestamp(const chrono::system_clock::time_point & utime) const
{
    std::time_t tmp = chrono::system_clock::to_time_t(utime);
    std::tm ts = *std::gmtime(&tmp);

    PGtimestamp ret = { 0 };
    ret.date.year = ts.tm_year + 1900;
    ret.date.mon  = ts.tm_mon;
    ret.date.mday = ts.tm_mday;
    ret.time.hour = ts.tm_hour;
    ret.time.min  = ts.tm_min;
    ret.time.sec  = ts.tm_sec;

    auto secs = chrono::duration_cast<chrono::seconds>(utime.time_since_epoch());
    auto usecs = chrono::duration_cast<chrono::microseconds>(utime.time_since_epoch());
    ret.time.usec = chrono::duration_cast<chrono::microseconds>(usecs - secs).count();

    return ret;
}

}
