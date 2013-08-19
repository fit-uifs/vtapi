/* 
 * File:   vtapi_querybuilder.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:40 PM
 */

#ifndef VTAPI_QUERYBUILDER_H
#define	VTAPI_QUERYBUILDER_H

namespace vtapi {
    class QueryBuilder;
    class PGQueryBuilder;
    class SLQueryBuilder;
}

#include "vtapi_libloader.h"
#include "vtapi_connection.h"
#include "../common/vtapi_tkeyvalue.h"
#include "../common/vtapi_logger.h"
#include "../common/vtapi_global.h"

namespace vtapi {


//TODO: comment
class QueryBuilder {
protected:

    fmap_t              *fmap;      /**< library functions address book */
    Connection          *connection;    /**< connection object */
    Logger              *logger;        /**< logger object for output messaging */
    string              thisClass;      /**< class name */

    vector<string>      opers;          /**< operators */
    void                *param;         /**< object for parametrized queries */

    string              initString;     /**< init query string (whole query or table) */
    string              dataset;        /**< active dataset */
    string              table;          /**< default table for queries */

public:
    /**
     * Constructor
     * @param fmap library functions address book
     * @param connection connection object
     * @param logger logger object for output messaging
     * @param initString initialization string (query/table or empty)
     */
    QueryBuilder(fmap_t *fmap, Connection *connection, Logger *logger, const string& initString = "") {
        this->initString    = initString;
        this->logger        = logger;
        this->connection    = connection;
        this->fmap          = fmap;
        this->param         = NULL;
    };
    /**
     * Virtual destructor - destroy implementation first
     */
    virtual ~QueryBuilder() { };

    /**
     * Gets object for parametrized queries
     * @return param object for parametrized queries
     */
    void *getParam() { return this->param; }
    /**
     * This is to specify dataset to be inserted in
     * @param dataset dataset into which new data will be inserted
     * @return success
     */
    bool setDataset(const string& dataset) { this->dataset = dataset; };
    /**
     * Gets query from initialization string
     * @return query string
     */
    virtual string getGenericQuery() = 0;
    /**
     * Builds SELECT query
     * @param groupby
     * @param orderby
     * @param limit
     * @param offset
     * @return SELECT query string
     */
    virtual string getSelectQuery(const string& groupby, const string& orderby, const int limit, const int offset) = 0;
    /**
     * Builds INSERT query
     * @return INSERT query string
     */
    virtual string getInsertQuery() = 0;
    /**
     * Builds UPDATE query
     * @return UPDATE query string
     */
    virtual string getUpdateQuery() = 0;
    
    /**
     * Resets query builder to initial state
     * @return successs
     */
    virtual bool reset() = 0;
    /**
     * Allocates new query param structure, destroys the old one
     */
    virtual void createParam() = 0;
    /**
     * Destroys query param structure
     */
    virtual void destroyParam() = 0;

