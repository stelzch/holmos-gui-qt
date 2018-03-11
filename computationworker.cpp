#include "computationworker.h"

ComputationWorker::ComputationWorker(QObject *parent, int n0, int n1) : QObject(parent), shouldStop(false), n0(n0), n1(n1), shouldUnwrapPhase(false)
{

}
template<typename T> void ComputationWorker::fftshift(T* buffer) {
    int yhalf = n0 / 2;
    int xhalf = n1 / 2;

    /* Swap upper-left with bottom-right */
#pragma omp parallel for collapse(2)
    for(int y=0; y<yhalf; y++) {
        for(int x=0; x<xhalf; x++) {
            T tmp;
            int coord_upper_left = y*n1+x;
            int coord_bottom_right = (y+yhalf)*n1+x+xhalf;
            tmp = buffer[coord_upper_left];
            buffer[coord_upper_left] = buffer[coord_bottom_right];
            buffer[coord_bottom_right] = tmp;
        }
    }

    /* Swap bottom-left with upper-right */
#pragma omp parallel for collapse(2)
    for(int y=yhalf; y<n0; y++) {
        for(int x=0; x<xhalf; x++) {
            T tmp;
            int coord_bottom_left = y*n1+x;
            int coord_upper_right = (y-yhalf)*n1+x+xhalf;
            tmp = buffer[coord_bottom_left];
            buffer[coord_bottom_left] = buffer[coord_upper_right];
            buffer[coord_upper_right] = tmp;
        }
    }
}
template<typename T> void ComputationWorker::normalize(T* buffer, int elements) {
    T min, max;
#pragma omp parallel for
    for(int i=0; i<elements; i++) {
        if(buffer[i] < min) min = buffer[i];
        if(buffer[i] > max) max = buffer[i];
    }
    qDebug() << "Normalized, Min/Max: " << min << max;
}

void ComputationWorker::doWork() {

    rectX = rectY = rectR = 0;

    cv::Mat image = cv::imread("holmos_raw.png", 0);
    assert(image.rows == n0);
    assert(image.cols == n1);
/*
    raspicam::RaspiCam cam;
    cam.setWidth(n1);
    cam.setHeight(n0);
    cam.setFormat(raspicam::RASPICAM_FORMAT_RGB);
    cam.open();*/

    QImage cameraImg(n1, n0, QImage::Format_Grayscale8),
            magnitudeSpectrum(n1, n0, QImage::Format_Grayscale8),
            phaseAngle(n1, n0, QImage::Format_Grayscale8);

    QPainter spectrumPainter(&magnitudeSpectrum);
    unsigned char *buffer = new unsigned char[n0*n1*3];
    fftw_init_threads();
    fftw_plan_with_nthreads(4);
    fftw_complex *fourierTransform = new fftw_complex[n0 * n1];
    fftw_complex *croppedFourierTransform = new fftw_complex[n0 * n1];

    /* Variables for the phase unwrap */
    double *r2s = new double[n0 * n1 * 4];
    double *holo_cos = new double[n0 * n1 * 4];
    double *holo_sin = new double[n0 * n1 * 4];
    std::vector<double> x2;
    std::vector<double> y2;
    fftw_complex *phi_prime = new fftw_complex[n0 * n1 * 4];
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
            r2s[y*(2*n1)+x] = x2.at(x) + y2.at(y);
        }
    }
    qDebug() << "r2s[10,10] = " << r2s[10*2048+10];

    /* Plan fourier transform */
    fftw_plan fft1, fft2;
    fft1 = fftw_plan_dft_2d(n0, n1, fourierTransform, fourierTransform, FFTW_FORWARD, FFTW_MEASURE);
    fft2 = fftw_plan_dft_2d(n0, n1, croppedFourierTransform, croppedFourierTransform, FFTW_BACKWARD, FFTW_MEASURE);
    std::chrono::steady_clock::time_point t1, t2;
    double secs, fps;
    for(int y=0; y<n0; y++) {
        for(int x=0; x<n1; x++) {
            if(y<image.rows && x<image.cols) {
                buffer[3*(y*n1+x)] = image.at<uchar>(y*n1+x);
                buffer[3*(y*n1+x)+1] = 0;
                buffer[3*(y*n1+x)+2] = 0;
            }
        }
    }
    while(!shouldStop) {
        t1 = std::chrono::steady_clock::now();
        //cam.grab();
        //cam.retrieve(buffer);
        qDebug() << "Frame";

#pragma omp parallel for
        for(int i=0; i<n0*n1; i++) {
            cameraImg.bits()[i] = image.at<uchar>(i);
            fourierTransform[i][0] = buffer[3*i+0] / 255.0f;
            fourierTransform[i][1] = 0.0f;
        }
        emit cameraImageReady(cameraImg);
        fftw_execute(fft1);

        fftshift(reinterpret_cast<std::complex<double>*>(fourierTransform));

#pragma omp parallel for
        for(int y=0; y<n0; y++)
            for(int x=0; x<n1; x++)
                magnitudeSpectrum.bits()[y*n1+x] = (abs(reinterpret_cast<std::complex<double>*>(fourierTransform)[y*n1+x]))*0.2;
        //normalize<unsigned char>(magnitudeSpectrum.bits(), n0*n1);

        spectrumPainter.setPen(Qt::white);
        spectrumPainter.drawRect(rectX, rectY, rectR*2, rectR*2);

        emit magnitudeSpectrumReady(magnitudeSpectrum);

        /* Copy ROI to cropppedFourierTransform */
        memset(croppedFourierTransform, 0, sizeof(fftw_complex) * n0 * n1);
        for(int y=rectY; y<(rectY+rectR*2); y++) {
            for(int x=rectX; x<(rectX+rectR*2); x++) {
                int newX = x - rectX + n1 * 0.5 - rectR;
                int newY = y - rectY + n0 * 0.5 - rectR;
                croppedFourierTransform[newY*n1+newX][0] = fourierTransform[y*n1+x][0];
                croppedFourierTransform[newY*n1+newX][1] = fourierTransform[y*n1+x][1];
            }
        }

        fftshift(reinterpret_cast<std::complex<double>*>(croppedFourierTransform));

        fftw_execute(fft2);

        /* Calculate the phase angle */
        for(int i=0; i<n0*n1; i++) {
                phaseAngle.bits()[i] = (atan2(croppedFourierTransform[i][0], croppedFourierTransform[i][1]) + M_PI) / (2 * M_PI) * 255;
        }


        emit phaseAngleReady(phaseAngle);
        t2 = std::chrono::steady_clock::now();
        secs = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        fps = 1 / (secs / 1000.0);
        qDebug() << "Calculation took" << secs << "ms, FPS: " << fps;
    }
    delete [] buffer;
    //cam.release();

    fftw_cleanup_threads();
}
