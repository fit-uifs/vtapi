/* 
 * File:   vtapi_select.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:59 PM
 */

#ifndef VTAPI_SELECT_H
#define	VTAPI_SELECT_H

namespace vtapi {
    class Select;
}

#include "vtapi_query.h"

namespace vtapi {

    
/**
 * @brief Class for constructing and executing SELECT queries
 * @see Basic information on page @ref KEYVALUES
 * @note Error codes 21*
 */
class Select : public Query {
protected:

    string                          groupby;    /**< String used for the GROUP BY statement */
    string                          orderby;    /**< String used for the ORDER BY statement */

    int                             limit;      /**< Specify a size (a number of rows) of the resultset */
    int                             offset;     /**< Specify an index of row, where the resultset starts */

public:

    /**
     * Constructor of a SELECT query object
     * @param commons configuration object of Commons class
     * @param initString initialization string (query)
     */
    Select(const Commons& commons, const string& initString = "");
    /**
     * Get SELECT query string
     * @return query string
     */
    string getQuery();
    /**
     * Execute SELECT query and fetch result
     * @return success
     */
    bool execute();
    /**
     * Shift offset, execute SELECT query and fetch result
     * @return success
     */
    bool executeNext();
    /**
     * This is used to specify a function in the (column) list
     * It may be called more times.
     * @param funtext
     * @return success
     * @todo @b code: neimplementováno (pak doplnit do doc)
     */
    bool function(const string& funtext);
    /**
     * This is used to join tables if they can be performed automatically.
     * If not, returns false (no quarantee before version 2).
     * @warning no quarantee before version 2 !!
     * @return success
     * @todo @b code: neimplementováno (potom doplnit i doc)
     */
    bool join();
    /**
     * Get LIMIT argument
     * @return limit
     */
    int getLimit() { return this->limit; };
    /**
     * Set LIMIT argument
     * @param limit limit
     */
    void setLimit(const int limit) { this->limit = limit; };
    /**
     * This is used to specify the table for FROM statement and the column list for SELECT statement
     * It may be called more times.
     * @param table table to select
     * @param column column for select
     * @return success
     */
    bool from(const string& table, const string& column);
    /**
     * This is a WHERE statement construction function
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    bool whereString(const string& key, const string& value, const string& oper = "=", const string& from = "");
    /**
     * This is a WHERE statement construction function for integers
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    bool whereInt(const string& key, const int value, const string& oper = "=", const string& from = "");
    /**
     * This is a WHERE statement construction function for floats
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
    bool whereFloat(const string& key, const float value, const string& oper = "=", const string& from = "");
    /**
     * This is a WHERE statement construction function for seqtype
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereSeqtype(const string& key, const string& value, const string& oper = "=", const string& from = "");
    /**
     * This is a WHERE statement construction function for inouttype
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereInouttype(const string& key, const string& value, const string& oper = "=", const string& from = "");
    /**
     * This is a WHERE statement construction function for permissions
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
//     bool wherePermissions(const string& key, const string& value, const string& oper = "=", const string& from = "");
    /**
     * This is a WHERE statement construction function for timestamp
     * It can be called several times.
     * @param key key to compare with the value
     * @param value requested value for key
     * @param oper comparision operator between key and value
     * @param from table where the key is situated
     * @return success
     */
     bool whereTimestamp(const string& key, const time_t& value, const string& oper = "=", const string& from = "");
};

} // namespace vtapi

#endif	/* VTAPI_SELECT_H */

