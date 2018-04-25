#include "computationworker.h"
#include "mimage.h"
#include "mcompleximage.h"
#include "fouriertransformer.h"
#include <opencv2/opencv.hpp>
#include <cassert>

using namespace cv;
using namespace std;

typedef Point2f ComplexPixel;

ComputationWorker::ComputationWorker(QObject *parent, int n0, int n1) : QObject(parent), shouldStop(false), n0(n0), n1(n1), shouldUnwrapPhase(false),
    rectX(0), rectY(0), rectR(0)
{

}

void ComputationWorker::fftshift(Mat img) {
        int hx = img.cols / 2;
        int hy = img.rows / 2;
        Mat q1(img, Rect(hx, 0, hx, hy));
        Mat q2(img, Rect(0, 0, hx, hy));
        Mat q3(img, Rect(0, hy, hx, hy));
        Mat q4(img, Rect(hx, hy, hx, hy));
        Mat tmp;

        q1.copyTo(tmp);
        q3.copyTo(q1);
        tmp.copyTo(q3);

        q2.copyTo(tmp);
        q4.copyTo(q2);
        tmp.copyTo(q4);

}
QImage ComputationWorker::asQImage(Mat img) {
    assert(img.type() == CV_32FC1);
    QImage image(img.cols, img.rows, QImage::Format_RGB888);

    img.forEach<float>([&image, &img](float &p, const int position[]) -> void {
            image.bits()[3*(img.cols * position[0] + position[1])+0] = floor(p*255.0);
            image.bits()[3*(img.cols * position[0] + position[1])+1] = floor(p*255.0);
            image.bits()[3*(img.cols * position[0] + position[1])+2] = floor(p*255.0);
    });

    return image;
}
void ComputationWorker::multiplyComplex(cv::Mat& input, cv::Mat& factor, cv::Mat& out) {
    assert(input.type() == CV_32FC2);
    assert(out.type() == CV_32FC2);
    assert(factor.type() == CV_32FC2);

    out.forEach<ComplexPixel>([&input, &factor](ComplexPixel &p, const int pos[]) -> void {
        ComplexPixel i = input.at<ComplexPixel>(pos[0], pos[1]);
        ComplexPixel f = factor.at<ComplexPixel>(pos[0], pos[1]);
        p.x = i.x*f.x - i.y*f.y;
        p.y = i.x*f.y + i.x*f.x;
    });
}

void ComputationWorker::multiplyReal(cv::Mat& input, cv::Mat& factor, cv::Mat& out) {
    assert(input.type() == CV_32FC2);
    assert(out.type() == CV_32FC2);
    assert(factor.type() == CV_32FC1);

    out.forEach<ComplexPixel>([&input, &factor](ComplexPixel &p, const int pos[]) -> void {
        float fac = factor.at<float>(pos[0], pos[1]);
        ComplexPixel inp = input.at<ComplexPixel>(pos[0], pos[1]);
        p.x = inp.x * fac;
        p.y = inp.y * fac;
    });    
}

void ComputationWorker::divideReal(cv::Mat& input, cv::Mat& factor, cv::Mat& out) {
    assert(input.type() == CV_32FC2);
    assert(out.type() == CV_32FC2);
    assert(factor.type() == CV_32FC1);

    out.forEach<ComplexPixel>([&input, &factor](ComplexPixel &p, const int pos[]) -> void {
        float fac = factor.at<float>(pos[0], pos[1]);
        ComplexPixel inp = input.at<ComplexPixel>(pos[0], pos[1]);
        p.x = inp.x / fac;
        p.y = inp.y / fac;
    });    
}

void ComputationWorker::divideComplex(cv::Mat& input, cv::Mat& factor, cv::Mat& out) {
    assert(input.type() == CV_32FC2);
    assert(out.type() == CV_32FC2);
    assert(factor.type() == CV_32FC2);

    out.forEach<ComplexPixel>([&input, &factor](ComplexPixel &p, const int pos[]) -> void {
        ComplexPixel i = input.at<ComplexPixel>(pos[0], pos[1]);
        ComplexPixel f = factor.at<ComplexPixel>(pos[0], pos[1]);
        p.x = (i.x*f.x - i.y) / (f.x*f.x+f.y*f.y);
        p.y = (-i.x*f.y + i.y*f.y) / (f.x*f.x+f.y*f.y);
    });
}

