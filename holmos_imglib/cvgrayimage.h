#ifndef CVGRAYIMAGE_H
#define CVGRAYIMAGE_H

#include "holmos_imglib_global.h"
#include <opencv2/opencv.hpp>

#ifdef DOUBLE_PRECISION
    typedef double floatp;
    static const int HOLMOS_IMGLIB_GRAY_MAT_TYPE = CV_64FC1;
#else
    typedef float floatp;
    static const int HOLMOS_IMGLIB_GRAY_MAT_TYPE = CV_32FC1;
#endif

class HOLMOS_IMGLIBSHARED_EXPORT CvGrayImage
{

public:
    /**
     * @brief Create new CvGrayImage
     * @param height the height in pixels
     * @param width the width in pixels
     */
    CvGrayImage(int height, int width);

    /**
     * @return the value at the given coordinates
     */
    floatp getAt(int y, int x);

    /**
     * @brief Run the lambda on each pixel and set its value accordingly
     * @param functor the lambda to run
     */
    void forEach(std::function<floatp (int, int)>functor);

    /**
     * @brief initialize the image with a given value
     */
    void initValue(floatp val);

    cv::Mat *getMat();

    virtual ~CvGrayImage();
protected:
    int width, height;
    cv::Mat *mat;
};

#endif // CVGRAYIMAGE_H
