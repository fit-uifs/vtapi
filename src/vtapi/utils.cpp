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
// #include <opencv2/imgproc/imgproc.hpp>
#include <vtapi/common/utils.h>
#include <iostream>

using namespace std;

namespace vtapi {
  vector<char> convert_mat2img(cv::Mat cv_img, string format) {
    vector<char> img_data;
    cv::imencode("." + format, cv_img, reinterpret_cast<vector<uchar> &>(img_data));

    return img_data;
  }

/*
  vector<char> convert_mat2base64(cv::Mat cv_img, string format) {
    stringstream ss;
    Poco::Base64Encoder b64enc(ss);

    vector<char> img_data = convert_mat2img(cv_img, format);
    b64enc << string(img_data.begin(), img_data.end());

    string str_result = "0123456789112345678921234567893123456789412345678951234567896123456789712345678981234567899123456789"; //ss.str();

    return vector<char>(str_result.begin(), str_result.end());
  }
  */

} // namespace vtapi
