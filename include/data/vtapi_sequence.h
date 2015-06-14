/**
 * @file
 * @brief   Declaration of Sequence, Video and VideoPlayer classes
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */


#pragma once

#include <ctime>
#include <string>
#include "vtapi_keyvalues.h"
#include "vtapi_interval.h"

namespace vtapi {

/**
 * @brief A Sequence class manages videos and images
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @note Error codes 32*
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Sequence : public KeyValues
{
public:
    /**
     * Constructor for sequences
     * @param orig   pointer to the parrent KeyValues object
     * @param name   specific sequence name
     */
    explicit Sequence(const KeyValues& orig, const std::string& name = "");

    /**
     * Destructor
     */
    virtual ~Sequence();
    
    /**
     * Moves to a next sequence and sets sequence name and location varibles
     * @return success
     * @note Overloading next() from KeyValues
     */
    virtual bool next();

    /**
     * Gets a sequence name
     * @return string value with the name of the sequence
     */
    std::string getName();

    /**
     * Gets a sequence location
     * @return string value with the location of the sequence
     */
    std::string getLocation();
    
    /**
     * Gets a sequence type
     * @return string value [video|images|data]
     */
    std::string getType();
    
    /**
     * Adds a new sequence to a database table - minimal insert
     * @param name       name of the sequence
     * @param location   location of the sequence
     * @param type       type of the sequence
     * @return success
     */
    bool add(const std::string& name, const std::string& location, const std::string& type);

    /**
     * Adds a new sequence to a database table - full insert
     * @param name       name of the sequence
     * @param location   location of the sequence
     * @param type       type of the sequence
     * @param userid     name of an owner
     * @param notes      optional description
     * @return success
     */
    bool add(
        const std::string& name,
        const std::string& location,
        const std::string& type,
        const std::string& userid,
        const std::string& notes);
    
    /**
     * Creates a new interval specified by a start time and an end time
     * @return pointer to the new interval
     */
    Interval* newInterval()
    { return new Interval(*this); }

    /**
     * Creates a new image specified by a name
     * @param name   name of the image
     * @return pointer to the new image
     */
    Image* newImage(const std::string& name = "")
    { return new Image(*this, name); }
    
protected:
    bool preUpdate();
};


/**
 * @brief ImageFolder class manages folders with static images
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @note Error codes 321*
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class ImageFolder : public Sequence
{
public:
    /**
     * Constructor for ImageFolder
     * @param orig   pointer to the parent KeyValues object
     * @param name   specific image folder name
     */
    explicit ImageFolder(const KeyValues& orig, const std::string& name = "");

    /**
     * Adds a new image folder to the dataset
     * @param name       name of the image folder
     * @param location   location of the image folder
     * @return success
     */
    bool add(const std::string& name, const std::string& location);

    /**
     * Individual next() for image folder
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next();
};

/**
 * @brief Video class manages videos
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @note Error codes 321*
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Video : public Sequence
{
public:
    /**
     * Constructor for Video
     * @param orig   pointer to the parent KeyValues object
     * @param name   specific video name
     */
    explicit Video(const KeyValues& orig, const std::string& name = "");

    /**
     * Destructor
     */
    ~Video();
    
    /**
     * Adds a new video to the dataset
     * @param name       name of the video
     * @param location   location of the video
     * @return success
     */
    bool add(const std::string& name, const std::string& location, const time_t& realtime = 0);

    /**
     * Moves to a next video, releases capture and sets sequence name and location varibles
     * @return success
     * @note Overloading next() from Sequence
     */
    bool next();
    
#if HAVE_OPENCV
    cv::VideoCapture capture;   /**< Video file capture */
    
    /**
     * Opens a video capture (not necessary to call directly)
     * @todo Test
     * @return success
     */
    bool openVideo();
    
    /**
     * Closes a video capture (not necessary to call directly)
     */
    void closeVideo();

    /**
     * Gets next frame from current capture
     * @return frame of the video
     */
    cv::Mat getData();
    
    /**
     * Gets video length in frames
     * @return video length in frames
     * @unimplemented
     */
    size_t getLength();
    
    /**
     * Gets video FPS rate
     * @return FPS
     */
    float getFPS();
    
    /**
     * Gets video real-world start time
     * @return start time
     */
    time_t getRealStartTime();
    
    /**
     * Sets video real-world start time
     * @return success
     */
    bool updateRealStartTime(const time_t& starttime);
    
#endif

};


} // namespace vtapi
