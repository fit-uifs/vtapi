/*
 * File:   Video.cpp
 * Author: chmelarp
 *
 * Created on 1. 10. 2011
 */

#include "vtapi.h"

Video::Video(const KeyValues& orig, const String& name) : Sequence(orig, name) {
    thisClass = "Video";

    select->whereString("seqtyp", "video");
}


String Video::getDataLocation() {
    if (baseLocation.empty()) warning(3216, "No (base) location specified");
    if (datasetLocation.empty()) warning(3216, "No (dataset) location specified");
    if (sequenceLocation.empty()) warning(3216, "No sequence location specified");

    return (baseLocation + datasetLocation + sequenceLocation);
}


bool Video::add(String name, String location) {
    destruct(insert);

    insert = new Insert(*this, "sequences");
    insert->keyString("seqname", name);
    insert->keyString("seqlocation", location);
    // FIXME: Vojta, P3k ... tohle by jednou mohlo jet
    // insert->keyEnum("seqtyp", "video");

#ifdef __OPENCV_CORE_HPP__
    // TODO: P3k check something else???
    String fn = baseLocation + datasetLocation + location;
    if (!fileExists(fn)) warning(3210, "Cannot open file " + fn);
    // TODO: check the lenght and so on...
#endif
}



#ifdef __OPENCV_HIGHGUI_HPP__

VideoPlayer::VideoPlayer(Commons& orig) : Commons(orig) {
    thisClass = "VideoPlayer(Commons&)";
}

VideoPlayer::VideoPlayer(Video& orig) : Commons(orig) {
    thisClass = "VideoPlayer(Video&)";

    videos.push_back(orig);
}


// TODO: play 1, 2, 4, 6, 9, 12 and 16 videos at once :)
bool VideoPlayer::play() {
    // TODO: P3k dodelat nacteni vice Video a Interval
    cv::VideoCapture cap;
    int fps = 25;

    if (!videos.empty()) {
        cap = cv::VideoCapture(videos.front().getDataLocation());
        int fps = (int) cap.get(CV_CAP_PROP_FPS);
    }
    // TODO: images, ...
    // frame = cv::imread("img.jpg");
    // if there is no file to play, use the default video capture device (camera)
    else {
        cap = cv::VideoCapture(0);
    }

    // toz a jedem
    if(cap.isOpened()) {
        cv::namedWindow("video",1);
        cv::Mat frame;
        while(true)
        {
            cap >> frame;
            cv::imshow("video", frame);
            if(cv::waitKey(1000 / fps) >= 0) break; // correct the with real timer!
        }
    }
    else {
        warning(161, "Sorry, there is nothing to play, aborting :(");
    }
}

#endif