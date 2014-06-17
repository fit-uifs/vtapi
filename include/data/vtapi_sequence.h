/* 
 * File:   vtapi_sequence.h
 * Author: vojca
 *
 * Created on May 7, 2013, 12:57 PM
 */

#ifndef VTAPI_SEQUENCE_H
#define	VTAPI_SEQUENCE_H

namespace vtapi {
    class Sequence;
    class Video;
}

#include "vtapi_keyvalues.h"
#include "vtapi_interval.h"
#include "vtapi_process.h"
#include <opencv2/opencv.hpp>

namespace vtapi {


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
    string file_name_video; // < File name of a video
    string file_name_image; // < File name of an image */
//Methods
public:
    /**
     * Constructor for sequences
     * @param orig pointer to the parrent KeyValues object
     * @param name specific sequence name
     */
    Sequence(const KeyValues& orig, const string& name = "");

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
    bool add(const string& name, const string& location, const string& type);

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
    bool add(const string& name, const string& location, const string& type,
    const string& userid, const string& groupid, const string& notes);

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
    string getName();
    
    /**
     * Get a sequence location
     * @return string value with the location of the sequence
     */
    string getLocation();
    
    
    /**
     * Get a sequence type
     * @return string value [video|images|data]
     */
    string getType();
    
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
    Image* newImage(const string& name = "");

    /**
     *
     * @param name
     * @return
     * @todo @b code: neimplementováno (potom doplnit i doc)
     */
    Process* newProcess(const string& name = "");

#ifdef __OPENCV_CORE_HPP__
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
    Video(const KeyValues& orig, const string& name);

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
    bool add(string name, string location);


#ifdef __OPENCV_CORE_HPP__
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

#ifdef __OPENCV_HIGHGUI_HPP__
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
    string videoOutput;
    //

    vector<Image> images;
    vector<Video> videos;
    vector<Interval> intervals;

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

    bool setPlayerOutput(string filename);
    string getPlayerOutput();

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

