/**
 * @file
 * @brief   Declaration of Sequence, Video and VideoPlayer classes
 *
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
     * Construct sequence object for iterating through VTApi sequences (video/imagefolder)
     * If a specific name is set, object will represent one sequence only
     * @param commons base Commons object
     * @param name sequence name (no name = all sequences)
     */
    Sequence(const Commons& orig, const std::string& name = std::string());

    /**
     * Construct sequence object for iterating through VTApi sequences (video/imagefolder)
     * Object will represent set of sequences specified by their names
     * @param commons base Commons object
     * @param names list of sequences names
     */
    Sequence(const Commons& commons, const std::list<std::string>& names);
    
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
     * Gets a sequence type
     * @return string value [video|images|data]
     */
    std::string getType();
    
    /**
     * Gets sequence comment
     * @return string value
     */
    std::string getComment();

    /**
     * Gets full path to sequence location
     * @return full path
     */
    std::string getDataLocation();
    
    /**
     * Sets video's comment
     * @param comment new comment
     * @return success
     */
    bool updateComment(const std::string& comment);
    
protected:
    bool preUpdate();

private:
    Sequence() = delete;
    Sequence(const Sequence&) = delete;
    Sequence& operator=(const Sequence&) = delete;
};


/**
 * @brief ImageFolder class manages folders with static images
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @note Error codes 321*
 * 
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
     * Construct image folder object for iterating through VTApi image folders
     * If a specific name is set, object will represent one image folder only
     * @param commons base Commons object
     * @param name image folder name (no name = all image folders)
     */
    ImageFolder(const Commons& orig, const std::string& name = std::string());

    /**
     * Construct image folder object for iterating through VTApi image folders
     * Object will represent set of image folders specified by their names
     * @param commons base Commons object
     * @param names list of image folders names
     */
    ImageFolder(const Commons& commons, const std::list<std::string>& names);

    /**
     * Individual next() for image folder
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next();
    
private:
    ImageFolder() = delete;
    ImageFolder(const Sequence&) = delete;
    ImageFolder& operator=(const ImageFolder&) = delete;
};

/**
 * @brief Video class manages videos
 *
 * @see Basic definition on page @ref BASICDEFS
 *
 * @note Error codes 321*
 * 
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
     * Construct video object for iterating through VTApi videos
     * If a specific name is set, object will represent one video only
     * @param commons base Commons object
     * @param name video name (no name = all videos)
     */
    Video(const Commons& orig, const std::string& name = std::string());

    /**
     * Construct video object for iterating through VTApi videos
     * Object will represent set of videos specified by their names
     * @param commons base Commons object
     * @param names list of sequences videos
     */
    Video(const Commons& commons, const std::list<std::string>& names);

    /**
     * Destructor
     */
    ~Video();

    /**
     * Moves to a next video, releases capture and sets sequence name and location varibles
     * @return success
     * @note Overloading next() from Sequence
     */
    bool next();
    
    /**
     * Opens a video capture (not necessary to call directly)
     * @return success
     */
    bool openVideo();
    
    /**
     * Closes a video capture (not necessary to call directly)
     */
    void closeVideo();

    /**
     * Gets OpenCV capture object of opened video
     * @return reference to capture object
     */
    cv::VideoCapture& getCapture();
    
    /**
     * Gets next frame from current capture
     * @return frame of the video
     */
    cv::Mat getNextFrame();
    
    /**
     * Gets video length in frames
     * @return video length in frames
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

private:
    cv::VideoCapture _capture;   /**< Video file capture */

    Video() = delete;
    Video(const Video&) = delete;
    Video& operator=(const Video&) = delete;
};


} // namespace vtapi
