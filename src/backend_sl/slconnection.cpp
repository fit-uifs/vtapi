
#include <common/vtapi_global.h>
#include <backends/vtapi_connection.h>

#if HAVE_SQLITE

using std::string;

using namespace vtapi;

SLConnection::SLConnection(const SLBackendBase &base, const std::string& connectionInfo) :
    Connection(connectionInfo),
    SLBackendBase(base)
{
    thisClass   = "SLConnection";
    conn        = NULL;
}

SLConnection::~SLConnection() {
    disconnect();
}

bool SLConnection::connect (const string& connectionInfo) {
    bool retval     = true;

    connInfo    = connectionInfo;
    fixSlashes(connInfo);
    string dbname = connInfo + "/" + SL_DB_PREFIX + SL_DB_PUBLIC + SL_DB_SUFFIX;
    sl.sqlite3_open_v2(dbname.c_str(), &conn, SQLITE_OPEN_READWRITE, NULL);

    retval = isConnected();
    if (!retval) {
        string errmsg = conn ? string(sl.sqlite3_errmsg(conn)) : "The connection couldn't have been established.";
        logger->error(122, errmsg, thisClass+"::connect(");
    }

    return retval;
}

bool SLConnection::reconnect (const string& connectionInfo) {
    if (!connectionInfo.empty()) {
        connInfo = connectionInfo;
        fixSlashes(connInfo);
    }
    disconnect();
    return connect(connInfo);
}

void SLConnection::disconnect () {
    if (conn) {
        sl.sqlite3_close(conn);
    }
}

bool SLConnection::isConnected () {
    int cur, high;
    if (conn) {
        return (sl.sqlite3_db_status(conn, SQLITE_DBSTATUS_SCHEMA_USED, &cur, &high, false) == SQLITE_OK);
    }
    else {
        return false;
    }
}

bool SLConnection::execute(const string& query, void *param) {
    SLparam     *sl_param   = (SLparam *) param;
    char        *errmsg     = NULL;
    bool        retval      = true;

    errorMessage.clear();

    retval = attachDatabase (sl_param->database);
    if (!retval) {
        errorMessage = "Database " + sl_param->database + " couldn't have been attached.";
    }
    else {
        retval = sl.sqlite3_exec(conn, query.c_str(), NULL, NULL, &errmsg) == SQLITE_OK;
        if (!retval && errmsg) {
            errorMessage = string(errmsg);
            sl.sqlite3_free(errmsg);
        }
    }

    return retval;
}

int SLConnection::fetch(const string& query, void *param, ResultSet *resultSet) {
    SLparam     *sl_param   = (SLparam *) param;
    SLres       *sl_res     = new SLres();
    char        *errmsg     = NULL;
    int         retval      = ER_FAIL;
    int         retquery    = SQLITE_ERROR;

    errorMessage.clear();

    if (!attachDatabase (sl_param->database)) {
        errorMessage = "Database " + sl_param->database + " couldn't have been attached.";
    }
    else {
        retquery = sl.sqlite3_get_table(conn, query.c_str(), &(sl_res->res), &(sl_res->rows), &(sl_res->cols), &errmsg);
        resultSet->newResult((void *) sl_res);
        if (retquery == SQLITE_OK) {
            retval = sl_res->rows;
        }
        else if (errmsg) {
            errorMessage = string(errmsg);
            sl.sqlite3_free(errmsg);
        }
    }

    return retval;
}

void* SLConnection::getConnectionObject() {
    return (void *) this->conn;
}

bool SLConnection::fixSlashes(string& path) {
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

bool SLConnection::attachDatabase(string& dbfile) {
    if (sl.sqlite3_db_filename(conn, dbfile.c_str()) == NULL) {
        string query = "ATTACH DATABASE \'" + connInfo + "/" + SL_DB_PREFIX + dbfile + SL_DB_SUFFIX + "\' AS \'" + dbfile + "\';";
        return sl.sqlite3_exec(conn, query.c_str(), NULL, NULL, NULL) == SQLITE_OK;
    }
    else {
        return true;
    }
}



#endif
