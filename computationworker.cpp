#include "computationworker.h"

ComputationWorker::ComputationWorker(QObject *parent) : QObject(parent), shouldStop(false)
{

}
template<typename T> void ComputationWorker::fftshift(T* buffer, int n0, int n1) {
    int yhalf = n0 / 2;
    int xhalf = n1 / 2;

    /* Swap upper-left with bottom-right */
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
    for(int i=0; i<elements; i++) {
        if(buffer[i] < min) min = buffer[i];
        if(buffer[i] > max) max = buffer[i];
    }
    qDebug() << "Normalized, Min/Max: " << min << max;
}

void ComputationWorker::rectXChanged(int newRectX) {
    rectX = newRectX;
}

void ComputationWorker::doWork() {
    rectX = rectY = rectR = 0;
    cv::VideoCapture cam;
    cam.open(0);
    cam.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
    if(!cam.isOpened()) {
        std::cerr << "[ERROR] Could not open camera device" << std::endl;
    }

    QImage cameraImg(n1, n0, QImage::Format_Grayscale8),
            magnitudeSpectrum(n1, n0, QImage::Format_Grayscale8),
            phaseAngle(n1, n0, QImage::Format_Grayscale8);
    QPainter spectrumPainter(&magnitudeSpectrum);
    double *angleBuffer = new double[n0 * n1];
    fftw_complex *fourierTransform = new fftw_complex[n0 * n1];
    fftw_complex *croppedFourierTransform = new fftw_complex[n0 * n1];
    cv::Mat frame;

    /* Plan fourier transform */
    fftw_plan fft1, fft2;
    fft1 = fftw_plan_dft_2d(n0, n1, fourierTransform, fourierTransform, FFTW_FORWARD, FFTW_ESTIMATE);
    fft2 = fftw_plan_dft_2d(n0, n1, croppedFourierTransform, croppedFourierTransform, FFTW_BACKWARD, FFTW_ESTIMATE);
    while(!shouldStop) {
        cam >> frame;
        qDebug() << frame.cols << frame.rows;
        assert(frame.cols == 1280);
        assert(frame.rows == 720);

        for(int y=0; y<n0; y++) {
            for(int x=0; x<n1; x++) {
                //cameraBuffer[y*n1+x] = frame.at<unsigned char>(3*(y*n1+x)+0);
                cameraImg.bits()[y*n1+x] = frame.at<unsigned char>(1*(y*n1+x));
                fourierTransform[y*n1+x][0] = frame.at<unsigned char>(1*(y*n1+x)) / 255.0;
                fourierTransform[y*n1+x][1] = 0.0;
            }
        }
        emit cameraImageReady(cameraImg);

        fftw_execute(fft1);

        fftshift(reinterpret_cast<std::complex<double>*>(fourierTransform), n0, n1);
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
        //croppedFourierTransform[650*n1+(720/2)+8][0] = 250;
        //croppedFourierTransform[650*n1+(720/2)+9][0] = 250;
        //croppedFourierTransform[650*n1+(720/2)+10][0] = 250;

        fftshift(reinterpret_cast<std::complex<double>*>(croppedFourierTransform), n0, n1);

        fftw_execute(fft2);

        /* Calculate the phase angle */
        for(int i=0; i<n0*n1; i++) {
                phaseAngle.bits()[i] = (atan2(croppedFourierTransform[i][0], croppedFourierTransform[i][1]) + M_PI) / (2 * M_PI) * 255;
                if(i==20000) qDebug() << atan2(croppedFourierTransform[i][0], croppedFourierTransform[i][1]);
                //angleBuffer[y*n1+x] = (arg(reinterpret_cast<std::complex<double>*>(croppedFourierTransform)[y*n1+x])*0.2); //+ M_PI) / 2 / M_PI;
        }


        emit phaseAngleReady(phaseAngle);



    }
}
