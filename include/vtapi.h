/**
 * @file
 * @brief   Declaration of main %VTApi class which provides a basic functionality of %VTApi.
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef VTAPI_H
#define	VTAPI_H

#include "common/vtapi_config.h"
#include "data/vtapi_dataset.h"


/**
 * @brief Own namespace of %VTApi library
 */
namespace vtapi {

/**
 * @brief VTApi class manages Commons and processes args[]
 * This is how to begin
 *
 * @todo: include http://www.gnu.org/s/gengetopt/gengetopt.html (special interest to the configuration files is needed)
 *
 * @note Error codes 60*
 */ // ********************************************************************** //
class VTApi {
public:

    Commons*    commons;    /**< Commons are common objects to the project. */

public:

    /**
     * Constructor recomended by any program
     * @param argc   argument count (as in usual program)
     * @param argv   argument vector (as in usual program)
     */
    VTApi(int argc, char** argv);

    /**
     * Constructor
     * @param configFile   location of configuration file
     */
    VTApi(const std::string& configFile = "./vtapi.conf");

    /**
     * Constructor
     * @deprecated
     * @param connStr
     * @param location
     * @param user
     * @param password
     */
    VTApi(const std::string& connStr, const std::string& location, const std::string& user, const std::string& password);

    /**
     * Copy constructor
     * @param orig
     */
    VTApi(const Commons& orig);

    /**
     * Copy constructor
     * @param orig
     */
    VTApi(const VTApi& orig);

    /**
     * Destructor
     */
    ~VTApi();


    /**
     * This might be a HOW-TO function for learning and testing purposes
     * @see documentation -> examples -> vtapi.conf, SAMPLES.txt
     * @todo is this on given location?
     */
    void test();


    /**
     * This is how to continue after creating the API class...
     * @param name   specific dataset name
     * @return new Dataset
     */
    Dataset* newDataset(const std::string& name = "");

    /**
     * Methodology shortcuts
     * @param name   specific method name
     * @return new Method
     */
    Method* newMethod(const std::string& name = "");
    /**
     * @unimplemented
     */
    Method* addMethod(const Method& method);
    
    /**
     * Initializes app as vtapi Process instance
     * @param initState initial process state
     * @return Process instance object
     */
    Process *initProcess(ProcessState &initState);

    
private:

    void testGenericClasses();
    void testDataset();
    void testSequence(Dataset *);
    void testInterval(Sequence *);
    void testImage(Sequence *);
    void testVideo(Dataset *);
    void testMethod(Dataset *);
    void testProcess(Dataset *);

    void testPerformance();

};

} // namespace vtapi

#endif	/* VTAPI_H */

