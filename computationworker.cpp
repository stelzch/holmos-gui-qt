#include "computationworker.h"
#include <opencv2/opencv.hpp>
#include <cassert>

#ifndef DEBUG_PHONY_CAMERA
#include <raspicam/raspicam_cv.h>
#endif

using namespace cv;
using namespace std;

typedef Point2f ComplexPixel;

ComputationWorker::ComputationWorker(QObject *parent, int n0, int n1, int captureX, int captureY) : QObject(parent), shouldStop(false), n0(n0), n1(n1), shouldUnwrapPhase(false),
    rectX(0), rectY(0), rectR(0),
    captureX(captureX), captureY(captureY)
{
    assert(captureY >= n0);
    assert(captureX >= n1);
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
    qDebug() << "ComputationWorker initiated, captureX=" << captureX << "captureY=" << captureY;
#ifdef DEBUG_PHONY_CAMERA
    Mat image = imread("holmos_raw.png", 0);
    assert(image.type() == CV_8UC1);
    assert(image.rows == captureY);
    assert(image.cols == captureX);
#else
	raspicam::RaspiCam_Cv camera;
	camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
    camera.set(CV_CAP_PROP_FRAME_WIDTH, captureX);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT, captureY);
	camera.open();

#endif
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

	Mat frame;
    int left = floor((captureX - n1) / 2);
    int top = floor((captureY - n0) / 2);
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
    Mat buffer1(2*n0, 2*n1, CV_32FC2);

    
    fftwf_plan fft1, fft2;
    fftwf_complex *fft_buffer = (fftwf_complex *) fftwf_malloc(n0*2*n1*2*sizeof(fftwf_complex));
    fft1 = fftwf_plan_dft_2d(n0, n1, fft_buffer, fft_buffer, FFTW_FORWARD, FFTW_ESTIMATE);
    fft2 = fftwf_plan_dft_r2c_2d(n0, n1, img.ptr<float>(),
        reinterpret_cast<fftwf_complex*>(fourierTransform.ptr<ComplexPixel>()),
        FFTW_ESTIMATE);

    while(shouldStop == false) {

        /* Load image and convert to floating point */
#ifdef DEBUG_PHONY_CAMERA
        img.forEach<float>([&image, &left, &top](float &p, const int position[]) -> void {
                p = (float) image.at<unsigned char>(top + position[0], left + position[1]) / 255.0;
        });
#else
		camera.grab();
		camera.retrieve(frame);
        img.forEach<float>([&frame, &left, &top](float &p, const int position[]) -> void {
                p = frame.at<Point3_<uint8_t>>(top + position[0], left + position[1]).z / 255.0;
        });
#endif
        qDebug() << "Emitting camera image";


		
        emit cameraImageReady(asQImage(img));

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

}
