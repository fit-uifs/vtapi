/* 
 * File:   vtapi_interval.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:57 PM
 */

#ifndef VTAPI_INTERVAL_H
#define	VTAPI_INTERVAL_H

#include "vtapi_keyvalues.h"

namespace vtapi {
    
class Sequence;


/**
 * @brief Interval is equivalent to an interval of images
 *
 * @see Basic definition on page @ref LOGICAL
 *
 * @note Error codes 33*
 */
class Interval : public KeyValues {
public:
    
    /**
     * Constructor for intervals
     * @param orig pointer to the parrent KeyValues object
     * @param selection specific name of a selection table
     */
    Interval(const KeyValues& orig, const string& selection = "intervals");

    /**
     * Unnecessary
     * @return success
     * @note Overloading next() from KeyValues
     */
    //bool next();

    /**
     * Get sequence name of parent sequence
     * @return sequence string
     */
    string getSequenceName();

    /**
     * Get sequence object of parent sequence
     * @return sequence object
     */
    Sequence* getParentSequence();

    /**
     * Get a start time of the current interval
     * @return start time
     */
    int getStartTime();
    /**
     * Get an end time of the current interval
     * @return end time
     */
    int getEndTime();

    /**
     * Add new interval to the table - minimal insert
     * @param sequence interval name
     * @param t1 start time
     * @param t2 end time
     * @param location location of the image
     * @return success
     */
    bool add(const string& sequence, const int t1, const int t2 = -1, const string& location = "");
    /**
     * Add new interval to the table - full insert
     * @param sequence interval name
     * @param t1 start time
     * @param t2 end time
     * @param location location of the image
     * @param userid name of the owner
     * @param notes optional description
     * @return success
     */
    bool add(const string& sequence, const int t1, const int t2, const string& location,
    const string& userid, const string& notes);
    /**
     * This is used to support updates
     * @return success (in derived classes)
     */
    /**
     * Execute SQL INSERT command for interval
     * @return success
     * @note Overloading addExecute from KeyValues
     */
    bool addExecute();
    /**
     * Reset interval to initial state
     * @return success
     */
    bool preSet();

};

/**
 * @brief This represents images
 *
 * @see Basic definition on page @ref LOGICAL
 *
 * @note Error codes 339*
 */
class Image : public Interval {
public:

    /**
     * Constructor for Images
     * @param orig pointer to the parrent KeyValues object
     * @param selection specific name of a selection table
     */
    Image(const KeyValues& orig, const string& selection = "intervals");

    /**
     * Get a sequence (order) number of the current image (interval)
     * @return number or 0
     */
    int getTime();

    /**
     * This is most probably what you always wanted...
     * @return string value with the location of the data
     * @note overloading getDataLocation from Commons
     */
    string getDataLocation();
    /**
     * Simply adds an image (interval) to the sequence table (no checking)
     * @param sequence interval name
     * @param t time (start time is the same as the end time)
     * @param location location of the image
     * @return success
     */
    bool add(const string& sequence, const int t, const string& location);

#ifdef __OPENCV_CORE_HPP__    
    cv::Mat image;
    
    cv::Mat getData();
#endif
    
protected:

    /**
     * This is here just for image name
     * @return string value with the location of the image
     */
    string getImgLocation();

};

} // namespace vtapi

#endif	/* VTAPI_INTERVAL_H */

