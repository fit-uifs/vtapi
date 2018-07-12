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
#include "eventfilter.h"
#include "intervalevent.h"

namespace vtapi {

class Dataset;
class Sequence;
class Video;
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
     * @brief Copy constructor
     */
    Interval(const Interval &copy);

    /**
     * Constructor for intervals
     * @param commons     shared Commons object
     * @param selection   specific name of a selection table
     */
    Interval(const Commons& commons, const std::string& selection);

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
    Dataset *getParentDataset() const;

    /**
     * @brief Gets parent task object
     * @return task object (initialized)
     */
    Task *getParentTask() const;

    /**
     * @brief Gets parent sequence name
     * @return sequence name
     */
    std::string getParentSequenceName() const;

    /**
     * @brief Gets parent sequence object
     * @return sequence object (initialized)
     */
    Sequence *getParentSequence() const;

    /**
     * Gets interval ID
     * @return interval ID
     */
    int getId() const;

    /**
     * Gets a start frame of the current interval
     * @return start time
     */
    unsigned int getStartTime() const;

    /**
     * Gets an end frame of the current interval
     * @return end time
     */
    unsigned int getEndTime() const;

    /**
     * Gets real start time of the current interval
     * @return start time
     */
    std::chrono::system_clock::time_point calculateRealStartTime();

    /**
     * Gets real end time of the current interval
     * @return end time
     */
    std::chrono::system_clock::time_point calculateRealEndTime();

    /**
     * @brief Gets event length in seconds
     * @return seconds
     */
    double getLengthSeconds() const;

    /**
     * Sets interval's end time
     * @param t1 start time
     */
    bool updateStartTime(unsigned int t1);

    /**
     * Sets interval's end time
     * @param t2 end time
     */
    bool updateEndTime(unsigned int t2);

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
    bool filterBySequences(const std::vector<std::string>& seqnames);

    /**
     * Sets filter for intervals by task before calling next()
     * @param taskname task name
     * @return success
     */
    bool filterByTask(const std::string& taskname);

    /**
     * @brief Sets filter for intervals by examining its event value
     * @param eventkey column for event
     * @param taskname events' task
     * @param seqnames filter events for these sequences
     * @param filter filter definition
     * @return success
     */
    bool filterByEvent(const std::string& eventkey, const std::string& taskname,
                       const std::vector<std::string>& seqnames, const EventFilter & filter);

protected:
    virtual bool preUpdate() override;

private:
    std::shared_ptr<Video> _pparent_vid;

    Interval() = delete;
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
          const std::string& selection,
          const std::string& name = std::string());

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

    /**
     * Loads image data
     * @return image file data
     */
    cv::Mat getImageData();

private:

    Image() = delete;
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
};

} // namespace vtapi
