/* 
 * File:   vtapi_querybuilder.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:40 PM
 */

#ifndef VTAPI_QUERYBUILDER_H
#define	VTAPI_QUERYBUILDER_H

#include "vtapi_backendlibs.h"
#include "../common/vtapi_logger.h"
#include "../common/vtapi_tkeyvalue.h"

namespace vtapi {

class Connection;
class TypeManager;

/**
 * @brief Class provides functionality to build various SQL queries
 *
 * Query parameters must first be inserted through keyX and whereX methods.
 * KeyX methods should be used to add key(column)/value pairs (or just keys) for
 * main section of SELECT, UPDATE and INSERT queries. WhereX methods construct
 * WHERE clause for SELECT and UPDATE queries.
 *
 * It's possible to set default dataset and table which will be used if those
 * are not specified in keyX or whereX call.
 *
 * Query string can be obtained via getSelectQuery, getUpdateQuery or
 * getInsertQuery methodhs.
 */
class QueryBuilder {
protected:

    fmap_t              *fmap;          /**< library functions address book */
    Connection          *connection;    /**< connection object */
    TypeManager         *typeManager;   /**< type manager object */
    Logger              *logger;        /**< logger object for output messaging */
    std::string         thisClass;      /**< class name */

    std::vector<std::string> opers;     /**< operators */
    void                *param;         /**< object for parametrized queries */

