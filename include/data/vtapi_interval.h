/**
 * @file
 * @brief   Declaration of Interval and Image classes
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#ifndef VTAPI_INTERVAL_H
#define	VTAPI_INTERVAL_H

#include "vtapi_keyvalues.h"

namespace vtapi {
    
class Sequence;


/**
 * @brief Interval is equivalent to an interval of images
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @note Error codes 33*
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Interval : public KeyValues {
public:
    
    /**
     * Constructor for intervals
     * @param orig        pointer to the parrent KeyValues object
     * @param selection   specific name of a selection table
     */
    Interval(const KeyValues& orig, const std::string& selection = "intervals");

    /*
     * Unnecessary
     * @return success
     * @note Overloading next() from KeyValues
     */
    //bool next();

    /**
     * Gets interval ID
     * @return interval ID
     */
    int getId();
    
    /**
     * Gets sequence name of parent sequence
     * @return sequence string
     */
    std::string getSequenceName();

    /**
     * Gets sequence object of parent sequence
     * @return sequence object
     */
    Sequence* getParentSequence();

    /**
     * Gets a start time of the current interval
     * @return start time
     */
    int getStartTime();
    /**
     * Gets an end time of the current interval
     * @return end time
     */
    int getEndTime();

    /**
     * Adds new interval to the table - minimal insert
     * @param sequence   interval name
     * @param t1         start time
     * @param t2         end time
     * @param location   location of an image
     * @return success
     */
    bool add(const std::string& sequence, const int t1, const int t2 = -1, const std::string& location = "");
    /**
     * Adds new interval to the table - full insert
     * @param sequence   interval name
     * @param t1         start time
     * @param t2         end time
     * @param location   location of an image
     * @param userid     name of an owner
     * @param notes      optional description
     * @return success
     */
    bool add(const std::string& sequence, const int t1, const int t2, const std::string& location,
    const std::string& userid, const std::string& notes);

    /**
     * Executes SQL INSERT command for interval
     * @return success
     * @note Overloading addExecute from KeyValues
     */
    bool addExecute();
    /**
     * Resets interval to the initial state
     * @return success
     */
    bool preSet();

    /**
     * Represents intervals only for selected sequence
     * @param seqname   sequence name
     */
    void filterBySequence(const std::string& seqname);
};





/**
 * @brief This represents images
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @note Error codes 339*
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Image : public Interval {
public:

    /**
     * Constructor for Images
     * @param orig        pointer to the parrent KeyValues object
     * @param selection   specific name of a selection table
     */
    Image(const KeyValues& orig, const std::string& selection = "intervals");

    /**
     * Gets a sequence (order) number of the current image (interval)
     * @return sequence number or 0
     */
    int getTime();

    /**
     * This is most probably what you always wanted...
     * @return string value with the location of the data
     * @note overloading getDataLocation from Commons
     */
    std::string getDataLocation();
    /**
     * Simply adds an image (interval) to the sequence table (no checking)
     * @param sequence   interval name
     * @param t          time (start time is the same as the end time)
     * @param location   location of the image
     * @return success
     */
    bool add(const std::string& sequence, const int t, const std::string& location);

#if HAVE_OPENCV  
    cv::Mat image;   /**< Image file data */
    /**
     * Loads an image from a file (given by imgLocation of %VTApi)
     * @return image file data
     */
    cv::Mat getData();
#endif
    
protected:

    /**
     * This is here just for image name
     * @return string value with the location of the image
     */
    std::string getImgLocation();

};

} // namespace vtapi

#endif	/* VTAPI_INTERVAL_H */