    /**
     * This is used to specify the table for FROM statement and the column list for SELECT statement
     * It may be called more times.
     * @param table table to select
     * @param column column for select
     * @return success
     */
    virtual bool keyFrom(const string& table, const string& column) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyString(const string& key, const string& value, const string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     * @unimplemented neimplementováno (pak zkontrolovat doc)
     */
    virtual bool keyStringA(const string& key, string* values, const int size, const string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyInt(const string& key, int value, const string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyIntA(const string& key, int* values, const int size, const string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyFloat(const string& key, float value, const string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyFloatA(const string& key, float* values, const int size, const string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keySeqtype(const string& key, const string& value, const string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyInouttype(const string& key, const string& value, const string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
//    virtual bool keyPermissions(const string& key, const string& value, const string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyTimestamp(const string& key, const time_t& value, const string& from = "") = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * This is to specify the (single) table to be inserted in
     * @param table table into which new data will be inserted
     * @return success
     * @todo @b doc: Petrovy závorky
     */
    bool setTable(const string& table) { this->table = table; };

    /**
     * This is a WHERE statement construction class
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return
     */
    virtual bool whereString(const string& key, const string& value, const string& oper = "=", const string& from = "") = 0;

    /**
     * This is a WHERE statement construction class for integers
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return
     */
    virtual bool whereInt(const string& key, const int value, const string& oper = "=", const string& from = "") = 0;

    /**
     * This is a WHERE statement construction class for floats
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return
     */
    virtual bool whereFloat(const string& key, const float value, const string& oper = "=", const string& from = "") = 0;
    /**
     * This is a WHERE statement construction class for seqtype
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return
     */
    virtual bool whereSeqtype(const string& key, const string& value, const string& oper = "=", const string& from = "") = 0;
    /**
     * This is a WHERE statement construction class for inouttype
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return
     */
    virtual bool whereInouttype(const string& key, const string& value, const string& oper = "=", const string& from = "") = 0;
    /**
     * This is a WHERE statement construction class for permissions
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return
     */
//    virtual bool wherePermissions(const string& key, const string& value, const string& oper = "=", const string& from = "") = 0;
    /**
     * This is a WHERE statement construction class for timestamp
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return
     */
    virtual bool whereTimestamp(const string& key, const time_t& value, const string& oper = "=", const string& from = "") = 0;

protected:

    /**
     * Escape key/table pair
     * @param key key
     * @param table table
     * @return escaped full key name
     */
    virtual string escapeColumn(const string& key, const string& table) = 0;
    /**
     * Escape identifier
     * @param ident identifier
     * @return escaped identifier
     */
    virtual string escapeIdent(const string& ident) = 0;
    /**
     * Escape literal
     * @param literal literal
     * @return escaped literal
     */
    virtual string escapeLiteral(const string& literal) = 0;
};


class PGQueryBuilder : public QueryBuilder {
private:

    TKeys                   keys_main;          /**< table keys storage for SELECT FROM, INSERT INTO and UPDATE SET clauses */
    TKeys                   keys_where;         /**< table keys storage for WHERE clause */

    int                     keysCnt;            /**< keys counter */
    vector<int>             keys_main_order;    /**< Indexes of keys_main */
    vector<int>             keys_where_order;   /**< Indexes of keys_where */

public:

    PGQueryBuilder(fmap_t *fmap, Connection *connection, Logger *logger = NULL, const string& initString = "");
    ~PGQueryBuilder();

    string getGenericQuery();
    string getSelectQuery(const string& groupby, const string& orderby, const int limit, const int offset);
    string getInsertQuery();
    string getUpdateQuery();

    bool keyFrom(const string& table, const string& column);
    bool keyString(const string& key, const string& value, const string& from = "");
    bool keyStringA(const string& key, string* values, const int size, const string& from = "");
    bool keyInt(const string& key, int value, const string& from = "");
    bool keyIntA(const string& key, int* values, const int size, const string& from = "");
    bool keyFloat(const string& key, float value, const string& from = "");
    bool keyFloatA(const string& key, float* values, const int size, const string& from = "");
    bool keySeqtype(const string& key, const string& value, const string& from = "");
    bool keyInouttype(const string& key, const string& value, const string& from = "");
//    bool keyPermissions(const string& key, const string& value, const string& from = "");
    bool keyTimestamp(const string& key, const time_t& value, const string& from = "");

    bool whereString(const string& key, const string& value, const string& oper = "=", const string& from = "");
    bool whereInt(const string& key, const int value, const string& oper = "=", const string& from = "");
    bool whereFloat(const string& key, const float value, const string& oper = "=", const string& from = "");
    bool whereSeqtype(const string& key, const string& value, const string& oper = "=", const string& from = "");
    bool whereInouttype(const string& key, const string& value, const string& oper = "=", const string& from = "");
//    bool wherePermissions(const string& key, const string& value, const string& oper = "=", const string& from = "");
    bool whereTimestamp(const string& key, const time_t& value, const string& oper = "=", const string& from = "");

    bool reset();
    void createParam();
    void destroyParam();

protected:

    string escapeColumn(const string& key, const string& table);
    string escapeIdent(const string& ident);
    string escapeLiteral(const string& literal);
};


class SLQueryBuilder : public QueryBuilder {
private:

    TKeyValues      key_values_main;    /**< table key/values storage for SELECT FROM, INSERT INTO and UPDATE SET clauses */
    TKeyValues      key_values_where;   /**< table key/values storage for SELECT FROM, INSERT INTO and UPDATE SET clauses */

public:

    SLQueryBuilder(fmap_t *fmap, Connection *connection, Logger *logger = NULL, const string& initString = "");
    ~SLQueryBuilder();

    string getGenericQuery();
    string getSelectQuery(const string& groupby, const string& orderby, const int limit, const int offset);
    string getInsertQuery();
    string getUpdateQuery();    

    bool keyFrom(const string& table, const string& column);
    bool keyString(const string& key, const string& value, const string& from = "");
    bool keyStringA(const string& key, string* values, const int size, const string& from = "");
    bool keyInt(const string& key, int value, const string& from = "");
    bool keyIntA(const string& key, int* values, const int size, const string& from = "");
    bool keyFloat(const string& key, float value, const string& from = "");
    bool keyFloatA(const string& key, float* values, const int size, const string& from = "");
    bool keySeqtype(const string& key, const string& value, const string& from = "");
    bool keyInouttype(const string& key, const string& value, const string& from = "");
//    bool keyPermissions(const string& key, const string& value, const string& from = "");
    bool keyTimestamp(const string& key, const time_t& value, const string& from = "");

    bool whereString(const string& key, const string& value, const string& oper = "=", const string& from = "");
    bool whereInt(const string& key, const int value, const string& oper = "=", const string& from = "");
    bool whereFloat(const string& key, const float value, const string& oper = "=", const string& from = "");
    bool whereSeqtype(const string& key, const string& value, const string& oper = "=", const string& from = "");
    bool whereInouttype(const string& key, const string& value, const string& oper = "=", const string& from = "");
//    bool wherePermissions(const string& key, const string& value, const string& oper = "=", const string& from = "");
    bool whereTimestamp(const string& key, const time_t& value, const string& oper = "=", const string& from = "");

    bool reset();
    void createParam();
    void destroyParam();

protected:

    string escapeColumn(const string& key, const string& table);
    string escapeIdent(const string& ident);
    string escapeLiteral(const string& literal);

private:

    bool checkSeqtype(const string& value);
    bool checkInouttype(const string& value);
    void destroyKeys();

};

} // namespace vtapi

#endif	/* VTAPI_QUERYBUILDER_H */