void ComputationWorker::saveMat(QString filename, Mat& img) {
    
}

void ComputationWorker::doWork() {
    Mat image = imread("holmos_raw.png", 0);
    assert(image.type() == CV_8UC1);
    assert(image.rows == n0);
    assert(image.cols == n1);

    /* Create r2s array */
    Mat r2s_real(n0*2, n1*2, CV_32FC1);
    std::vector<double> x2;
    std::vector<double> y2;
    for(int x=-n1; x<n1; x++)
        x2.push_back(pow(x, 2));
    for(int y=-n0; y<n0; y++)
        y2.push_back(pow(y, 2));

    std::rotate(x2.begin(), x2.begin()+n1, x2.end());
    std::rotate(y2.begin(), y2.begin()+n0, y2.end());

    assert(x2.size() == 2*n1);
    assert(y2.size() == 2*n0);
    for(int y=0; y<n0*2; y++) {
        for(int x=0; x<n1*2; x++) {
            r2s_real.at<float>(y, x) = x2.at(x) + y2.at(y) + 1e-10;
            if(x2.at(x) + y2.at(y) +1e-10 < 1e-10) {
                qDebug() << "ohoh: " << x << y << x2.at(x) + y2.at(y);
            }
        }
    }
    Mat r2splanes[] = {r2s_real, Mat::zeros(n0*2, n1*2, CV_32FC1)};
    Mat r2s;
    merge(r2splanes, 2, r2s);

    Mat img(n0, n1, CV_32FC1);        
    Mat fourierTransform(n0, n1, CV_32FC2);
    Mat planes[] = {img, Mat::zeros(n0, n1, CV_32FC1)};
    Mat magnitudeSpectrum(n0, n1, CV_32FC1);
    Mat cropped = Mat::zeros(n0, n1, CV_32FC2);
    Mat phaseAngle(n0, n1, CV_32FC1);
    Mat phaseAngleNorm(n0, n1, CV_32FC1);
    Mat mirror = Mat::zeros(n0*2, n1*2, CV_32FC2);
    Mat holo_sin_real(n0*2, n1*2, CV_32FC1);
    Mat holo_cos_real(n0*2, n1*2, CV_32FC1);
    Mat holo_sin(n0*2, n1*2, CV_32FC2), holo_cos(n0*2, n1*2, CV_32FC2);
    Mat phi_prime(n0, n1, CV_32FC1);
    Mat unwrapped_phase(n0, n1, CV_32FC1);
    Mat buffer1(2*n0, 2*n1, CV_32FC2), buffer2(2*n0, 2*n1, CV_32FC2);

    
    fftwf_plan fft1, fft2;
    fftwf_complex *fft_buffer = (fftwf_complex *) fftwf_malloc(n0*2*n1*2*sizeof(fftwf_complex));
    fft1 = fftwf_plan_dft_2d(n0, n1, fft_buffer, fft_buffer, FFTW_FORWARD, FFTW_ESTIMATE);
    fft2 = fftwf_plan_dft_r2c_2d(n0, n1, img.ptr<float>(),
        reinterpret_cast<fftwf_complex*>(fourierTransform.ptr<ComplexPixel>()),
        FFTW_ESTIMATE);

    while(shouldStop == false) {

        /* Load image and convert to floating point */
        img.forEach<float>([&image](float &p, const int position[]) -> void {
                p = (float) image.at<unsigned char>(position[0], position[1]) / 255.0;
        });
        emit cameraImageReady(asQImage(img));
        qDebug() << img.at<float>(0) << img.at<float>(1023) << img.at<float>(n1);

        dft(img, fourierTransform, DFT_COMPLEX_OUTPUT);


        fftshift(fourierTransform);                
        split(fourierTransform, planes);
        cv::magnitude(planes[0], planes[1], magnitudeSpectrum);
        magnitudeSpectrum += Scalar::all(1);
        log(magnitudeSpectrum, magnitudeSpectrum);
        normalize(magnitudeSpectrum, magnitudeSpectrum, 0, 1, CV_MINMAX);

        emit magnitudeSpectrumReady(asQImage(magnitudeSpectrum));

        cropped.forEach<ComplexPixel>([](ComplexPixel &p, const int pos[]) -> void {
            p.x = 0.0;
            p.y = 0.0;
        });
        Rect cropRect(rectX-rectR, rectY-rectR, rectR*2, rectR*2);
        Rect destRect(n1/2-rectR, n0/2-rectR, rectR*2, rectR*2);
        Mat satellite(fourierTransform, cropRect);
        satellite.copyTo(cropped(destRect));
        
        fftshift(cropped);
        dft(cropped, cropped, DFT_INVERSE);
        
        qDebug() << cropped.cols << cropped.rows;
        phaseAngle.forEach<float>([&cropped](float&p, const int pos[]) -> void {
                assert(pos[0] < 1024);
                assert(pos[1] < 1024);
                //p = (float) cropped.at<float>(0, 1020, 1023);
                //p = (float) cropped.at<float>(0, pos[0], pos[1]);
                ComplexPixel cr = cropped.at<ComplexPixel>(pos[0], pos[1]);
                p = float(atan2(cr.y, cr.x));
        });
        qDebug() << phaseAngle.at<float>(103) << phaseAngle.at<float>(1024);
        normalize(phaseAngle, phaseAngleNorm, 0, 1.0, NORM_MINMAX);

        if(!shouldUnwrapPhase) {
            emit phaseAngleReady(asQImage(phaseAngleNorm));
        } else {
            for(int y=0; y<n0; y++) {
                for(int x=0; x<n1; x++) {
                    float val = phaseAngle.at<float>(y, x);

                    mirror.at<ComplexPixel>(y, x).x = val;
                    mirror.at<ComplexPixel>(y, 2*n1 - x - 1).x = val;
                    mirror.at<ComplexPixel>(2*n0 - y - 1, x).x = val;
                    mirror.at<ComplexPixel>(2*n0 - y - 1, 2*n1 - x - 1).x = val;
                }
            }
            holo_sin_real.forEach<float>([&mirror](float &p, const int pos[]) -> void {
                    p = sin(mirror.at<ComplexPixel>(pos[0], pos[1]).x);
            });
            holo_cos_real.forEach<float>([&mirror](float &p, const int pos[]) -> void {
                    p = cos(mirror.at<ComplexPixel>(pos[0], pos[1]).x);
            });
            holo_sin.forEach<ComplexPixel>([&mirror](ComplexPixel &p, const int pos[]) -> void {
                    p.x = sin(mirror.at<ComplexPixel>(pos[0], pos[1]).x);
                    p.y = 0;
            });
            holo_cos.forEach<ComplexPixel>([&mirror, &fft_buffer](ComplexPixel &p, const int pos[]) -> void {
                    p.x = cos(mirror.at<ComplexPixel>(pos[0], pos[1]).x);
                    /*fft_buffer[pos[0]*mirror.cols*2+pos[0]][0] = p.x;
                    fft_buffer[pos[0]*mirror.cols*2+pos[0]][1] = 0.0;*/
                    p.y = 0;
            });

            dft(holo_sin_real, holo_sin, DFT_COMPLEX_OUTPUT);
            multiplyReal(holo_sin, r2s_real, holo_sin);
            dft(holo_sin, holo_sin, DFT_INVERSE);
            multiplyReal(holo_sin, holo_cos_real, holo_sin);




            dft(holo_cos_real, holo_cos, DFT_COMPLEX_OUTPUT);
            multiplyReal(holo_cos, r2s_real, holo_cos);
            dft(holo_cos, holo_cos, DFT_INVERSE);
            multiplyReal(holo_cos, holo_sin_real, holo_cos);
            qDebug() << "Cosine multiplied";

            qDebug() << holo_sin.at<ComplexPixel>(20, 20).x;
            double min, max;
            cv::minMaxLoc(holo_sin, &min, &max);
            qDebug() << "Max: " << max << "Min:" << min;
            qDebug() << "Final pass";
            buffer1 = holo_sin - holo_cos;
            dft(buffer1, buffer1);
            divideReal(buffer1, r2s_real, buffer1);
            dft(buffer1, buffer1, DFT_INVERSE);

            phi_prime.forEach<float>([&buffer1](float &p, const int pos[]) -> void {
                    p = buffer1.at<ComplexPixel>(pos[0], pos[1]).x;
            }); 

            unwrapped_phase.forEach<float>([&phi_prime, &phaseAngle](float &p, const int pos[]) -> void {
                    float phase = phaseAngle.at<float>(pos[0], pos[1]);
                    float phi = phi_prime.at<float>(pos[0], pos[1]);

                    p = (phase + 2*M_PI*round((phi-phase) / 2 / M_PI));
            });
            normalize(unwrapped_phase, unwrapped_phase, 0, 1, CV_MINMAX);
            emit phaseAngleReady(asQImage(unwrapped_phase));
        }
    }

    /*cv::Mat image = cv::imread("holmos_raw.png", 0);
    assert(image.rows == n0);
    assert(image.cols == n1);*/


  /*
    MGrayImage img = MGrayImage::loadFromFile("/home/cstelz/mtestimage.png");
    MComplexImage cimg = MComplexImage::fromGrayImage(img);
    MComplexImage cropped(n0, n1);
    MComplexImage mirrored(n0*2, n1*2);
    MComplexImage holo_cos(mirrored), holo_sin(mirrored), buf1(mirrored), buf2(mirrored);
    MGrayImage r2s(n0*2, n1*2);
    r2s.initValue(1e-9);
    qDebug() << cimg.getWidth() << "x" << cimg.getHeight();

     Create r2s array 
    std::vector<double> x2;
    std::vector<double> y2;
    for(int x=-n1; x<n1; x++)
        x2.push_back(pow(x, 2));
    for(int y=-n0; y<n0; y++)
        y2.push_back(pow(y, 2));

    std::rotate(x2.begin(), x2.begin()+n1, x2.end());
    std::rotate(y2.begin(), y2.begin()+n0, y2.end());

    assert(x2.size() == 2*n1);
    assert(y2.size() == 2*n0);
    for(int y=0; y<n0*2; y++) {
        for(int x=0; x<n1*2; x++) {
            r2s.getData()[y*(2*n1)+x] = x2.at(x) + y2.at(y) + 1e-10;
            if(x2.at(x) + y2.at(y) +1e-10 < 1e-10) {
                qDebug() << "ohoh: " << x << y << x2.at(x) + y2.at(y);
            }
        }
    }
    double minval = 2.0, xcoord, ycoord;
    for(unsigned int i=0; i<r2s.getWidth()*r2s.getHeight(); i++) {
        if(r2s.getAt(i) < minval) {
            minval=r2s.getAt(i);
            xcoord=i%r2s.getWidth(); ycoord=floor(i/r2s.getHeight());
        }
    }
    qDebug() << "Minval r2s: " << r2s.getMin() << xcoord << ycoord;
    qDebug() << r2s.getAt(0);


    FourierTransformer ft;
    ft.planFFTFor(cimg, true);
    ft.planFFTFor(cropped, false);

    ft.planFFTFor(mirrored, true);
    ft.planFFTFor(mirrored, false);


    qDebug() << img.getWidth() << "x" << img.getHeight();
    auto i = 0;
    while(!shouldStop) {
        qDebug() << "FPS: " << fps.frame();

        emit cameraImageReady(img.asQImage());
        MComplexImage cimg2 = MComplexImage::fromGrayImage(img);



        ft.executeFFT(cimg2, true);
        cimg2.fftshift();

        auto magspec = cimg2.getMagnitudeSpectrum();
        //magspec.normalize();
        magspec /= 500.0;
        qDebug() << "Max: " << magspec.getMax();

        emit magnitudeSpectrumReady(magspec.asQImage());

        qDebug() << rectX << rectY;
        auto satellite = cimg2.getSector(rectX-rectR, rectY-rectR, rectR*2);
        cropped.initValue(std::complex<double>(0.0, 0.0));
        cropped.setSector((n1 / 2)-rectR, (n0 / 2)-rectR, satellite);

        cropped.fftshift();
        ft.executeFFT(cropped, false);

        auto phaseAngle = cropped.getAngle();
        qDebug() << r2s.getMin() << r2s.getMax() << " r2s";


        if(this->shouldUnwrapPhase) {
            for(unsigned int y=0; y<n0; y++) {
                for(unsigned int x=0; x<n1; x++) {
                    double val = phaseAngle.getAt(y*n1+x);

                    mirrored.getData()[y*2*n1+x] = val;
                    mirrored.getData()[y*2*n1+(2*n1-x)] = val;
                    mirrored.getData()[(2*n0-y)*2*n1+x] = val;
                    mirrored.getData()[(2*n0-y)*2*n1+(2*n1-x)] = val;
                }
            }

            holo_sin.loadGrayImage(mirrored.getReal().sine());
            holo_cos.loadGrayImage(mirrored.getReal().cosine());

            qDebug() << "Holosin [0,0] = " << holo_sin.getAt(0).real();
            qDebug() << "Holocos [0,0] = " << holo_cos.getAt(0).real();

            ft.executeFFT(holo_sin, buf1, true);
            ft.executeFFT(holo_cos, buf2, true);

            qDebug() << "buf1 [0,0] = " << buf1.getAt(0).real();
            qDebug() << "buf2 [0,0] = " << buf2.getAt(0).real();

            assert(r2s.getHeight() == buf1.getHeight());
            assert(r2s.getWidth() == buf1.getWidth());

            buf1 *= r2s;
            buf2 *= r2s;

            qDebug() << "buf1 [0,0] = " << buf1.getAt(0).real();
            qDebug() << "buf2 [0,0] = " << buf2.getAt(0).real();

            ft.executeFFT(buf1, false);
            ft.executeFFT(buf2, false);

            qDebug() << "buf1 [0,0] = " << buf1.getAt(0).real();
            qDebug() << "buf2 [0,0] = " << buf2.getAt(0).real();

            buf1 *= holo_cos;
            buf2 *= holo_sin;

            qDebug() << "buf1 [0,0] = " << buf1.getAt(0).real();
            qDebug() << "buf2 [0,0] = " << buf2.getAt(0).real();

            buf1 -= buf2;
            qDebug() << "buf1 [0,0] = " << buf1.getAt(0).real();


            ft.executeFFT(buf1, true);

            buf1 /= r2s;

            ft.executeFFT(buf1, false);

            qDebug() << "buf max: " << buf1.getAt(0).real();

            for(unsigned int y=0; y<phaseAngle.getHeight(); y++) {
                for(unsigned int x=0; x<phaseAngle.getWidth(); x++) {
                    double angle = phaseAngle.getAt(y*phaseAngle.getWidth()+x);
                    double phi = buf1.getAt(x, y).real();

                    double val = angle + 2 * M_PI * round((phi - angle) / 2 / M_PI);
                    phaseAngle.getData()[y*phaseAngle.getWidth()+x] = phi;
                }
            }
            qDebug() << "Value: " << phaseAngle.getAt(20);
            phaseAngle.normalize();
            phaseAngle *= 255.0;
            qDebug() << "Max: " << phaseAngle.getMax() << " Min: " << phaseAngle.getMin();
            emit phaseAngleReady(phaseAngle.asQImage());

        } else {
            phaseAngle += M_PI;
            phaseAngle /= 2*M_PI;
            emit phaseAngleReady((phaseAngle * 255.0).asQImage());
        }
        qDebug() << "Frame ready: " << i++;
    }
        */

}
