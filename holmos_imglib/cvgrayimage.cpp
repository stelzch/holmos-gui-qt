#include "cvgrayimage.h"


CvGrayImage::CvGrayImage(int height, int width) {
    this->width = width;
    this->height = height;

    mat = new cv::Mat(height, width, HOLMOS_IMGLIB_GRAY_MAT_TYPE);
}


CvGrayImage::~CvGrayImage() {
    delete mat;
}
