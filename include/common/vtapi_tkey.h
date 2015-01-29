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

typedef std::vector<TKey> TKeys;

/**
 * @brief Class represents a generic Key (of the Key-Value pair)
 *
 * Used in queries (size>0 for vectors)
 *
 * @note You can use size=-1 for NULL
 */
class TKey {
public:
    std::string type;    /**< Name of the data type */
    std::string key;     /**< Name of the column */
    int         size;    /**< positive for array, 0 for single value, -1 for NULL */
    std::string from;    /**< The source (table) */

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
     * Constructor with full specification of arguments
     * @param type key data type
     * @param key key name (column name)
     * @param size negative for NULL, 1 for single value, more than 1 for vectors
     * @param from additional key specification (eg. table)
     */
    TKey(const std::string& type, const std::string& key, const int size, const std::string& from = "") : type(type), key(key), size(size), from(from) {};

    virtual ~TKey() {};

    /**
     * Print data
     * @return string of TKey data
     */
    virtual std::string print();

    /**
     * Gets string representation of the first value
     * @return representation of value(s)
     */
    virtual std::string getValue() { return ""; };
    /**
     * Gets string representation of first limit values
     * @param limit how many values
     * @return representation of value(s)
     */
    virtual std::string getValues(const int limit = 0) { return ""; };

};

} // namespace vtapi

#endif	/* VTAPI_TKEY_H */

