/**
 * @file
 * @brief Main classes which provide a basic functionality of %VTApi.
 *
 * @copyright Brno University of Technology &copy; 2011 &ndash; 2012
 *
 * VTApi is distributed under BUT OPEN SOURCE LICENCE (Version 1).
 * This licence agreement provides in essentials the same extent of rights as the terms of GNU GPL version 2 and Software fulfils the requirements of the Open Source software.
 *
 * @authors
 * Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 *
 * @todo @b doc[PC]: dodelat create db
 */

#ifndef VTAPI_H
#define	VTAPI_H

#include "vtapi_data.h"

namespace vtapi {


/**
 * @brief VTApi class manages Commons and processes args[]
 * This is how to begin
 *
 * TODO: include http://www.gnu.org/s/gengetopt/gengetopt.html
 *       special interest to the configuration files is needed
 *
 * @note Error codes 60*
 */ // ********************************************************************** //
class VTApi {
public:

    Commons*    commons;    /**< Commons are common objects to the project. */

public:

    /**
     * Constructor recomended by any program
     * @param argc
     * @param argv
     * @todo @b doc: "(in the future) je stále aktuální?
     * @todo @b doc: další konstruktory
     */
    VTApi(int argc, char** argv);

    /**
     * Constructor
     * @deprecated
     * @param configFile location
     */
    VTApi(const string& configFile);

    /**
     * Constructor
     * @deprecated
     * @param connStr
     * @param location
     * @param user
     * @param password
     */
    VTApi(const string& connStr, const string& location, const string& user, const string& password);

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
     */
    void test();


    /**
     * This is how to continue after creating the API class...
     * @param name specific dataset name
     * @return new dataset
     */
    Dataset* newDataset(const string& name = "");

    /**
     * Methodology shortcuts
     * @param name
     * @return
     */
    Method* newMethod(const string& name = "");
    Method* addMethod(const Method& method);

private:

    void testGenericClasses();
    void testDataset();
    void testSequence(Dataset *);
    void testInterval(Sequence *);
    void testImage(Sequence *);
    void testVideo(Dataset *);
    void testMethod(Dataset *);
    void testProcess(Dataset *);

    void testPerformance ();

};

} // namespace vtapi

#endif	/* VTAPI_H */

