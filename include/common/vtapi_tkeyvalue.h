/* 
 * File:   vtapi_tkeyvalue.h
 * Author: vojca
 *
 * Created on May 7, 2013, 1:06 PM
 */

#ifndef VTAPI_TKEYVALUE_H
#define	VTAPI_TKEYVALUE_H

#include "vtapi_tkey.h"

namespace vtapi {

typedef vector<TKey *>     TKeyValues;


/**
 * @brief A generic class for storing a single Key-Value(s) pair
 *
 *
 * @warning
 *     - use PDOs only ... http://en.wikipedia.org/wiki/Plain_old_data_structure <br>
 *     - if you use pointers, you shouldn't free them
 * @note You can use size=-1 for NULL :)
 *
 * @see http://en.wikipedia.org/wiki/Plain_old_data_structure
 * @see http://www.cplusplus.com/doc/tutorial/templates/
 * @see http://stackoverflow.com/questions/2627223/c-template-class-constructor-with-variable-arguments
 * @see http://www.cplusplus.com/reference/std/typeinfo/type_info/
 */
template <typename T>
class TKeyValue : public TKey {
public:

    string      typein;     /**< This attribute is there for validation */
    T           *values;    /**< values */

public:
    /**
     * Default constructor
     */
    TKeyValue() : TKey(), values(NULL) {};

    /**
     * Full constructor with single value
     * @param type key data type
     * @param key key name (column name)
     * @param value single value
     * @param from additional key specification (eg. table)
     */
    TKeyValue(const string& type, const string& key, T value, const string& from = "")
            : TKey(type, key, 1, from) {
        this->values = new T[1];
        this->values[0] = value;
        this->typein = typeid(this->values).name();
    }
    /**
     * Full constructor with multiple values
     * @param type key data type
     * @param key key name (column name)
     * @param values array of values
     * @param size values array size
     * @param from additional key specification (eg. table)
     */
    TKeyValue (const string& type, const string& key, T* values, const int size, const string& from = "")
            : TKey(type, key, size, from) {
        if (this->size > 0) {
            this->values = new T[this->size];
            std::copy(values, values+this->size, this->values);
        }
        this->typein = typeid(values).name();
    }

    // TODO: tady nekde by melo byt pretypovani na standardni C/databazove typy
    
    /**
     * Destructor
     */
    ~TKeyValue () {
        destructall(values);
        
    }

    /**
     * Print values from TKey members
     * @return string which contains a dump of TKey members
     */
    string print();

    /**
     * Gets string representation of the first value
     * @return representation of value(s)
     */
    string getValue();
    /**
     * Gets string representation of first limit values
     * @param limit how many values, less than 0 = unlimited
     * @return representation of value(s)
     */
    string getValues(const int limit = 0);
};


/**
 * Prints string representation of Key-Value(s) pair
 * @return Key-Value string
 */
template <class T>
string TKeyValue<T>::print() {
    string retString = "TKeyValue<" + string(typeid(values).name()) + "> type=" + type +
            ", key=" + key + ", from=" + from + ", size=" + toString(size) + ",\n  values=";
    retString += this->getValues();
    cout << retString << endl;
    return (retString);
}

/**
 * Gets string representation of single value
 * @return value string
 */
template <class T>
string TKeyValue<T>::getValue() {
    if (values && size > 0) {
        return toString(values[0]);
    }
    else {
        return string("");
    }
}

/**
 * Gets string representation of values array
 * @param limit maximum limit of values (0 = no limit)
 * @return values string
 */
template <class T>
string TKeyValue<T>::getValues(const int limit) {
    string retString = "";
    if (values && size > 0) {
        if (size > 1) retString += "[";
        for(int i = 0; (i < size) && (limit <= 0 || i < limit); ++i) {
            retString += toString(values[i]) + ",";
        }
        if (limit <= 0) retString = retString.erase(retString.length()-1);
        if (size > 1) retString += "]";
    }
    return retString;
}


} // namespace vtapi

#endif	/* VTAPI_TKEYVALUE_H */
