/**
 * @file
 * @brief   Declaration of Sequence, Video and VideoPlayer classes
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref Licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */


#ifndef VTAPI_SEQUENCE_H
#define	VTAPI_SEQUENCE_H

#include "vtapi_keyvalues.h"
#include "vtapi_interval.h"

namespace vtapi {

/**
 * @brief A Sequence class manages videos and images
 *
 * @see Basic definition on page @ref LOGICAL
 *
 * @note Error codes 32*
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref Licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Sequence : public KeyValues {
/* Memebers
protected:
    std::string file_name_video; // < File name of a video
    std::string file_name_image; // < File name of an image */
//Methods
public:
    /**
     * Constructor for sequences
     * @param orig   pointer to the parrent KeyValues object
     * @param name   specific sequence name
     */
    Sequence(const KeyValues& orig, const std::string& name = "");

    /**
     * Moves to a next sequence and set sequence name and location varibles
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next();

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
     * @param groupid    name of an owner group
     * @param notes      optional description
     * @return success
     */
    bool add(const std::string& name, const std::string& location, const std::string& type,
    const std::string& userid, const std::string& groupid, const std::string& notes);

    /**
     * Executes SQL INSERT command for sequence
     * @return success
     * @note Overloading addExecute from KeyValues
     */
    bool addExecute();
    /**
     * Resets sequence to initial state
     * @return success
     */
    bool preSet();
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
     * Creates a new interval specified by a start time and an end time
     * @param t1 start time
     * @param t2 end time
     * @return pointer to the new interval
     * @todo @b doc/code: parameters t1 and t2 are unused!
     */
    Interval* newInterval(const int t1 = -1, const int t2 = -1);

    /**
     * Creates a new image specified by a name
     * @param name   name of the image
     * @return pointer to the new image
     */
    Image* newImage(const std::string& name = "");

#if HAVE_OPENCV
    Image* imageBuffer;   /**< Buffer of images metadata at a same location */
    cv::Mat frame;        /**< Video frame or image data */
    
    /**
     * This is to get the data from the sequence - either video frames or images. 
     * In the second case, the imageBuffer is used and you are not supposed to call next (as well as in the case of video).
     * @return cv::Mat
     */
    cv::Mat getData();
#endif

};


/**
 * @brief Video class manages videos
 *
 * @see Basic definition on page @ref LOGICAL
 *
 * @note Error codes 321*
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref Licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class Video : public Sequence {
public:
    /**
     * Constructor for Video
     * @param orig   pointer to the parrent KeyValues object
     * @param name   specific sequence name
     */
    Video(const KeyValues& orig, const std::string& name);

    /**
     * Creates a new frame specified by a frame number
     * @param frame   ordinal number of the image
     * @return pointer to the new image
     * @unimplemented
     */
    Image* newFrame(const int frame = 1);

    /**
     * Adds a new video to the dataset
     * @param name       name of the video
     * @param location   location of the video
     * @return success
     */
    bool add(std::string name, std::string location);


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
     * @return 
     */
    void closeVideo();

    /**
     * Gets next frame from current capture
     * @return frame of the video
     */
    cv::Mat getData();
    
    /**
     * Gets video length in frames
     * @return 
     */
    size_t getLength();
#endif

};

#if HAVE_OPENCV
/**
 * @brief This is the ever-simplest video player...
 *
 * VideoPlayer makes copies of each object, so it doesn't affect nexts() performed elsewhere,
 * however, it may fail in case of next, where are hundreds of thousands of tuples (KeyValues)
 * 
 * @warning This behavior might be changed later.
 *
 * @note Error codes 16*
 *
 * @todo @b doc: třída prakticky bez dokumentace
 * @todo @b code: neimplementováno
 * 
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref Licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */
class VideoPlayer : public Commons {
protected:
    std::string videoOutput;
    //

    std::vector<Image> images;
    std::vector<Video> videos;
    std::vector<Interval> intervals;

public:
    /**
     * A void constructor - plays nothing at all at the moment
     * @param orig
     */
    VideoPlayer(Commons& orig);
    /**
     * @unimplemented
     */
    VideoPlayer(Image& image);
    /**
     * A void constructor - plays nothing at all at the moment
     * @param video
     */
    VideoPlayer(Video& video);
    /**
     * @unimplemented
     */
    VideoPlayer(Interval& interval);

    /**
     * @unimplemented
     */
    bool playerAdd(Image& image);    
    /**
     * @unimplemented
     */
    bool playerAdd(Video& video);
    /**
     * @unimplemented
     */
    bool playerAdd(Interval& interval);

    /**
     * @unimplemented
     */
    bool setPlayerOutput(std::string filename);
    /**
     * @unimplemented
     */
    std::string getPlayerOutput();

    /**
     * This function simply plays what added before
     * ... or a default capture in case of none (can be used to store the capture)
     * @return success
     */
    bool play();
};

#endif

} // namespace vtapi

#endif	/* VTAPI_SEQUENCE_H */