    std::string         initString;     /**< init query string (whole query or table) */
    std::string         table;          /**< default table for queries */
    std::string         dataset;        /**< active dataset */

public:
    /**
     * Constructor
     * @param fmap library functions address book
     * @param connection connection object
     * @param typeManager typeManager object
     * @param logger logger object for output messaging
     * @param initString initialization string (query/table or empty)
     */
    QueryBuilder(fmap_t *fmap, Connection *connection, TypeManager *typeManager, Logger *logger, const std::string& initString = "") {
        this->initString    = initString;
        this->typeManager   = typeManager;
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
     */
    void setDataset(const std::string& dataset) { this->dataset = dataset; };
   /**
     * This is to specify the (single) table to be inserted in
     * @param table table into which new data will be inserted
     */
    void setTable(const std::string& table) { this->table = table; };
    /**
     * Gets query from initialization string
     * @return query string
     */
    virtual std::string getGenericQuery() = 0;
    /**
     * Builds SELECT query
     * @param groupby
     * @param orderby
     * @param limit
     * @param offset
     * @return SELECT query string
     */
    virtual std::string getSelectQuery(const std::string& groupby, const std::string& orderby, const int limit, const int offset) = 0;
    /**
     * Builds INSERT query
     * @return INSERT query string
     */
    virtual std::string getInsertQuery() = 0;
    /**
     * Builds UPDATE query
     * @return UPDATE query string
     */
    virtual std::string getUpdateQuery() = 0;
    
    /**
     * Resets query builder to initial state
     */
    virtual void reset() = 0;
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
    virtual bool keyFrom(const std::string& table, const std::string& column) = 0;

    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyString(const std::string& key, const std::string& value, const std::string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyStringA(const std::string& key, std::string* values, const int size, const std::string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyInt(const std::string& key, int value, const std::string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyIntA(const std::string& key, int* values, const int size, const std::string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyFloat(const std::string& key, float value, const std::string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyFloatA(const std::string& key, float* values, const int size, const std::string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keySeqtype(const std::string& key, const std::string& value, const std::string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyInouttype(const std::string& key, const std::string& value, const std::string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
//    virtual bool keyPermissions(const std::string& key, const std::string& value, const std::string& from = "") = 0;
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyTimestamp(const std::string& key, const time_t& value, const std::string& from = "") = 0;

#ifdef HAVE_OPENCV
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    virtual bool keyCvMat(const std::string& key, const cv::Mat& value, const std::string& from = "") = 0;
#endif
    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * This is a WHERE statement construction class
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereString(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "") = 0;

    /**
     * This is a WHERE statement construction class for integers
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereInt(const std::string& key, const int value, const std::string& oper = "=", const std::string& from = "") = 0;

    /**
     * This is a WHERE statement construction class for floats
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereFloat(const std::string& key, const float value, const std::string& oper = "=", const std::string& from = "") = 0;
    /**
     * This is a WHERE statement construction class for seqtype
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereSeqtype(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "") = 0;
    /**
     * This is a WHERE statement construction class for inouttype
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereInouttype(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "") = 0;
    /**
     * This is a WHERE statement construction class for permissions
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
//    virtual bool wherePermissions(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "") = 0;
    /**
     * This is a WHERE statement construction class for timestamp
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    virtual bool whereTimestamp(const std::string& key, const time_t& value, const std::string& oper = "=", const std::string& from = "") = 0;

protected:

    /**
     * Escape key/table pair
     * @param key key
     * @param table table
     * @return escaped full key name
     */
    virtual std::string escapeColumn(const std::string& key, const std::string& table) = 0;
    /**
     * Escape identifier
     * @param ident identifier
     * @return escaped identifier
     */
    virtual std::string escapeIdent(const std::string& ident) = 0;
    /**
     * Escape literal
     * @param literal literal
     * @return escaped literal
     */
    virtual std::string escapeLiteral(const std::string& literal) = 0;
    /**
     * Checks validity of seqtype value
     * @param value seqtype value
     * @return success
     */
    bool checkSeqtype(const std::string& value) {
        return (value.compare("images") == 0) || (value.compare("video") == 0) || (value.compare("data") == 0);
    }
    /**
     * Checks validity of inouttype value
     * @param value inouttype value
     * @return success
     */
    bool checkInouttype(const std::string& value) {
        return (value.compare("in") == 0) || (value.compare("inout") == 0) || (value.compare("out") == 0);
    }
};

#if HAVE_POSTGRESQL
class PGQueryBuilder : public QueryBuilder {
private:

    TKeys                   keys_main;          /**< table keys storage for SELECT FROM, INSERT INTO and UPDATE SET clauses */
    TKeys                   keys_where;         /**< table keys storage for WHERE clause */

    int                     keysCnt;            /**< keys counter */
    std::vector<int>             keys_main_order;    /**< Indexes of keys_main */
    std::vector<int>             keys_where_order;   /**< Indexes of keys_where */

public:

    PGQueryBuilder(fmap_t *fmap, Connection *connection, TypeManager *typeManager, Logger *logger = NULL, const std::string& initString = "");
    ~PGQueryBuilder();

    std::string getGenericQuery();
    std::string getSelectQuery(const std::string& groupby, const std::string& orderby, const int limit, const int offset);
    std::string getInsertQuery();
    std::string getUpdateQuery();

    bool keyFrom(const std::string& table, const std::string& column);
    bool keyString(const std::string& key, const std::string& value, const std::string& from = "");
    bool keyStringA(const std::string& key, std::string* values, const int size, const std::string& from = "");
    bool keyInt(const std::string& key, int value, const std::string& from = "");
    bool keyIntA(const std::string& key, int* values, const int size, const std::string& from = "");
    bool keyFloat(const std::string& key, float value, const std::string& from = "");
    bool keyFloatA(const std::string& key, float* values, const int size, const std::string& from = "");
    bool keySeqtype(const std::string& key, const std::string& value, const std::string& from = "");
    bool keyInouttype(const std::string& key, const std::string& value, const std::string& from = "");
//    bool keyPermissions(const std::string& key, const std::string& value, const std::string& from = "");
    bool keyTimestamp(const std::string& key, const time_t& value, const std::string& from = "");
#if HAVE_OPENCV
    bool keyCvMat(const std::string& key, const cv::Mat& value, const std::string& from = "");
#endif
    
    bool whereString(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "");
    bool whereInt(const std::string& key, const int value, const std::string& oper = "=", const std::string& from = "");
    bool whereFloat(const std::string& key, const float value, const std::string& oper = "=", const std::string& from = "");
    bool whereSeqtype(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "");
    bool whereInouttype(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "");
//    bool wherePermissions(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "");
    bool whereTimestamp(const std::string& key, const time_t& value, const std::string& oper = "=", const std::string& from = "");

    void reset();
    void createParam();
    void destroyParam();

protected:

    std::string escapeColumn(const std::string& key, const std::string& table);
    std::string escapeIdent(const std::string& ident);
    std::string escapeAlias(const std::string& key);
    std::string escapeLiteral(const std::string& literal);
};
#endif

#if HAVE_SQLITE
class SLQueryBuilder : public QueryBuilder {
private:

    TKeyValues      key_values_main;    /**< table key/values storage for SELECT FROM, INSERT INTO and UPDATE SET clauses */
    TKeyValues      key_values_where;   /**< table key/values storage for SELECT FROM, INSERT INTO and UPDATE SET clauses */

public:

    SLQueryBuilder(fmap_t *fmap, Connection *connection, TypeManager *typeManager, Logger *logger = NULL, const std::string& initString = "");
    ~SLQueryBuilder();

    std::string getGenericQuery();
    std::string getSelectQuery(const std::string& groupby, const std::string& orderby, const int limit, const int offset);
    std::string getInsertQuery();
    std::string getUpdateQuery();    

    bool keyFrom(const std::string& table, const std::string& column);
    bool keyString(const std::string& key, const std::string& value, const std::string& from = "");
    bool keyStringA(const std::string& key, std::string* values, const int size, const std::string& from = "");
    bool keyInt(const std::string& key, int value, const std::string& from = "");
    bool keyIntA(const std::string& key, int* values, const int size, const std::string& from = "");
    bool keyFloat(const std::string& key, float value, const std::string& from = "");
    bool keyFloatA(const std::string& key, float* values, const int size, const std::string& from = "");
    bool keySeqtype(const std::string& key, const std::string& value, const std::string& from = "");
    bool keyInouttype(const std::string& key, const std::string& value, const std::string& from = "");
//    bool keyPermissions(const std::string& key, const std::string& value, const std::string& from = "");
    bool keyTimestamp(const std::string& key, const time_t& value, const std::string& from = "");
#if HAVE_OPENCV
    bool keyCvMat(const std::string& key, const cv::Mat& value, const std::string& from = "");
#endif
    
    bool whereString(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "");
    bool whereInt(const std::string& key, const int value, const std::string& oper = "=", const std::string& from = "");
    bool whereFloat(const std::string& key, const float value, const std::string& oper = "=", const std::string& from = "");
    bool whereSeqtype(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "");
    bool whereInouttype(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "");
//    bool wherePermissions(const std::string& key, const std::string& value, const std::string& oper = "=", const std::string& from = "");
    bool whereTimestamp(const std::string& key, const time_t& value, const std::string& oper = "=", const std::string& from = "");
    
    void reset();
    void createParam();
    void destroyParam();

protected:

    std::string escapeColumn(const std::string& key, const std::string& table);
    std::string escapeIdent(const std::string& ident);
    std::string escapeLiteral(const std::string& literal);

private:

    /**
     * Frees all key s vectors
     */
    void destroyKeys();

};
#endif

} // namespace vtapi

#endif	/* VTAPI_QUERYBUILDER_H */

