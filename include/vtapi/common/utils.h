/**
 * @file
 * @brief   Useful tools
 *
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 *
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

// #include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <Poco/Base64Encoder.h>


using namespace std;

namespace vtapi {
  vector<char> convert_mat2img(cv::Mat cv_img, string format = "jpg");
//  vector<char> convert_mat2base64(cv::Mat cv_img, string format = "jpg");
} // namespace vtapi
