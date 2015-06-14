
#include <set>
#include <common/vtapi_global.h>
#include <backends/vtapi_querybuilder.h>

#if HAVE_POSTGRESQL

// postgres data transfer format: 0=text, 1=binary
#define PG_FORMAT           1

#define DEF_NO_SCHEMA   "!NO_SCHEMA!"
#define DEF_NO_TABLE    "!NO_TABLE!"
#define DEF_NO_COLUMN   "!NO_COLUMN!"
#define DEF_NO_VALUES   "!NO_VALUES!"
#define DEF_NO_QUERY    "!NO_QUERY!"

using namespace std;

namespace vtapi {


PGQueryBuilder::PGQueryBuilder(const PGBackendBase &base, void *connection, const string& initString) :
    QueryBuilder (connection, initString),
    PGBackendBase(base)
{
    thisClass = "PGQueryBuilder";
    m_cntParam = 0;
}

PGQueryBuilder::~PGQueryBuilder()
{
    destroyQueryParam(queryParam);    
}

void PGQueryBuilder::reset()
{
    m_listMain.clear();
    m_listWhere.clear();
    m_cntParam = 0;
    destroyQueryParam(queryParam);    
}

void *PGQueryBuilder::createQueryParam()
{
    return (void*) pqt.PQparamCreate((PGconn *) connection);
}

void PGQueryBuilder::destroyQueryParam(void *param)
{
    if (param) {
        pqt.PQparamClear((PGparam *) param);
    }
}

void *PGQueryBuilder::duplicateQueryParam(void *param)
{
    if (param) {
        return pqt.PQparamDup((PGparam *) param);
    }
    else {
        return NULL;
    }
}

string PGQueryBuilder::getGenericQuery()
{
    return initString;
}

string PGQueryBuilder::getSelectQuery(const string& groupby, const string& orderby, const int limit, const int offset)
{
    string queryString;

    // use initialization string if specified
    if (!initString.empty()) {
        queryString = initString;
        
        // replace trailing ;
        for (int i = queryString.length() - 1; i >= 0 && queryString[i] == ';'; i--) {
            queryString[i] = ' ';
        }
    }
    // otherwise use keys to construct tables/columns part of the query
    else {
        string tableStr;
        string tablesStr;
        string columnsStr;
        set<string> setTables;
        
        // go through keys, construct tables/columns
        for (MAIN_LIST_IT it = m_listMain.begin(); it != m_listMain.end(); it++) {
            TKey &key = (*it).key;

            // add column after SELECT
            string col  = constructColumn(key.m_key, key.m_from);
            if (!columnsStr.empty()) {
                columnsStr += ',';
            }
            columnsStr += col;
            if (key.m_key[0] != '*') {
                columnsStr += " AS " + constructAlias(key.m_key);
            }

            // add table after FROM
            string tab = constructTable(key.m_from);
            if (setTables.count(tab) == 0) {
                if (!tablesStr.empty()) tablesStr += ',';
                tablesStr += tab;
                setTables.insert(tab);
            }
        }

        bool bError = columnsStr.empty() || tablesStr.empty();
        if (columnsStr.empty()) columnsStr  = DEF_NO_COLUMN;
        if (tablesStr.empty())  tablesStr   = DEF_NO_TABLE;
        
        queryString = "SELECT " + columnsStr + "\nFROM " + tablesStr;

        if (bError) {
            logger->warning(201, "incomplete query: " + queryString, thisClass + "::getSelectQuery()");
            return DEF_NO_QUERY;
        }
    }
    
    // construct WHERE clause
    if (!m_listWhere.empty()) {
        string whereStr;
        
        for (WHERE_LIST_IT it = m_listWhere.begin(); it != m_listWhere.end(); it++) {
            TKey &key = (*it).key;

            if (!whereStr.empty()) whereStr += "\nAND\n";

            // bind key to PGparam value
            if ((*it).idParam > 0) {
                whereStr += constructColumn(key.m_key, key.m_from);
                whereStr += ' ' + (*it).oper + ' ';
                whereStr += '$' + toString((*it).idParam);
            }
            // use key as custom expression
            else {
                whereStr += (key.m_key);
                whereStr += ' ' + (*it).oper + ' ';
                whereStr += (*it).value;
            }
        }

        if (!whereStr.empty()) {
            queryString += "\nWHERE\n" + whereStr;
        }
    }
    
    // construct rest of the query
    if (!groupby.empty()) {
        queryString += "\nGROUP BY " + groupby;
    }
    if (!orderby.empty()) {
        queryString += "\nORDER BY " + orderby;
    }
    if (limit > 0) {
        queryString += "\nLIMIT " + toString(limit);
    }
    if (offset > 0) {
        queryString += "\nOFFSET " + toString(offset);
    }
    queryString += ';';

    return queryString;
}

string PGQueryBuilder::getInsertQuery()
{
    if (!initString.empty()) {
        return initString;
    }
    else {
        string &tab = defaultTable;
        string tableStr;
        string intoStr;
        string valuesStr;

        // construct columns/values part of the query
        for (MAIN_LIST_IT it = m_listMain.begin(); it != m_listMain.end(); it++) {
            TKey &key = (*it).key;
            
            if (!key.m_from.empty()) tab = key.m_from;
            
            if (it != m_listMain.begin()) intoStr += ',';
            intoStr     += escapeIdent(key.m_key);
            
            if (it != m_listMain.begin()) valuesStr += ',';
            valuesStr   += '$' + toString((*it).idParam);
        }

        bool bError = tab.empty() || intoStr.empty() || valuesStr.empty();
        if (intoStr.empty())    intoStr  = DEF_NO_COLUMN;
        if (valuesStr.empty())  valuesStr= DEF_NO_VALUES;

        tableStr = constructTable(tab);

        // construct query
        string queryString = "INSERT INTO " + tableStr + "(" + intoStr + ")\nVALUES(" + valuesStr + ");";

        if (bError) {
            logger->warning(202, "incomplete query: " + queryString, thisClass + "::getInsertQuery()");
            return DEF_NO_QUERY;
        }

        return queryString;
    }
}

string PGQueryBuilder::getUpdateQuery()
{
    string queryString;

    // use initialization string if specified
    if (!initString.empty()) {
        queryString = initString;

        // replace trailing ; with spaces
        for (int i = queryString.length() - 1; i >= 0; i--) {
            queryString[i] = ' ';
        }
    }
    // otherwise use keys to construct tables/columns part of the query
    else {
        string &tab = defaultTable;
        string tableStr;
        string setStr;

        // construct set part of the query
        for (MAIN_LIST_IT it = m_listMain.begin(); it != m_listMain.end(); it++) {
            TKey &key = (*it).key;

            if (!key.m_from.empty()) tab = key.m_from;
            
            if (!setStr.empty()) setStr += ',';
            setStr += constructColumnNoTable(key.m_key) + "=$" + toString((*it).idParam);
        }

        bool bError = tab.empty() || setStr.empty();
        if (setStr.empty()) setStr = DEF_NO_COLUMN;

        tableStr = constructTable(tab);
        
        queryString = "UPDATE " + tableStr + "\nSET " + setStr;

        if (bError) {
            logger->warning(203, "incomplete query: " + queryString, thisClass + "::getUpdateQuery()");
            return DEF_NO_QUERY;
        }
    }
    
    // construct WHERE clause
    if (!m_listWhere.empty()) {
        string whereStr;
        
        for (WHERE_LIST_IT it = m_listWhere.begin(); it != m_listWhere.end(); it++) {
            TKey &key = (*it).key;

            if (!whereStr.empty()) whereStr += " AND ";

            if ((*it).idParam > 0) {
                whereStr += constructColumn(key.m_key, key.m_from);
                whereStr += ' ' + (*it).oper + ' ';
                whereStr += "$" + toString((*it).idParam);
            }
            else {
                whereStr += (key.m_key);
                whereStr += ' ' + (*it).oper + ' ';
                whereStr += (*it).value;
            }
        }

        if (!whereStr.empty()) {
            queryString += "\nWHERE\n" + whereStr;
        }
    }

    queryString += ";";

    return queryString;
}

string PGQueryBuilder::getCountQuery()
{
    string queryString = getSelectQuery("", "", 0, 0);
    size_t fromPos = queryString.find("\nFROM ");
    
    if (fromPos != string::npos) {
        return "SELECT COUNT(*) AS count" + queryString.substr(fromPos);
    }
    else {
        return DEF_NO_QUERY;
    }
}

string PGQueryBuilder::getBeginQuery()
{
    return "BEGIN;";
}

string PGQueryBuilder::getCommitQuery()
{
    return "COMMIT;";
}

string PGQueryBuilder::getRollbackQuery()
{
    return "ROLLBACK;";
}

template<typename T>
bool PGQueryBuilder::keySingleValue(const string& key, T value, const char *type, const string& from)
{
    uint idParam = 0;

    do {
        if (key.empty()) break;

        idParam = addToParam(type, value);
        if (!idParam) break;

        m_listMain.push_back(MAIN_ITEM(key, from, idParam));
    } while (0);

    return (idParam > 0);
}

template<typename T>
bool PGQueryBuilder::keyArray(const std::string& key, T* values, const int size, const char *type, const char* type_arr, const std::string& from)
{
    uint idParam = 0;
    PGarray arr = { 0 };

    do {
        if (key.empty() || !values || size <= 0) break;

        arr.param = pqt.PQparamCreate((PGconn *) connection);
        if (!arr.param) break;

        // put the array elements
        for (int i = 0; i < size; ++i) {
            pqt.PQputf(arr.param, type, values[i]);
        }

        idParam = addToParam(type_arr, &arr);
        if (!idParam) break;

        m_listMain.push_back(MAIN_ITEM(key, from, idParam));
    } while (0);

    if (arr.param) pqt.PQparamClear(arr.param);

    return (idParam > 0);
}

bool PGQueryBuilder::keyFrom(const string& table, const string& column)
{
    if (column.empty()) {
        return false;
    }
    else {
        if (defaultTable.empty()) defaultTable = table;
        m_listMain.push_back(MAIN_ITEM(column, table, 0));
        return true;
    }
}

bool PGQueryBuilder::keyString(const string& key, const string& value, const string& from)
{
    return keySingleValue(key, value.c_str(), "%varchar", from);
}

bool PGQueryBuilder::keyStringA(const string& key, string* values, const int size, const string& from)
{
    uint idParam = 0;
    PGarray arr = { 0 };

    do {
        if (key.empty() || !values || size <= 0) break;

        arr.param = pqt.PQparamCreate((PGconn *) connection);
        if (!arr.param) break;

        // put the array elements
        for (int i = 0; i < size; ++i) {
            pqt.PQputf(arr.param, "%varchar", (PGvarchar)values[i].c_str());
        }

        idParam = addToParam("%varchar[]", &arr);
        if (!idParam) break;

        m_listMain.push_back(MAIN_ITEM(key, from, idParam));
    } while (0);

    if (arr.param) pqt.PQparamClear(arr.param);

    return (idParam > 0);
}

bool PGQueryBuilder::keyInt(const string& key, int value, const string& from)
{
    return keySingleValue(key, value, "%int4", from);
}

bool PGQueryBuilder::keyIntA(const string& key, int* values, const int size, const string& from)
{
    return keyArray(key, values, size, "%int4", "%int4[]", from);
}

bool PGQueryBuilder::keyFloat(const string& key, float value, const string& from)
{
    return keySingleValue(key, value, "%float4", from);
}

bool PGQueryBuilder::keyFloatA(const string& key, float* values, const int size, const string& from)
{
    return keyArray(key, values, size, "%float4", "%float4[]", from);
}

bool PGQueryBuilder::keySeqtype(const string& key, const string& value, const string& from)
{
    return checkSeqtype(value) && keySingleValue(key, value.c_str(), "%public.seqtype", from);
}

bool PGQueryBuilder::keyInouttype(const string& key, const string& value, const string& from)
{
    return checkInouttype(value) && keySingleValue(key, value.c_str(), "%public.inouttype", from);
}

bool PGQueryBuilder::keyPStatus(const string& key, ProcessState::STATUS_T value, const string& from)
{
    return (value != ProcessState::STATUS_NONE) && keySingleValue(key, ProcessState::toStatusString(value).c_str(), "%public.pstatus", from);
}

bool PGQueryBuilder::keyTimestamp(const string& key, const time_t& value, const string& from)
{
    PGtimestamp ts = UnixTimeToTimestamp(value);
    return keySingleValue(key, &ts, "%timestamp", from);
}

#if HAVE_OPENCV
bool PGQueryBuilder::keyCvMat(const string& key, const cv::Mat& value, const string& from)
{
    bool ret = true;
    PGparam *cvmat = NULL;
    PGarray mat_dims = { 0 };

    do {
        // create dimensions array
        mat_dims.param = pqt.PQparamCreate((PGconn *) connection);
        if (!mat_dims.param) { ret = false; break; }
        
        for (int i = 0; i < value.dims; i++) {
            ret &= (0 != pqt.PQputf(mat_dims.param, "%int4", value.size[i]));
        }
        if (!ret) break;

        // matrix data
        PGbytea mat_data = { (int) (value.dataend - value.datastart), (char*) value.data };
        
        // create cvmat composite
        cvmat = pqt.PQparamCreate((PGconn *) connection);
        if (!cvmat) { ret = false; break; }
        
        ret = (0 != pqt.PQputf(cvmat, "%int4 %int4[] %bytea*",
            (PGint4) value.type(), &mat_dims, &mat_data));
        if (!ret) break;

        ret = keySingleValue(key, cvmat, "%public.cvmat", from);
    } while(0);

    if (cvmat) pqt.PQparamClear(cvmat);
    if (mat_dims.param) pqt.PQparamClear(mat_dims.param);

    return ret;
}
#endif

bool PGQueryBuilder::keyIntervalEvent(const string& key, const IntervalEvent& value, const string& from)
{
    bool ret = true;
    PGparam * event = NULL;
    
    do {
        // user data
        PGbytea data = { (int)value.user_data_size, (char*) value.user_data };

        // create interval event composite
        event = pqt.PQparamCreate((PGconn *) connection);
        if (!event) { ret = false; break; }

        ret = (0 != pqt.PQputf(event, "%int4 %int4 %bool %box %float8 %bytea*",
            (PGint4) value.group_id, (PGint4) value.class_id, (PGbool) value.is_root,
            (PGbox *) & value.region, (PGfloat8) value.score, &data));
        if (!ret) break;
        
        ret = keySingleValue(key, event, "%public.vtevent", from);
    } while(0);

    if (event) pqt.PQparamClear(event);

    return ret;
}



template<typename T>
bool PGQueryBuilder::whereSingleValue(const string& key, T value, const char *type, const string& oper, const string& from)
{
    uint idParam = 0;

    do {
        if (key.empty()) break;

        idParam = addToParam(type, value);
        if (!idParam) break;

        m_listWhere.push_back(WHERE_ITEM(key, from, oper, idParam));
    } while (0);

    return (idParam > 0);
}

bool PGQueryBuilder::whereString(const string& key, const string& value, const string& oper, const string& from)
{
    return whereSingleValue(key, value.c_str(), "%varchar", oper, from);
}

bool PGQueryBuilder::whereInt(const string& key, const int value, const string& oper, const string& from)
{
    return whereSingleValue(key, value, "%int4", oper, from);
}

bool PGQueryBuilder::whereFloat(const string& key, const float value, const string& oper, const string& from)
{
    return whereSingleValue(key, value, "%float4", oper, from);
}

bool PGQueryBuilder::whereSeqtype(const string& key, const string& value, const string& oper, const string& from)
{
    return checkSeqtype(value) && whereSingleValue(key, value.c_str(), "%public.seqtype", oper, from);
}

bool PGQueryBuilder::whereInouttype(const string& key, const string& value, const string& oper, const string& from)
{
    return checkInouttype(value) && whereSingleValue(key, value.c_str(), "%public.inouttype", oper, from);
}

bool PGQueryBuilder::wherePStatus(const string& key, ProcessState::STATUS_T value, const string& oper, const string& from)
{
    return value != ProcessState::STATUS_NONE && whereSingleValue(key, ProcessState::toStatusString(value).c_str(), "%public.pstatus", oper, from);
}

bool PGQueryBuilder::whereTimestamp(const string& key, const time_t& value, const string& oper, const string& from)
{
    PGtimestamp ts = UnixTimeToTimestamp(value);
    return whereSingleValue(key, &ts, "%timestamp", oper, from);
}

bool PGQueryBuilder::whereTimeRange(const string& key_start, const string& key_length, const time_t& value_start, const uint value_length, const string& oper, const string& from)
{
    bool bRet = true;

    do {
        if (key_start.empty() || key_length.empty()) { bRet = false; break; }

        string exp = 
            "'[" + UnixTimeToTimestampString(value_start) + ',' +
            UnixTimeToTimestampString(value_start + value_length) + "]'";
        
        string value =
            "public.tsrange(" + constructColumn(key_start, from) + ',' +
            constructColumn(key_length, from) + ')';
        
        m_listWhere.push_back(WHERE_ITEM(exp, oper, value));
    } while (0);

    return bRet;
}

bool PGQueryBuilder::whereRegion(const string& key, const IntervalEvent::box& value, const string& oper, const string& from)
{
    return whereSingleValue(key, &value, "%box", oper, from);
}

bool PGQueryBuilder::whereExpression(const string& expression, const std::string& value, const std::string& oper)
{
    if (!expression.empty() && !value.empty()) {
        m_listWhere.push_back(WHERE_ITEM(expression, oper, value));
        return true;
    }
    else {
        return false;
    }
}

template<typename T>
unsigned int PGQueryBuilder::addToParam(const char* type, T value)
{
    uint ret = 0;
    
    do {
        if (!queryParam && !(queryParam = createQueryParam())) break;

        if (pqt.PQputf((PGparam *)queryParam, type, value) == 0) {
            logger->warning(658, "failed to add value to query: " + toString(value), thisClass + "::addToParam()");
            break;
        }

        ret = ++m_cntParam;
    } while(0);
    
    return ret;
}

string PGQueryBuilder::constructTable(const string& table, const string& schema)
{
    const string& tab = (!table.empty() ? table : defaultTable);
    
    // no table specified
    if (tab.empty()) {
        return DEF_NO_TABLE;
    }
    // no DB schema in table name, use function arg or default schema
    else if (tab.find('.') == string::npos) {
        if (!schema.empty()) {
            return schema + '.' + tab;
        }
        else if (!defaultSchema.empty()) {
            return defaultSchema + '.' + tab;
        }
        else {
            return DEF_NO_SCHEMA + '.' + tab;
        }
    }
    // DB schema in table name
    else {
        return tab;
    }
}

string PGQueryBuilder::constructColumn(const string& column, const string& table)
{
    if (column.empty()) {
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
                if (col.empty()) col = DEF_NO_COLUMN;

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
            if (col.empty()) col = DEF_NO_COLUMN;

            // composite members are accessed through comma, special escape
            if (column[charPos] == ',') {
                string rest = column.substr(charPos+1, string::npos);
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

string PGQueryBuilder::constructColumnNoTable(const string& column)
{
    size_t charPos = column.find_first_of(",.");
    if (charPos > 0 && charPos < column.length() - 1) {
        return escapeIdent(column.substr(0,charPos)) + '.' + escapeIdent(column.substr(charPos+1, string::npos));
    }
    else {
        return escapeIdent(column);
    }
}


string PGQueryBuilder::constructAlias(const string& column)
{
    return column.substr(0, column.find_first_of(":[(,."));
}

string PGQueryBuilder::escapeIdent(const string& ident)
{
    char *escaped = pg.PQescapeIdentifier((PGconn *)connection, ident.c_str(), ident.length());
    string ret = escaped;
    pg.PQfreemem(escaped);
    return ret;
}

string PGQueryBuilder::escapeLiteral(const string& literal)
{
    char *escaped = pg.PQescapeLiteral((PGconn *)connection, literal.c_str(), literal.length());
    string ret = escaped;
    pg.PQfreemem(escaped);
    return ret;
}

PGtimestamp PGQueryBuilder::UnixTimeToTimestamp(const time_t& utime)
{
    PGtimestamp ret = {0};
    struct tm* ts = gmtime(&utime);

    ret.date.year = ts->tm_year + 1900;
    ret.date.mon  = ts->tm_mon;
    ret.date.mday = ts->tm_mday;
    ret.time.hour = ts->tm_hour;
    ret.time.min  = ts->tm_min;
    ret.time.sec  = ts->tm_sec;

    return ret;
}

string PGQueryBuilder::UnixTimeToTimestampString(const time_t& utime)
{
    struct tm* ts = gmtime(&utime);
    char buffer[64];
    
    sprintf(buffer, "%d-%02d-%02d %02d:%02d:%02d",
        ts->tm_year + 1900, ts->tm_mon + 1, ts->tm_mday,
        ts->tm_hour, ts->tm_min, ts->tm_sec);
    
    return buffer;
}
#endif

}
