/* 
 * File:   main.cpp
 * Author: chmelarp
 *
 * Created on 28. září 2011, 23:52
 */

#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "commons.h"

using namespace std;
using namespace cv;

/*
 * 
 */
int main(int argc, char** argv) {

    Connector connector(CONNINFO, new Logger());
    connector.connected();

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


