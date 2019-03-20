#ifndef CVCOMPLEXIMAGE_H
#define CVCOMPLEXIMAGE_H

#include <opencv2/opencv.hpp>
#include <complex>
#include <limits>
#include <cvimage.h>
#include <cmath>
#include <fftw3.h>

#ifdef DOUBLE_PRECISION
    static const int HOLMOS_IMGLIB_CPLX_MAT_TYPE = CV_64FC2;
#define FFT_PLAN fftw_plan
#else
    static const int HOLMOS_IMGLIB_CPLX_MAT_TYPE = CV_32FC2;
#define FFT_PLAN fftwf_plan
#endif

typedef std::complex<floatp> ComplexPixel;

class CvComplexImage
{
public:
    CvComplexImage(int height_, int width_);

    ComplexPixel getAt(int y, int x);

    void forEach(std::function<ComplexPixel (int, int)> functor);

    void initValue(ComplexPixel val);

    ComplexPixel *getArr();

    int width() const;
    int height() const;

    CvComplexImage &fft();
    CvComplexImage &ifft();
    void magnitudeSpectrum(CvGrayImage &img);

    virtual ~CvComplexImage();

protected:
    int width_, height_;
    int bufferSize;
    FFT_PLAN plan = nullptr;
    FFT_PLAN iplan = nullptr;
    ComplexPixel *buffer;
};

#endif // CVCOMPLEXIMAGE_H
