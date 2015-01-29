/* 
 * File:   vtapi_sequence.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:57 PM
 */

#ifndef VTAPI_SEQUENCE_H
#define	VTAPI_SEQUENCE_H

#include "vtapi_keyvalues.h"
#include "vtapi_interval.h"
#include "vtapi_process.h"


namespace vtapi {
    
class Interval;
class Image;
class Process;

/**
 * @brief A Sequence class manages videos and images
 *
* @see Basic definition on page @ref LOGICAL
 *
 * @note Error codes 32*
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
     * @param orig pointer to the parrent KeyValues object
     * @param name specific sequence name
     */
    Sequence(const KeyValues& orig, const std::string& name = "");

    /**
     * Move to a next sequence and set sequence name and location varibles
     * @return success
     * @note Overloading next() from KeyValues
     */
    bool next();

    /**
     * Add new sequence to a database table - minimal insert
     * @param name name of the sequence
     * @param location location of the sequence
     * @param type type of the sequence
     * @return success
     */
    bool add(const std::string& name, const std::string& location, const std::string& type);

    /**
     * Add new sequence to a database table - full insert
     * @param name name of the sequence
     * @param location location of the sequence
     * @param type type of the sequence
     * @param userid name of the owner
     * @param groupid name of the owner group
     * @param notes optional description
     * @return success
     */
    bool add(const std::string& name, const std::string& location, const std::string& type,
    const std::string& userid, const std::string& groupid, const std::string& notes);

    /**
     * Execute SQL INSERT command for sequence
     * @return success
     * @note Overloading addExecute from KeyValues
     */
    bool addExecute();
    /**
     * Reset sequence to initial state
     * @return success
     */
    bool preSet();
    /**
     * Get a sequence name
     * @return string value with the name of the sequence
     */
    std::string getName();
    
    /**
     * Get a sequence location
     * @return string value with the location of the sequence
     */
    std::string getLocation();
    
    
    /**
     * Get a sequence type
     * @return string value [video|images|data]
     */
    std::string getType();
    
    /**
     * Create a new interval specified by a start time and an end time
     * @param t1 start time
     * @param t2 end time
     * @return pointer to the new interval
     */
    Interval* newInterval(const int t1 = -1, const int t2 = -1);

    /**
     * Create a new image specified by a name
     * @param name name of the image
     * @return pointer to the new image
     */
    Image* newImage(const std::string& name = "");

    /**
     *
     * @param name
     * @return
     * @todo @b code: neimplementováno (potom doplnit i doc)
     */
    Process* newProcess(const std::string& name = "");

#ifdef HAVE_OPENCV
    Image* imageBuffer;
    cv::Mat frame;
    
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
 */
class Video : public Sequence {
public:
    /**
     * Constructor for Video
     * @param orig pointer to the parrent KeyValues object
     * @param name specific sequence name
     */
    Video(const KeyValues& orig, const std::string& name);

    /**
     * Create a new frame specified by the frame number
     * @param frame number of the image
     * @return pointer to the new image
     */
    Image* newFrame(const int frame = 1);

    /**
     * Add new video to the dataset
     * @param name of the video
     * @param location of the video
     * @return success
     */
    bool add(std::string name, std::string location);


#ifdef HAVE_OPENCV
    cv::VideoCapture capture;
    
    /**
     * Opens a video capture (not necessary to call directly)
     * @todo Test
     * @return success
     */
    bool openVideo();

    /**
     *
     * @return
     * @todo @b code: neimplementováno (potom doplnit i doc)
     */
    cv::Mat getData();
#endif

};

#ifdef HAVE_OPENCV
/**
 * @brief This is the ever-simplest video player...
 *
 * VideoPlayer makes copies of each object, so it doesn't affect nexts() performed elsewhere,
 * however, it may fail in case of next, where are hundreds of thousands of tuples (@see Keyvalues)
 * // TODO: This behavior might be changed later
 *
 *  @note Error codes 16*
 *
 *  @todo @b doc: třída prakticky bez dokumentace
 *  @todo @b code: neimplementováno
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
    VideoPlayer(Image& image);
    VideoPlayer(Video& video);
    VideoPlayer(Interval& interval);

    bool playerAdd(Image& image);
    bool playerAdd(Video& video);
    bool playerAdd(Interval& interval);

    bool setPlayerOutput(std::string filename);
    std::string getPlayerOutput();

    /**
     * This function simply plays what added before
     * ... or a default capture in case of none (can be used to store the capture)
     * @return
     * @todo @b code: bez návratové hodnoty (potom doplnit i do doc)
     */
    bool play();
};

#endif

} // namespace vtapi

#endif	/* VTAPI_SEQUENCE_H */

