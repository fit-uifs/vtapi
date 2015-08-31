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

#include "keyvalues.h"
#include "dataset.h"
#include "interval.h"

namespace vtapi {

class Dataset;
class Interval;


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
class Sequence : protected KeyValues
{
public:
    /**
     * @brief Copy constructor
     * @param copy original object
     */
    Sequence(const Sequence& copy);

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
     * @param names vector of sequences names
     */
    Sequence(const Commons& commons, const std::vector<std::string>& names);
    
    /**
     * Destructor
     */
    virtual ~Sequence();
    
    using KeyValues::count;

    /**
     * Moves to a next sequence and sets sequence name and location varibles
     * @return success
     * @note Overloading next() from KeyValues
     */
    virtual bool next() override;

    /**
     * @brief Gets parent dataset object
     * @return dataset object (initialized)
     */
    Dataset *getParentDataset() const;

    /**
     * Gets a sequence name
     * @return string value with the name of the sequence
     */
    std::string getName() const;
    
    /**
     * Gets a sequence type
     * @return string value [video|images|data]
     */
    std::string getType() const;
    
    /**
     * Gets sequence location relative to dataset location
     * @return location of the current sequence
     */
    std::string getLocation() const;

    /**
     * @brief Gets sequence length (# of frames in video or images in folder)
     * @return size
     */
    unsigned int getLength() const;

    /**
     * Gets sequence comment
     * @return string value
     */
    std::string getComment() const;

    /**
     * Gets full path to sequence location
     * @return full path
     */
    std::string getDataLocation() const;
    
    /**
     * @brief Gets time when sequence was added to dataset
     * @return timestamp
     */
    std::chrono::system_clock::time_point getCreatedTime() const;

    /**
     * Sets video's comment
     * @param comment new comment
     * @return success
     */
    bool updateComment(const std::string& comment);

protected:
    virtual bool preUpdate() override;

private:
    Sequence() = delete;
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
     * @brief Copy constructor
     * @param copy original object
     */
    ImageFolder(const ImageFolder& copy);

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
     * @param names vector of image folders names
     */
    ImageFolder(const Commons& commons, const std::vector<std::string>& names);

    /**
     * Individual next() for image folder
     * @return success
     */
    bool next() override;

private:
    ImageFolder() = delete;
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
     * @brief Copy constructor
     * @param copy original object
     */
    Video(const Video& copy);

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
     * @param names vector of sequences videos
     */
    Video(const Commons& commons, const std::vector<std::string>& names);

    /**
     * Moves to a next video, releases capture and sets sequence name and location varibles
     * @return success
     */
    bool next() override;
    
    /**
     * Gets OpenCV capture object of opened video
     * @return reference to capture object
     */
    cv::VideoCapture openVideo() const;
    
    /**
     * Gets video FPS rate
     * @return FPS
     */
    double getFPS() const;

    /**
     * @brief Gets video speed (0-1)
     * @return  speed
     */
    double getSpeed() const;
    
    /**
     * Gets video real-world start time
     * @return start time
     */
    std::chrono::system_clock::time_point getRealStartTime() const;
    
    /**
     * Sets video real-world start time
     * @return success
     */
    bool updateRealStartTime(const std::chrono::system_clock::time_point &starttime);

private:
    Video() = delete;
    Video& operator=(const Video&) = delete;
};


} // namespace vtapi
