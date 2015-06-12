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
     * Moves to next interval
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next();

    /**
     * Gets interval ID
     * @return interval ID
     */
    int getId();
    /** 
     * Gets process name of associated process
     * @return process string
     */
    std::string getProcessName();
    /**
     * Gets sequence name of parent sequence
     * @return sequence string
     */
    std::string getSequenceName();
    /**
     * Gets a start frame of the current interval
     * @return start time
     */
    int getStartTime();
    /**
     * Gets an end frame of the current interval
     * @return end time
     */
    int getEndTime();
    /**
     * Gets real start and end time of the current interval
     * @param t1 start time
     * @param t2 end time
     * @return  sucess
     */
    bool getRealStartEndTime(time_t *t1, time_t *t2);
    
    /**
     * Sets interval's start/end time
     * @param t1 start time
     * @param t2 end time
     */
    bool updateStartEndTime(const int t1, const int t2 = -1);

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
     * Sets filter for intervals by ID before calling next()
     * @param id interval ID
     * @return success
     */
    bool filterById(const int id);
    /**
     * Sets filter for intervals by sequence before calling next()
     * @param seqname   sequence name
     * @return success
     */
    bool filterBySequence(const std::string& seqname);
    /**
     * Sets filter for intervals by process before calling next()
     * @param prsname process name
     * @return success
     */
    bool filterByProcess(const std::string& prsname);
    /**
     * Sets filter for intervals by duration(real seconds) before calling next()
     * @param t_low min duration in real seconds
     * @param t_high max duration in real seconds
     * @return success
     */
    bool filterByDuration(const float t_low, const float t_high);
    /**
     * Sets filter for intervals by overlapping time range before calling next()
     * @param t_low overlapping interval lower bound (UNIX time)
     * @param t_high overlapping interval higher bound (UNIX time)
     * @return success
     */
    bool filterByTimeRange(const time_t t_low, const time_t t_high);
    /**
     * Sets filter for intervals by overlapping event region before calling next()
     * @param region overlapping region
     * @return success
     */
    bool filterByRegion(const IntervalEvent::box& region);
    
protected:
    bool preUpdate();
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
     * @param name        image name
     * @param selection   specific name of a selection table
     */
    explicit Image(const KeyValues& orig, const std::string& name = "", const std::string& selection = "intervals");

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

