#include "cvgrayimage.h"


template<typename T>
CvImage<T>::CvImage(int height, int width) {
    this->width = width;
    this->height = height;

    mat = new cv::Mat(height, width, HOLMOS_IMGLIB_GRAY_MAT_TYPE);
}

template<typename T>
T CvImage<T>::getAt(int y, int x) {
    return mat->at<T>(y, x);
}

template<typename T>
cv::Mat *CvImage<T>::getMat() {
    return mat;
}

template<typename T>
void CvImage<T>::forEach(std::function<floatp (int, int)> functor) {
    /* Capture the functor and execute it within the lambda expression */
    mat->forEach<T>([functor](T &pixel, const int *position) {
        pixel = functor(position[0], position[1]);
    });
}

template<typename T>
void CvImage<T>::initValue(T val) {
    mat->forEach<floatp>([val] (floatp &pixel, const int *pos) {
        Q_UNUSED(pos) pixel = val;
    });
}

template<typename T>
CvImage<T> CvImage<T>::fromQImage(QImage &img) {
    CvGrayImage retVal(img.height(), img.width());
    retVal.forEach([img] (int y, int x) {
        QRgb colorValue = img.pixel(x, y);

        return qRed(colorValue);
    });

    return retVal;
}

template<typename T>
void CvImage<T>::normalize() {
    cv::normalize(*mat, *mat, 0.0, 1.0, cv::NormTypes::NORM_MINMAX);
}

template<typename T>
T *CvImage<T>::getArr() {
    return mat->ptr<T>(0);
}


template<typename T>
CvImage<T>::~CvImage() {
    delete mat;
}
