#include "cvgrayimage.h"


CvGrayImage::CvGrayImage(int height, int width) {
    this->width = width;
    this->height = height;

    mat = new cv::Mat(height, width, HOLMOS_IMGLIB_GRAY_MAT_TYPE);
}

floatp CvGrayImage::getAt(int y, int x) {
    return mat->at<floatp>(y, x);
}

cv::Mat *CvGrayImage::getMat() {
    return mat;
}

void CvGrayImage::forEach(std::function<floatp (int, int)> functor) {

    /* Capture the functor and execute it within the lambda expression */
    mat->forEach<floatp>([functor](floatp &pixel, const int *position) {
        pixel = functor(position[0], position[1]);
    });
}

void CvGrayImage::initValue(floatp val) {
    mat->forEach<floatp>([val] (floatp &pixel, const int *pos) {
        Q_UNUSED(pos)
        pixel = val;
    });
}


CvGrayImage::~CvGrayImage() {
    delete mat;
}
