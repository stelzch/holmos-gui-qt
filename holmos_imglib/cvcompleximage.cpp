#include "cvcompleximage.h"
#include <QDebug>


CvComplexImage::CvComplexImage(int height, int width)
{
    this->width_ = width;
    this->height_ = height;

    assert(height * width < std::numeric_limits<int>::max());
    bufferSize = width * height;
    buffer = new ComplexPixel[
            static_cast<unsigned>(bufferSize)];
}

ComplexPixel CvComplexImage::getAt(int y, int x) {
    return buffer[y * height_ + x];
}

void CvComplexImage::forEach(std::function<ComplexPixel (int, int)> functor) {
    for (int i = 0; i < bufferSize; i++) {
        int y = i / width_;
        int x = i % width_;

        buffer[y * height_ + x] = functor(y, x);
    }
}

void CvComplexImage::initValue(ComplexPixel val) {
    forEach([val] (int y, int x) {
        return val;
    });
}

int CvComplexImage::width() const {
    return width_;
}

int CvComplexImage::height() const {
    return height_;
}


ComplexPixel *CvComplexImage::getArr() {
    return buffer;
}

CvComplexImage &CvComplexImage::fft() {
#ifdef DOUBLE_PRECISION
    if (plan == nullptr) {
        fftw_complex *bufferPtr = reinterpret_cast<fftw_complex*>(buffer);
        plan = fftw_plan_dft_2d(height, width,
                                bufferPtr, bufferPtr,
                                FFTW_FORWARD, FFTW_MEASURE);
    }
    fftw_execute(plan);
#else
    if (plan == nullptr) {
        fftwf_complex *bufferPtr = reinterpret_cast<fftwf_complex*>(buffer);
        plan = fftwf_plan_dft_2d(height_, width_,
                                bufferPtr, bufferPtr,
                                FFTW_FORWARD, FFTW_MEASURE);
    }
    fftwf_execute(plan);
#endif

    return *this;
}

CvComplexImage &CvComplexImage::ifft() {
#ifdef DOUBLE_PRECISION
    if (iplan == nullptr) {
        fftw_complex *bufferPtr = reinterpret_cast<fftw_complex*>(buffer);
        iplan = fftw_plan_dft_2d(height, width,
                                bufferPtr, bufferPtr,
                                FFTW_BACKWARD, FFTW_MEASURE);
    }
    fftw_execute(iplan);
#else
    if (iplan == nullptr) {
        fftwf_complex *bufferPtr = reinterpret_cast<fftwf_complex*>(buffer);
        iplan = fftwf_plan_dft_2d(height_, width_,
                                bufferPtr, bufferPtr,
                                FFTW_BACKWARD, FFTW_MEASURE);
    }
    fftwf_execute(iplan);
#endif

    return *this;
}

void CvComplexImage::magnitudeSpectrum(CvGrayImage &img) {
    assert(img.width() == width());
    assert(img.height() == height());

    img.forEach([this] (int y, int x) {
        return abs(this->buffer[y * width_ + x]);
    });
}


CvComplexImage::~CvComplexImage() {
    delete[] buffer;
}
