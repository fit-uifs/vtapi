/* 
 * File:   vtapi_tkeyvalue.h
 * Author: vojca
 *
 * Created on May 7, 2013, 1:06 PM
 */

#ifndef VTAPI_TKEYVALUE_H
#define	VTAPI_TKEYVALUE_H

namespace vtapi {
    template <class T>
    class TKeyValue;
}

#include <typeinfo>

#include "vtapi_tkey.h"
#include "vtapi_global.h"

namespace vtapi {

typedef vector<TKey *>     TKeyValues;


/**
 * @brief A generic class for storing a single keyvalue type
 *
 * It uses std::copy (memcpy) to maintain the object data (except pointer targets)
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
 * @todo @b doc: parametry konstruktorů
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
     *
     * @param type
     * @param key
     * @param value
     * @param from table (optional)
     */
    TKeyValue(const string& type, const string& key, T value, const string& from = "")
            : TKey(type, key, 1, from) {
        this->values = new T[1];
        this->values[0] = value;
        this->typein = typeid(this->values).name();
    }
    TKeyValue (const string& type, const string& key, T* values, const int size, const string& from = "")
            : TKey(type, key, size, from) {
        if (this->size > 0) {
            this->values = new T[this->size];
            std::copy(values, values+this->size, this->values);
        }
        // memcpy(this->values, values, size*sizeof(values));
        this->typein = typeid(values).name();
    }

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


// tohle kdyz dam jinam, tak je to v haji - proc?
// to Petr: cti manualy, nejde to. Deklarace a definice template musi byt v jednom souboru
template <class T>
string TKeyValue<T>::print() {
    string retString = "TKeyValue<" + string(typeid(values).name()) + "> type=" + type +
            ", key=" + key + ", from=" + from + ", size=" + toString(size) + ",\n  values=";
    retString += this->getValues();
    cout << retString << endl;
    return (retString);
}

template <class T>
string TKeyValue<T>::getValue() {
    if (values && size > 0) {
        return toString(values[0]);
    }
    else {
        return string("");
    }
}

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

