
#include <common/vtapi_global.h>
#include <backends/vtapi_connection.h>

#if VTAPI_HAVE_SQLITE


using namespace std;

namespace vtapi {


SLConnection::SLConnection(const SLBackendBase &base, const std::string& connectionInfo)
    : Connection(connectionInfo), SLBackendBase(base)
{
    _conn = NULL;
}

SLConnection::~SLConnection()
{
    disconnect();
}

bool SLConnection::connect ()
{
    bool retval = true;

    fixSlashes(_connInfo);
    string dbname = _connInfo + "/" + SL_DB_PREFIX + SL_DB_PUBLIC + SL_DB_SUFFIX;

    VTLOG_DEBUG("Connecting to DB... " + dbname);

    _sl.sqlite3_open_v2(dbname.c_str(), &_conn, SQLITE_OPEN_READWRITE, NULL);
    if (!(retval = isConnected())) {
        VTLOG_ERROR(_sl.sqlite3_errmsg(_conn));
        disconnect();
    }

    return retval;
}

void SLConnection::disconnect ()
{
    if (_conn) {
        VTLOG_DEBUG("Disconnecting DB...");

        _sl.sqlite3_close(_conn);
    }
}

bool SLConnection::isConnected ()
{
    if (_conn) {
        int cur, high;
        int ret = _sl.sqlite3_db_status(_conn, SQLITE_DBSTATUS_SCHEMA_USED,
                                        &cur, &high, false);
        return ret == SQLITE_OK;
    }
    else {
        return false;
    }
}

bool SLConnection::execute(const string& query, void *param)
{
    SLparam     *sl_param   = (SLparam *) param;
    char        *errmsg     = NULL;
    bool        retval      = true;

    VTLOG_DEBUG(query);

    _errorMessage.clear();

    retval = attachDatabase (sl_param->database);
    if (!retval) {
        _errorMessage = "Database " + sl_param->database + " couldn't have been attached.";
        VTLOG_ERROR(_errorMessage);
    }
    else {
        retval = _sl.sqlite3_exec(_conn, query.c_str(), NULL, NULL, &errmsg) == SQLITE_OK;
        if (!retval) {
            if (errmsg) {
                _errorMessage = string(errmsg);
                _sl.sqlite3_free(errmsg);
            }
            else {
                _errorMessage = "Query failed : " + query;
            }
            VTLOG_ERROR(_errorMessage);
        }
    }

    return retval;
}

int SLConnection::fetch(const string& query, void *param, ResultSet *resultSet)
{
    SLparam     *sl_param   = (SLparam *) param;
    SLres       *sl_res     = new SLres();
    char        *errmsg     = NULL;
    int         retval      = -1;
    int         retquery    = SQLITE_ERROR;

    VTLOG_DEBUG(query);

    _errorMessage.clear();

    if (!attachDatabase (sl_param->database)) {
        _errorMessage = "Database " + sl_param->database + " couldn't have been attached.";
        VTLOG_ERROR(_errorMessage);
    }
    else {
        retquery = _sl.sqlite3_get_table(_conn, query.c_str(), &(sl_res->res),
                                         &(sl_res->rows), &(sl_res->cols), &errmsg);
        resultSet->newResult((void *) sl_res);
        if (retquery == SQLITE_OK) {
            retval = sl_res->rows;
        }
        else {
            if (errmsg) {
                _errorMessage = string(errmsg);
                _sl.sqlite3_free(errmsg);
            }
            else {
                _errorMessage = "Query failed : " + query;
            }
            VTLOG_ERROR(_errorMessage);
        }
    }

    return retval;
}

void* SLConnection::getConnectionObject()
{
    return (void *)_conn;
}

bool SLConnection::fixSlashes(string& path)
{
    size_t len = path.length();
    size_t slPos = 0;
    size_t nsPos = len;

    do {
        slPos = path.find('\\', slPos);
        if (slPos != string::npos) {
            path[slPos] = '/';
        }
        else {
            break;
        }
    } while (1);
    do {
        nsPos--;
        if (nsPos < 0) {
            path.clear();
            return false;
        }
    } while (path[nsPos] == '/');
    if (nsPos < len - 1) {
        path = path.substr(0, nsPos + 1);
    }

    return true;
}

bool SLConnection::attachDatabase(string& dbfile)
{
    if (_sl.sqlite3_db_filename(_conn, dbfile.c_str()) == NULL) {
        string path = _connInfo + "/" + SL_DB_PREFIX + dbfile + SL_DB_SUFFIX;
        string query = "ATTACH DATABASE \'" + path + "\' AS \'" + dbfile + "\';";
        return _sl.sqlite3_exec(_conn, query.c_str(), NULL, NULL, NULL) == SQLITE_OK;
    }
    else {
        return true;
    }
}

}

#endif

