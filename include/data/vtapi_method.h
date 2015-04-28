/**
 * @file
 * @brief   Declaration of Method class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef VTAPI_METHOD_H
#define	VTAPI_METHOD_H

#include "vtapi_keyvalues.h"
#include "vtapi_process.h"

namespace vtapi {

class Process;

/**
 * @brief A class which represents methods and gets also their keys
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @note Error codes 35*
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Method : public KeyValues {
public:

    TKeys methodKeys; /**< A vector of key-value pairs*/

public:
    
    /**
     * Constructor for methods
     * @param orig   pointer to the parrent KeyValues object
     * @param name   specific name of method, which we can construct
     */
    Method(const KeyValues& orig, const std::string& name = "");

    /**
     * Moves to a next method and set a method name and its methodkeys variables
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next();

    /**
     * Gets method ID
     * @return method ID
     */
    int getId();
    /**
     * Gets a name of the current method
     * @return string value with the name of the method
     */
    std::string getName();
    /**
     * This is used to refresh the methodKeys vector
     * @return TKeys
     */
    TKeys getMethodKeys();
    /**
     * Prints method's keys
     */
    void printMethodKeys();
    /**
     * Creates process object for access to existing processes
     * @return pointer to new process object
     */
    Process* newProcess(const std::string& name = "");
    /**
     * Creates process object for starting new process
     * @return new process object
     */
    Process* addProcess();  

    /**
     * @unimplemented
     */
    Method* add(const std::string& name);
    
    /**
     * @unimplemented
     */
    Method* loadCode();    

    /**
     * @unimplemented
     */
    Method* saveCode();    
    
    /**
     * @unimplemented
     */
    void init();
    /**
     * @unimplemented
     */
    void exit();
    
    /**
     * @unimplemented
     */
    std::string getLastError() { return std::string(""); }
    
private:

    /**
     * @unimplemented
     */
    void printData(const std::string& inout);

};

} // namespace vtapi

#endif	/* VTAPI_METHOD_H */

