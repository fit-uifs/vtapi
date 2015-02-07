/* 
 * File:   vtapi_insert.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:59 PM
 */

#ifndef VTAPI_INSERT_H
#define	VTAPI_INSERT_H

#include "vtapi_query.h"

namespace vtapi {

    
/**
 * @brief Class for constructing and executing INSERT queries
 * @see Basic information on page @ref KEYVALUES
 * @note Error codes 22*
 */
class Insert : public Query {
public:

    /**
     * Constructor of an INSERT query object
     * @param commons configuration object of Commons class
     * @param initString initialization string (query/table or empty)
     */
    Insert(const Commons& commons, const std::string& initString = "");
    /**
     * Get INSERT query string
     * @return query string
     */
    std::string getQuery();
    /**
     * Execute INSERT query
     * @return success
     */
    bool execute();
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool keyString(const std::string& key, const std::string& value, const std::string& from = "");
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     * @todo @b code: neimplementováno (pak zkontrolovat doc)
     */
    bool keyStringA(const std::string& key, std::string* values, const int size, const std::string& from = "");
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool keyInt(const std::string& key, int value, const std::string& from = "");
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     */
    bool keyIntA(const std::string& key, int* values, const int size, const std::string& from = "");
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool keyFloat(const std::string& key, float value, const std::string& from = "");
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param values values
     * @param size size of array
     * @param from selection (table; this is optional)
     * @return success
     */
    bool keyFloatA(const std::string& key, float* values, const int size, const std::string& from = "");
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool keySeqtype(const std::string& key, const std::string& value, const std::string& from = "");
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool keyInouttype(const std::string& key, const std::string& value, const std::string& from = "");
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
//    bool keyPermissions(const std::string& key, const std::string& value, const std::string& from = "");
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool keyTimestamp(const std::string& key, const time_t& value, const std::string& from = "");
    
#if HAVE_OPENCV
    /**
     * This is a persistent function to add keys (columns) and values
     * It may be called several times.
     * @param key key
     * @param value value
     * @param from selection (table; this is optional)
     * @return success
     */
    bool keyCvMat(const std::string& key, const cv::Mat& value, const std::string& from = "");
#endif
};

} // namespace vtapi

#endif	/* VTAPI_INSERT_H */

