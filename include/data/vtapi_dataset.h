/**
 * @file
 * @brief   Declaration of Dataset class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef VTAPI_DATASET_H
#define	VTAPI_DATASET_H

#include "vtapi_keyvalues.h"
#include "vtapi_sequence.h"
#include "vtapi_method.h"
#include "vtapi_process.h"

namespace vtapi {

/**
 * @brief This class should always be on the path of your program...
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @note Error codes 31*
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Dataset : public KeyValues {
public:

    /**
     * This is the recommended constructor.
     *
     * @warning you can ommit the \a name only in these cases:
     *    -# Don't know the name -> use next
     *    -# The dataset is in your vtapi.conf
     *
     * @param orig   pointer to the parent KeyValues object (in this case usually Commons object)
     * @param name   specific dataset name
     * @note Use rather "vtapi->newDataset()"
     */
    Dataset(const KeyValues& orig, const std::string& name = "");

    /**
     * Moves to a next dataset and sets dataset name and location varibles
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next();

    /**
     * Gets name of the current dataset
     * @return name of the current dataset
     */
    std::string getName();

    /**
     * Gets location of the current dataset
     * @return location of the current dataset
     */
    std::string getLocation();

    /**
     * Creates new Sequence object for the current dataset
     * @param name   sequence name (no name = all sequences)
     * @return pointer to the new Sequence object
     */
    Sequence* newSequence(const std::string& name = "");

    /**
     * Creates new Video (Sequence) object for the current dataset
     * @param name   video (sequence) name (no name = all sequences)
     * @return pointer to the new Video object
     */
    Video* newVideo(const std::string& name = "");
    /**
     * Creates new ImageFolder (Sequence) object for the current dataset
     * @param name image folder name (no name = all image folders)
     * @return pointer to the new ImageFolder object
     */
    ImageFolder* newImageFolder(const std::string& name = "");
    /**
     * Creates new Method object for the current dataset
     * @param name   method name (no name = all methods)
     * @return pointer to the new Method object
     */
    Method* newMethod(const std::string& name = "");

    /**
     * Creates new Process object for the current dataset
     * @param name   process name (no name = all processes)
     * @return pointer to the new Process object
     */
    Process* newProcess(const std::string& name = "");

};

} // namespace vtapi

#endif	/* VTAPI_DATASET_H */

