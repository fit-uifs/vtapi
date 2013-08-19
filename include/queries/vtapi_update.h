/* 
 * File:   vtapi_update.h
 * Author: vojca
 *
 * Created on May 7, 2013, 1:00 PM
 */

#ifndef VTAPI_UPDATE_H
#define	VTAPI_UPDATE_H

namespace vtapi {
    class Update;
}

#include "vtapi_query.h"

namespace vtapi {
    

/**
 * @brief Class for constructing and executing UPDATE queries
 * @see Basic information on page @ref KEYVALUES
 * @note Error codes 23*
 */
class Update : public Query {
public:

    /**
     * Constructor of an Update query object
     * @param commons configuration object of Commons class
     * @param initString initialization string (query/table or empty)
     */
    Update(const Commons& commons, const string& initString = "");
    /**
     * Get UPDATE query string
     * @return query string
     */
    string getQuery();
    /**
     * Execute UPDATE query
     * @return success
     */
    bool execute();
    /**
     * This is a persistent function to set keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool setString(const string& key, const string& value, const string& from = "");
    /**
     * This is a persistent function to set keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     * @todo @b code: neimplementováno (pak zkontrolovat doc)
     */
    bool setStringA(const string& key, string* values, const int size, const string& from = "");
    /**
     * This is a persistent function to set keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool setInt(const string& key, int value, const string& from = "");
    /**
     * This is a persistent function to set keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     */
    bool setIntA(const string& key, int* values, const int size, const string& from = "");
    /**
     * This is a persistent function to set keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool setFloat(const string& key, float value, const string& from = "");
    /**
     * This is a persistent function to set keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     */
    bool setFloatA(const string& key, float* values, const int size, const string& from = "");
    /**
     * This is a persistent function to set keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool setSeqtype(const string& key, const string& value, const string& from = "");
    /**
     * This is a persistent function to set keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool setInouttype(const string& key, const string& value, const string& from = "");
    /**
     * This is a persistent function to set keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
//    bool setPermissions(const string& key, const string& value, const string& from = "");
    /**
     * This is a persistent function to set keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool setTimestamp(const string& key, const time_t& value, const string& from = "");
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

#endif	/* VTAPI_UPDATE_H */

