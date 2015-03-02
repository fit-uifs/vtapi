/**
 * @file
 * @brief   Declaration of TKey class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef VTAPI_TKEY_H
#define	VTAPI_TKEY_H


namespace vtapi {

class TKey;

/**
 * TKeys is an abbreviation for a vector of TKey values 
 */
typedef std::vector<TKey> TKeys;

/**
 * @brief Class represents a generic Key (of the Key-Value pair)
 *
 * Used in queries (size > 0 for vectors)
 *
 * @note You can use size = -1 for NULL
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class TKey {
public:
    std::string type;    /**< Name of the data type */
    std::string key;     /**< Name of the column */
    int         size;    /**< Positive for array, 0 for single value, -1 for NULL */
    std::string from;    /**< The source (table) */

public:
    /**
     * Default constructor
     */
    TKey() : size(-1) {};
    /**
     * Copy constructor
     * @param orig   TKey to copy
     */
    TKey(const TKey& orig) : type(orig.type), key(orig.key), from(orig.from), size(orig.size) {};
    /**
     * Constructor with full specification of arguments
     * @param type   key data type
     * @param key    key name (column name)
     * @param size   negative value represents NULL, 1 represents single value, value grater than 1 represents vectors
     * @param from   additional key specification (eg. table)
     * @todo comment for class member size and for parametr size of constructor are inconsistent (0 for single value and positive for array vs. 1 for single value and greater than 1 are vectors
     */
    TKey(const std::string& type, const std::string& key, const int size, const std::string& from = "") : type(type), key(key), size(size), from(from) {};

    virtual ~TKey() {};

    /**
     * Prints data
     * @return string of TKey data
     */
    virtual std::string print();

    /**
     * Gets string representation of the first value
     * @return representation of value(s)
     * @unimplemented
     */
    virtual std::string getValue() { return ""; };
    /**
     * Gets string representation of first limit values
     * @param limit how many values
     * @return representation of value(s)
     * @unimplemented
     */
    virtual std::string getValues(const int limit = 0) { return ""; };

};

} // namespace vtapi

#endif	/* VTAPI_TKEY_H */

