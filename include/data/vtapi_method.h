/* 
 * File:   vtapi_method.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:57 PM
 */

#ifndef VTAPI_METHOD_H
#define	VTAPI_METHOD_H

#include "vtapi_keyvalues.h"

namespace vtapi {

class Process;

/**
 * @brief A class which represents methods and gets also their keys
 *
 * @see Basic definition on page @ref LOGICAL
 *
 * @note Error codes 35*
 */
class Method : public KeyValues {
public:

    TKeys methodKeys; /**< A vector of key-value pairs*/

public:
    
    /**
     * Constructor for methods
     * @param orig pointer to the parrent KeyValues object
     * @param name specific name of method, which we can construct
     */
    Method(const KeyValues& orig, const std::string& name = "");

    /**
     * Move to a next method and set a method name and its methodkeys variables
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next();

    /**
     * Get a name of the current method
     * @return string value with the name of the method
     */
    std::string getName();
    /**
     * This is used to refresh the methodKeys vector
     * @return TKeys
     */
    TKeys getMethodKeys();

    /**
     * Create new process for current dataset
     * @return pointer to new process
     */
    Process* newProcess(const std::string& name);

    /**
     * Runs a process of this method - abstract
     * @return success
     * @todo code: neimplementováno
     */
    virtual bool run() { return false; }
   

    Method* add(const std::string& name);
    
    Method* loadCode();    

    Method* saveCode();    
    
    void init();
    void exit();
    
    // getters 
    
    // setters
    bool set() {};
    
    std::string getLastError() { return std::string(""); }
    void releaseProcess(Process *p) {}

private:

    /**
     *
     * @param inout
     * @todo code: neimplementováno (pak doplnit i doc)
     */
    void printData(const std::string& inout);

};

} // namespace vtapi

#endif	/* VTAPI_METHOD_H */

