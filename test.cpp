/* 
 * File:   main.cpp
 * Author: chmelarp
 *
 * Created on 28. září 2011, 23:52
 */

#include <cstdlib>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "commons.h"
#include "VTApi.h"

using namespace std;
using namespace cv;

/*
 * 
 */
int main(int argc, char** argv) {

    // OpenCV GUI
    // namedWindow("VTApi", 1);

    // FIXME:
    // The ANSI C++ specification (section 18.7.3) states that the first argument
    // in a variable argument lists cannot be a reference.
    // Doing so is undefined behaviour.

    // Logger* logger = new Logger();

    // PostreSQL connection
    // Connector* connector = new Connector(CONNINFO); // , logger
    // Commons* commons = new Commons(*connector);

    VTApi* vtapi = new VTApi(CONNINFO);

    Dataset* dataset = vtapi->newDataset();     // "public"
    if (!dataset->next()) vtapi->getLogger()->error("Well, there is no such parameter.");

    Sequence* sequence = dataset->newSequence();
    // while all media
    while (sequence->next()) {

        std::cout << sequence->getLocation();
/*
        // in case of images / frames... we can
        cv::Mat* mat = null;

        while (mat = sequence->getNextMat()) {
            imshow("VTApi", mat);
            if(waitKey(100) >= 0) break;
        }

        delete mat;
*/
    }

    delete sequence;
    delete dataset;
    delete vtapi;



/*
    VideoCapture cap("/home/chmelarp/Projects/VidTeAPI/video/Megamind.avi");
    if(!cap.isOpened()) return -1;

    Mat frame, edges;
    namedWindow("edges",1);
    for(;;)
    {
        cap >> frame;
        cvtColor(frame, edges, CV_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);
        imshow("edges", edges);
        if(waitKey(30) >= 0) break;
    }

 *
 *
 */    return 0;
}


