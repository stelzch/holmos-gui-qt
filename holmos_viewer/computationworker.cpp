#include "computationworker.h"
#include "imageretriever.h"
#include "exceptions.h"
#include <opencv2/opencv.hpp>
#include <cassert>

using namespace cv;
using namespace std;

typedef Point2f ComplexPixel;
typedef Point2d ComplexPixelDouble;

ComputationWorker::ComputationWorker(QObject *parent) : QObject(parent), shouldStop(false), shouldUnwrapPhase(false)
{
    rectX = 0;
    rectY = 0;
    rectR = 0;
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
template<typename T> QImage ComputationWorker::asQImage(Mat img) {
    assert(img.type() == CV_32FC1 || img.type() == CV_64FC1);
    QImage image(img.cols, img.rows, QImage::Format_RGB888);

    img.forEach<T>([&image, &img](T &p, const int position[]) -> void {
            image.bits()[3*(img.cols * position[0] + position[1])+0] = floor(p*255.0);
            image.bits()[3*(img.cols * position[0] + position[1])+1] = floor(p*255.0);
            image.bits()[3*(img.cols * position[0] + position[1])+2] = floor(p*255.0);
    });

    return image;
}

template<typename T> void ComputationWorker::multiplyReal(cv::Mat& input, cv::Mat& factor, cv::Mat& out) {
    assert(input.type() == CV_32FC2 || input.type() == CV_64FC2);
    assert(out.type() == CV_32FC2 || out.type() == CV_64FC2);
    assert(factor.type() == CV_32FC1 || factor.type() == CV_64FC1);

    out.forEach<Point_<T>>([&input, &factor](Point_<T> &p, const int pos[]) -> void {
        T fac = factor.at<T>(pos[0], pos[1]);
        Point_<T> inp = input.at<Point_<T>>(pos[0], pos[1]);
        p.x = inp.x * fac;
        p.y = inp.y * fac;
    });    
}

template<typename T>void ComputationWorker::divideReal(cv::Mat& input, cv::Mat& factor, cv::Mat& out) {
    assert(input.type() == CV_32FC2 || input.type() == CV_64FC2);
    assert(out.type() == CV_32FC2 || out.type() == CV_64FC2);
    assert(factor.type() == CV_32FC1 || factor.type() == CV_64FC1);

    out.forEach<Point_<T>>([&input, &factor](Point_<T> &p, const int pos[]) -> void {
        T fac = factor.at<T>(pos[0], pos[1]);
        Point_<T> inp = input.at<Point_<T>>(pos[0], pos[1]);
        p.x = inp.x / fac;
        p.y = inp.y / fac;
    });    
}

void ComputationWorker::doWork() {
    emit computeRunningStateChanged(true);
    QSettings settings("FRSEM", "HolMOS GUI");

    int cropX = settings.value("capture/cropX", -1).value<int>();
    int cropY = settings.value("capture/cropY", -1).value<int>();

    emit statusMessage(tr("Initializing buffers and fetching test image"));
    qDebug() << "Starting ComputationWorker with CamURL: " << camUrl;
    ImageRetriever ir(camUrl);

    Mat t;
    int n0=-5;
    int n1=-5;
    qDebug() << "Crop before: " << cropX << cropY;
    try {
        t = ir.retrieve();
        if(cropX > t.cols || cropX < 0) cropX = t.cols;
        if(cropY > t.rows || cropY < 0) cropY = t.rows;
        qDebug() << "Crop after: " << cropX << cropY;
        n0 = cropY;
        n1 = cropX;
        qDebug() << t.cols << t.rows;
        emit dimensionsChanged(cropX, cropY);

    } catch(ImageRetrivalException *e) {
        // Something went wrong, just take the easy way out for now...
        qDebug() << "Unable to request image";
        emit computeRunningStateChanged(false);
        emit statusMessage("[ERROR] Could not retrieve image from camera server!");
        return;
    }

    /** TODO: Select centered crop region, not upper left */
    Rect cropRegion(0, 0, n1, n0);
    qDebug() << "Roi selected";
    qDebug() << cropRegion.x << cropRegion.y << cropRegion.width << cropRegion.height;
    qDebug() << n0 << n1;

    /* Create r2s array needed for phase unwrap*/
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
        }
    }

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
    fftwf_complex *fft_buffer = (fftwf_complex *) fftwf_malloc(n0*2*n1*2*sizeof(fftwf_complex));



    while(!shouldStop) {
        /* ===================================
         * PREPARATION
         * ===================================
         * Check if the rect specification lies within our image bounds, emit
         * error message otherwise
         */
        if((rectX + rectR) > t.cols || (rectY + rectR) > t.rows) {
            emit statusMessage(tr("[ERROR] The specified rectangle is outside of the image"));
            break;
        }

        /* ===================================
         * STEP 1
         * ===================================
         * Grab the image from the cam server
         */
        emit statusMessage(tr("Grabbing frame from camera"));
        Mat frame;
        try {
            frame = ir.retrieve();
        } catch(ImageRetrivalException *e) {
            qDebug() << "Unable to request image";
            emit computeRunningStateChanged(false);
            emit statusMessage(tr("[ERROR] Could not retrieve image from camera server!"));
            break;
        }
        extractChannel(frame, frame, 0); // Extract the red channel
        frame(cropRegion).convertTo(img, CV_32FC1, 1/1024.0); // Convert to floating point

        emit cameraImageReady(asQImage<float>(img));

        /* =======================================
         * STEP 2
         * =======================================
         * Calculate the fourier magnitude spectrum
         */
        emit statusMessage(tr("Calculating fourier transform"));

        // Perform a discrete fourier transform
        dft(img, fourierTransform, DFT_COMPLEX_OUTPUT);

        fftshift(fourierTransform); // Center the zero frequency
        split(fourierTransform, planes); // Extract the imaginary and real values into seperate matrices
        cv::magnitude(planes[0], planes[1], magnitudeSpectrum); // Calculate the magnitude spectrum (absolue value of complex number)
        magnitudeSpectrum += Scalar::all(1); // Add 1 to avoid exception when logarithming
        log(magnitudeSpectrum, magnitudeSpectrum); // Make the values logarithmic
        normalize(magnitudeSpectrum, magnitudeSpectrum, 0, 1, CV_MINMAX); // Normalize them before outputting to avoid clipping


        qDebug() << "Calc finished";
        emit magnitudeSpectrumReady(asQImage<float>(magnitudeSpectrum));
        qDebug() << "Magnitude Spectrum emitted";

        /* =========================================
         * STEP 3
         * =========================================
         * Extract satellite and compute phase angle
         */
        emit statusMessage(tr("Calculating phase angle"));
        qDebug() << rectX << rectY << rectR;
        // Reset the cropped array to 0
        cropped.forEach<ComplexPixel>([](ComplexPixel &p, const int pos[]) -> void {
            p.x = 0.0;
            p.y = 0.0;
        });
        // Define the target and source rectangles
        Rect cropRect(rectX-rectR, rectY-rectR, rectR*2, rectR*2); // <- Source (Satellite)
        Rect destRect(n1/2-rectR, n0/2-rectR, rectR*2, rectR*2); // <- Target/Dest (Centered)
        Mat satellite(fourierTransform, cropRect); // <- Satellite pixel values
        satellite.copyTo(cropped(destRect));

        // Shift it and compute the inverse fourier transform
        fftshift(cropped);
        dft(cropped, cropped, DFT_INVERSE);

        // For each pixel in the phaseAngle-matrix, compute the argument of each complex number
        phaseAngle.forEach<float>([&cropped](float&p, const int pos[]) -> void {
                ComplexPixel cr = cropped.at<ComplexPixel>(pos[0], pos[1]);
                p = float(atan2(cr.y, cr.x));
        });


        if(!shouldUnwrapPhase) {
            // If we do not need to unwrap the phase, we're done here
            normalize(phaseAngle, phaseAngleNorm, 0, 1.0, NORM_MINMAX);
            emit phaseAngleReady(asQImage<float>(phaseAngleNorm));
        } else {

            /* ================================
             * STEP 4 (optional)
             * ================================
             * Unwrap the phase
             */
            emit statusMessage(tr("Unwrapping phase angle"));
            // Mirror the phaseAngle-matrix on the bottom x-axis, and then again on the y-axis
            for(int y=0; y<n0; y++) {
                for(int x=0; x<n1; x++) {
                    float val = phaseAngle.at<float>(y, x);

                    mirror.at<ComplexPixel>(y, x).x = val;
                    mirror.at<ComplexPixel>(y, 2*n1 - x - 1).x = val;
                    mirror.at<ComplexPixel>(2*n0 - y - 1, x).x = val;
                    mirror.at<ComplexPixel>(2*n0 - y - 1, 2*n1 - x - 1).x = val;
                }
            }
            // Compute the sine and cosine values of the mirror
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

            // Compute various ffts
            dft(holo_sin_real, holo_sin, DFT_COMPLEX_OUTPUT);
            multiplyReal<float>(holo_sin, r2s_real, holo_sin);
            dft(holo_sin, holo_sin, DFT_INVERSE);
            multiplyReal<float>(holo_sin, holo_cos_real, holo_sin);


            dft(holo_cos_real, holo_cos, DFT_COMPLEX_OUTPUT);
            multiplyReal<float>(holo_cos, r2s_real, holo_cos);
            dft(holo_cos, holo_cos, DFT_INVERSE);
            multiplyReal<float>(holo_cos, holo_sin_real, holo_cos);
            qDebug() << "Cosine multiplied";

            qDebug() << holo_sin.at<ComplexPixel>(20, 20).x;
            double min, max;
            cv::minMaxLoc(holo_sin, &min, &max);
            qDebug() << "Max: " << max << "Min:" << min;
            qDebug() << "Final pass";
            buffer1 = holo_sin - holo_cos;
            dft(buffer1, buffer1);
            divideReal<float>(buffer1, r2s_real, buffer1);
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
            emit phaseAngleReady(asQImage<float>(unwrapped_phase));
        }


    }
    emit computeRunningStateChanged(false);
}
