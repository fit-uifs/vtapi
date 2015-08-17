/**
 * @file
 * @brief   Declaration of Interval and Image classes
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include "keyvalues.h"
#include "dataset.h"
#include "sequence.h"
#include "task.h"
#include "intervalevent.h"

namespace vtapi {

class Dataset;
class Sequence;
class Task;


/**
 * @brief Interval is equivalent to an interval of images
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Interval : public KeyValues
{
public:
    /**
     * Constructor for intervals
     * @param commons     shared Commons object
     * @param selection   specific name of a selection table
     */
    Interval(const Commons& commons, const std::string& selection = "intervals");

    /**
     * Destructor
     */
    virtual ~Interval();

    /*
     * Moves to next interval
     * @return success
     */
    virtual bool next() override;

    /**
     * @brief Gets parent dataset object
     * @return dataset object (initialized)
     */
    Dataset *getParentDataset();

    /**
     * @brief Gets parent task object
     * @return task object (initialized)
     */
    Task *getParentTask();

    /**
     * @brief Gets parent sequence name
     * @return sequence name
     */
    std::string getParentSequenceName();

    /**
     * @brief Gets parent sequence object
     * @return sequence object (initialized)
     */
    Sequence *getParentSequence();

    /**
     * Gets interval ID
     * @return interval ID
     */
    int getId();
    
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
     * Sets filter for intervals by sequences before calling next()
     * @param seqnames   sequences names
     * @return success
     */
    bool filterBySequences(const std::list<std::string>& seqnames);
    /**
     * Sets filter for intervals by task before calling next()
     * @param taskname task name
     * @return success
     */
    bool filterByTask(const std::string& taskname);
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

private:
    Interval() = delete;
    Interval(const Interval&) = delete;
    Interval& operator=(const Interval&) = delete;
};





/**
 * @brief This represents images
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @note Error codes 339*
 * 
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Image : public Interval
{
public:
    /**
     * Constructor for Images
     * @param commons     shared Commons object
     * @param name        image name
     * @param selection   specific name of a selection table
     */
    Image(const Commons& commons,
          const std::string& name = std::string(),
          const std::string& selection = "intervals");

    /*
     * Moves to next image
     * @return success
     */
    bool next() override;

    /**
     * Gets a sequence (order) number of the current image (interval)
     * @return sequence number or 0
     */
    int getTime();

    /**
     * This is most probably what you always wanted...
     * @return string value with the location of the data
     */
    std::string getDataLocation();
    
//    /**
//     * Simply adds an image (interval) to the sequence table (no checking)
//     * @param sequence   interval name
//     * @param t          time (start time is the same as the end time)
//     * @param location   location of the image
//     * @return success
//     */
//    bool add(const std::string& sequence, const int t, const std::string& location);

    /**
     * Loads an image from a file (given by imgLocation of %VTApi)
     * @return image file data
     */
    cv::Mat& getImageData();

private:
    cv::Mat _image;   /**< Image file data */

    Image() = delete;
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
};

} // namespace vtapi
