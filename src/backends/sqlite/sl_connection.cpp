
#include <vtapi/common/global.h>
#include "sl_connection.h"

// sqlite database files
#define SL_DB_PREFIX "vtapi_"
#define SL_DB_SUFFIX ".db"
#define SL_DB_PUBLIC "public"

#define SLCONN ((sqlite3*)_conn)

using namespace std;

namespace vtapi {


bool SLConnection::connect ()
{
    bool retval = true;

    fixSlashes(_connection_info);
    string dbname = _connection_info + "/" + SL_DB_PREFIX + SL_DB_PUBLIC + SL_DB_SUFFIX;

    VTLOG_DEBUG("Connecting to DB... " + dbname);

    sqlite3_open_v2(dbname.c_str(), (sqlite3**)&_conn, SQLITE_OPEN_READWRITE, NULL);
    if (!(retval = isConnected())) {
        VTLOG_ERROR(sqlite3_errmsg(SLCONN));
        disconnect();
    }

    return retval;
}

void SLConnection::disconnect ()
{
    if (SLCONN) {
        VTLOG_DEBUG("Disconnecting DB...");

        sqlite3_close(SLCONN);
    }
}

bool SLConnection::isConnected () const
{
    if (SLCONN) {
        int cur, high;
        int ret = sqlite3_db_status(SLCONN, SQLITE_DBSTATUS_SCHEMA_USED,
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

    _error_message.clear();

    retval = attachDatabase (sl_param->database);
    if (!retval) {
        _error_message = "Database " + sl_param->database + " couldn't have been attached.";
        VTLOG_ERROR(_error_message);
    }
    else {
        retval = sqlite3_exec(SLCONN, query.c_str(), NULL, NULL, &errmsg) == SQLITE_OK;
        if (!retval) {
            if (errmsg) {
                _error_message = string(errmsg);
                sqlite3_free(errmsg);
            }
            else {
                _error_message = "Query failed : " + query;
            }
            VTLOG_ERROR(_error_message);
        }
    }

    return retval;
}

int SLConnection::fetch(const string& query, void *param, ResultSet &resultSet)
{
    SLparam     *sl_param   = (SLparam *) param;
    SLres       *sl_res     = new SLres();
    char        *errmsg     = NULL;
    int         retval      = -1;
    int         retquery    = SQLITE_ERROR;

    VTLOG_DEBUG(query);

    _error_message.clear();

    if (!attachDatabase (sl_param->database)) {
        _error_message = "Database " + sl_param->database + " couldn't have been attached.";
        VTLOG_ERROR(_error_message);
    }
    else {
        retquery = sqlite3_get_table(SLCONN, query.c_str(), &(sl_res->res),
                                         &(sl_res->rows), &(sl_res->cols), &errmsg);
        resultSet.newResult((void *) sl_res);
        if (retquery == SQLITE_OK) {
            retval = sl_res->rows;
        }
        else {
            if (errmsg) {
                _error_message = string(errmsg);
                sqlite3_free(errmsg);
            }
            else {
                _error_message = "Query failed : " + query;
            }
            VTLOG_ERROR(_error_message);
        }
    }

    return retval;
}

bool SLConnection::fixSlashes(string& path) const
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
    if (sqlite3_db_filename(SLCONN, dbfile.c_str()) == NULL) {
        string path = _connection_info + "/" + SL_DB_PREFIX + dbfile + SL_DB_SUFFIX;
        string query = "ATTACH DATABASE \'" + path + "\' AS \'" + dbfile + "\';";
        return sqlite3_exec(SLCONN, query.c_str(), NULL, NULL, NULL) == SQLITE_OK;
    }
    else {
        return true;
    }
}

}
