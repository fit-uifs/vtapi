/* 
 * File:   vtapi_tkey.h
 * Author: vojca
 *
 * Created on May 7, 2013, 1:05 PM
 */

#ifndef VTAPI_TKEY_H
#define	VTAPI_TKEY_H

namespace vtapi {
    class TKey;
}

#include "vtapi_global.h"

namespace vtapi {


typedef vector<TKey>    TKeys;

/**
 * @brief This represents the Key (of the Key-Value concept)
 *
 * Used in queries (size>0 for vectors)
 *
 * @note You can use size=-1 for NULL
 */
class TKey {
public:
    string      type;    /**< Name of the data type */
    string      key;     /**< Name of the column */
    int         size;    /**< positive for array, 0 for single value, -1 for NULL */
    string      from;    /**< The source (table) */

public:
    /**
     * Default constructor
     */
    TKey() : size(-1) {};
    /**
     * Copy constructor
     * @param orig key to copy
     */
    TKey(const TKey& orig) : type(orig.type), key(orig.key), from(orig.from), size(orig.size) {};
    /**
     * Constructor for full specification of arguments
     * @param type name of a data type
     * @param key name of a column
     * @param size "0" is the value right now
     * @param from distinguish between in/out right now
     * @todo @b doc: asi zobecnit - použito v keyvalues, method, query, video, vtapi a ne ke všemu to, dle mého, sedí
     */
    TKey(const string& type, const string& key, const int size, const string& from = "") : type(type), key(key), size(size), from(from) {};

    virtual ~TKey() {};

    /**
     * Print data
     * @return string of TKey data
     */
    virtual string print();

    /**
     * Gets string representation of the first value
     * @return representation of value(s)
     */
    virtual string getValue() { return ""; };
    /**
     * Gets string representation of first limit values
     * @param limit how many values
     * @return representation of value(s)
     */
    virtual string getValues(const int limit = 0) { return ""; };

};

} // namespace vtapi

#endif	/* VTAPI_TKEY_H */

