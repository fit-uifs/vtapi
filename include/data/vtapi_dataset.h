/* 
 * File:   vtapi_dataset.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:51 PM
 */

#ifndef VTAPI_DATASET_H
#define	VTAPI_DATASET_H

#include "vtapi_keyvalues.h"

namespace vtapi {
    
class Sequence;
class Video;
class Method;
class Process;

/**
 * @brief This class should always be on the path of your program...
 *
 * @see Basic definition on page @ref LOGICAL
 *
 * @note Error codes 31*
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
     * @param orig pointer to the parent KeyValues object
     * @param name specific dataset name
     * @todo @b doc: nejsem si jist, zda i zde je parametr "const KeyValues& orig" nadřazený prvek
     */
    Dataset(const KeyValues& orig, const string& name = "");

    /**
     * Move to a next dataset and set dataset name and location varibles
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next();

    /**
     * Get name of the current dataset
     * @return name name of the current dataset
     */
    string getName();

    /**
     * Get location of the current dataset
     * @return location of the current dataset
     */
    string getLocation();

    /**
     * Create new sequence object for the current dataset
     * @param name sequence name (no name = represent all sequences)
     * @return pointer to the new sequence object
     */
    Sequence* newSequence(const string& name = "");

    /**
     * Create new video (sequence) object for the current dataset
     * @param name video (sequence) name (no name = represent all sequences)
     * @return pointer to the new video object
     */
    Video* newVideo(const string& name = "");

    /**
     * Create new method object for the current dataset
     * @param name method name (no name = represent all methods)
     * @return pointer to the new method object
     */
    Method* newMethod(const string& name = "");

    /**
     * Create new process object for the current dataset
     * @param name process name (no name = represent all processes)
     * @return pointer to the new process object
     */
    Process* newProcess(const string& name = "");

};

} // namespace vtapi

#endif	/* VTAPI_DATASET_H */

