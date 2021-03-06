﻿#ifndef CVGRAYIMAGE_H
#define CVGRAYIMAGE_H

#include "holmos_imglib_global.h"
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QRgb>
#include <string>


#ifdef DOUBLE_PRECISION
    typedef double floatp;
    static const int HOLMOS_IMGLIB_GRAY_MAT_TYPE = CV_64FC1;
#else
    typedef float floatp;
    static const int HOLMOS_IMGLIB_GRAY_MAT_TYPE = CV_32FC1;
#endif


class CvGrayImage
{

public:
    /**
     * @brief Create new CvGrayImage
     * @param height the height in pixels
     * @param width the width in pixels
     */
    CvGrayImage(int height_, int width_);

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

    /**
     * @brief normalize the image to a minimum of 0.0 and a max of 1.0
     */
    void normalize();

    /**
     * @brief Generate a CvGrayImage from a QImage.
     * @param img a grayscale image. if it is not grayscale the first channel will be extracted
     */
    static CvGrayImage fromQImage(QImage &img);

    int width() const;
    int height() const;

    floatp *getArr();

    cv::Mat *getMat();

    virtual ~CvGrayImage();
protected:
    int width_, height_;
    cv::Mat *mat;
};

#endif // CVGRAYIMAGE_H
