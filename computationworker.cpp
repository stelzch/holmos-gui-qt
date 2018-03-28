#include "computationworker.h"
#include "mimage.h"
#include "mcompleximage.h"
#include "fouriertransformer.h"

ComputationWorker::ComputationWorker(QObject *parent, int n0, int n1) : QObject(parent), shouldStop(false), n0(n0), n1(n1), shouldUnwrapPhase(false)
{

}

std::vector<double> ComputationWorker::getMagnitudeSpectrum(std::complex<double> *buffer, unsigned int n0, unsigned int n1) {
    std::vector<double> buf;
    buf.resize(n0 * n1);
    for(int y=0; y<n0; y++) {
        for(int x=0; x<n1; x++) {
            buf[y*n1+x] = abs(buffer[y*n1+x]);
        }
    }

    return buf;
}

template<typename T> QImage ComputationWorker::bufferToQImage(T* buffer, unsigned int n0, unsigned int n1) {
    QImage img(n1, n0, QImage::Format_Grayscale8);

    for(int y=0; y<n0; y++) {
        for(int x=0; x<n1; x++) {
            img.bits()[y*n1+x] = static_cast<uchar>(buffer[y*n1+x]);
        }
    }

    return img;
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


    MGrayImage img = MGrayImage::loadFromFile("holmos_raw.png");
    MComplexImage cimg = MComplexImage::fromGrayImage(img);
    MComplexImage cropped(n0, n1);
    qDebug() << cimg.getWidth() << "x" << cimg.getHeight();

    FourierTransformer ft;
    ft.planFFTFor(cimg, true);
    ft.planFFTFor(cropped, false);


    qDebug() << img.getWidth() << "x" << img.getHeight();
    auto i = 0;
    while(!shouldStop) {

        emit cameraImageReady(img.asQImage());
        MComplexImage cimg2 = MComplexImage::fromGrayImage(img);



        ft.executeFFT(cimg2, true);
        cimg2.fftshift();

        auto magspec = cimg2.getMagnitudeSpectrum() / 500.0;

        emit magnitudeSpectrumReady(magspec.asQImage());

        qDebug() << rectX << rectY;
        auto satellite = cimg2.getSector(rectX-rectR, rectY-rectR, rectR*2);
        cropped.initValue(std::complex<double>(0.0, 0.0));
        cropped.setSector((n1 / 2)-rectR, (n0 / 2)-rectR, satellite);

        //emit phaseAngleReady((cropped.getMagnitudeSpectrum() / 500.0).asQImage());

        cropped.fftshift();
        ft.executeFFT(cropped, false);

        auto phaseAngle = cropped.getAngle();
        phaseAngle += M_PI;
        phaseAngle /= 2*M_PI;

        emit phaseAngleReady((phaseAngle * 255.0).asQImage());
        qDebug() << "Frame ready: " << i++;
    }


/*/*
    raspicam::RaspiCam cam;
    cam.setWidth(n1);
    cam.setHeight(n0);
    cam.setFormat(raspicam::RASPICAM_FORMAT_RGB);
    cam.open();*

    QImage cameraImg(n1, n0, QImage::Format_Grayscale8),
            magnitudeSpectrum(n1, n0, QImage::Format_Grayscale8),
            phaseAngle(n1, n0, QImage::Format_Grayscale8);

    QPainter spectrumPainter(&magnitudeSpectrum);
    unsigned char *buffer = new unsigned char[n0*n1*3];
    double *phaseAngleBuffer = new double[n0*n1];
    fftw_init_threads();
    fftw_plan_with_nthreads(4);
    fftw_complex *fourierTransform = new fftw_complex[n0 * n1];
    fftw_complex *croppedFourierTransform = new fftw_complex[n0 * n1];

    * Variables for the phase unwrap *
    double *r2s = new double[n0 * n1 * 4];
    double *holo_cos = new double[n0 * n1 * 4];
    double *holo_sin = new double[n0 * n1 * 4];
    double *mirrored = new double[n0 * n1 * 4];
    std::vector<double> x2;
    std::vector<double> y2;
    std::complex<double> *phi_prime = new std::complex<double>[n0 * n1 * 4];
    std::complex<double> *unwrap_fft_buffer = new std::complex<double>[n0 * n1 * 4];
    std::complex<double> *unwrap_fft_buffer2 = new std::complex<double>[n0 * n1 * 4];

    fftw_plan fft3 = fftw_plan_dft_r2c_2d(n0, n1, holo_sin, reinterpret_cast<fftw_complex*>(unwrap_fft_buffer), FFTW_MEASURE);
    fftw_plan fft4 = fftw_plan_dft_r2c_2d(n0, n1, holo_cos, reinterpret_cast<fftw_complex*>(unwrap_fft_buffer), FFTW_MEASURE);
    fftw_plan fft5 = fftw_plan_dft_2d(n0, n1, reinterpret_cast<fftw_complex*>(unwrap_fft_buffer),
                                                                              reinterpret_cast<fftw_complex*>(unwrap_fft_buffer2), FFTW_BACKWARD, FFTW_MEASURE);
    fftw_plan fft6 = fftw_plan_dft_2d(n0, n1, reinterpret_cast<fftw_complex*>(unwrap_fft_buffer),
                                                                              reinterpret_cast<fftw_complex*>(unwrap_fft_buffer), FFTW_BACKWARD, FFTW_MEASURE);
    fftw_plan fft7 = fftw_plan_dft_2d(n0, n1, reinterpret_cast<fftw_complex*>(unwrap_fft_buffer),
                                                                              reinterpret_cast<fftw_complex*>(unwrap_fft_buffer), FFTW_FORWARD, FFTW_MEASURE);


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
            r2s[y*(2*n1)+x] = x2.at(x) + y2.at(y) + 1e-6;
        }
    }
    qDebug() << "r2s[10,10] = " << r2s[10*2048+10];

    * Plan fourier transform *
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

        * Copy ROI to cropppedFourierTransform *
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

        * Calculate the phase angle *
        for(int i=0; i<n0*n1; i++) {
            phaseAngleBuffer[i] = atan2(croppedFourierTransform[i][0], croppedFourierTransform[i][1]);
        }
        if(shouldUnwrapPhase) {
            // Fill mirrored array
            for(int y=0; y<n0; y++) {
                for(int x=0; x<n1; x++) {
                    mirrored[y*n1*2+x] = phaseAngleBuffer[y*n1+x];
                    mirrored[y*n1*2+(x+n1)] = phaseAngleBuffer[y*n1+(n1-x)];
                    mirrored[(y+n0)*n1*2+x] = phaseAngleBuffer[(n0-y)*n1+x];
                    mirrored[(y+n0)*n1*2+(x+n1)] = phaseAngleBuffer[(n0-y)*n1+(n1-x)];


                }
            }
            qDebug() << "mirrored[0,0]" << mirrored[0];
            qDebug() << "mirrored[2000,0]" << mirrored[0*n1+2000];
            qDebug() << "mirrored[2000,2000]" << mirrored[2000*n1+2000];
            qDebug() << "mirrored[2000,0]" << mirrored[2000*n1+0];
            for(int y=0; y<n0*2; y++) {
                for(int x=0; x<n1*2; x++) {
                    holo_cos[y*n1*2+x] = cos(mirrored[y*n1*2+x]);
                    holo_sin[y*n1*2+x] = sin(mirrored[y*n1*2+x]);
                }
            }
            * fft3: holo_sin -> unwrap_fft_buffer (F)
             * fft4: holo_cos -> unwrap_fft_buffer (F)
             * fft5: unwrap_fft_buffer -> unwrap_fft_buffer2 (B)
             * fft6: unwrap_fft_buffer -> unwrap_fft_buffer  (B)
             * fft7: unwrap_fft_buffer -> unwrap_fft_buffer  (F)
             *
            fftw_execute(fft3);
            emit phaseAngleReady(bufferToQImage(getMagnitudeSpectrum(unwrap_fft_buffer, n0, n1).data(), n0, n1));
            for(int i=0; i<n0*n1*4; i++)
                unwrap_fft_buffer[i] *= r2s[i];
            qDebug() << unwrap_fft_buffer[0].real();

            fftw_execute(fft5);
            for(int i=0; i<n0*n1*4; i++)
                unwrap_fft_buffer2[i] += holo_cos[i];
            qDebug() << unwrap_fft_buffer[0].real();

            fftw_execute(fft4);
            for(int i=0; i<n0*n1*4; i++)
                unwrap_fft_buffer[i] *= r2s[i];
            qDebug() << unwrap_fft_buffer[0].real();

            fftw_execute(fft6);
            for(int i=0; i<n0*n1*4; i++)
                unwrap_fft_buffer[i] += holo_sin[i];
            qDebug() << unwrap_fft_buffer[0].real();

            for(int i=0; i<n0*n1*4; i++)
                unwrap_fft_buffer[i] = unwrap_fft_buffer2[i] - unwrap_fft_buffer[i];
            fftw_execute(fft7);
            qDebug() << unwrap_fft_buffer[0].real();

            for(int i=0; i<n0*n1*4; i++)
                unwrap_fft_buffer[i] /= r2s[i];
            qDebug() << unwrap_fft_buffer[0].real();

            fftw_execute(fft6);
            qDebug() << unwrap_fft_buffer[1].real();

            for(int y=0; y<n0; y++) {
                for(int x=0; x<n1; x++) {
                    double phi_ = unwrap_fft_buffer[y*n1+x].real();
                    double unwrapped_phase = (phaseAngleBuffer[y*n1+x] * 2*M_PI*round((phi_ - phaseAngleBuffer[y*n1+x]) / 2 / M_PI));
                    phaseAngle.bits()[y*n1+x] = (unwrapped_phase + M_PI) / (2*M_PI) * 255;
                }
                //if(y%50==0)
                //qDebug() << phaseAngle.bits()[y*n1];
            }
        } else {
            for(int i=0; i<n0*n1; i++) {
                phaseAngle.bits()[i] = (phaseAngleBuffer[i] + M_PI) / (2 * M_PI) * 255;
            }
            emit phaseAngleReady(phaseAngle);
        }





        t2 = std::chrono::steady_clock::now();
        secs = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        fps = 1 / (secs / 1000.0);
        qDebug() << "Calculation took" << secs << "ms, FPS: " << fps;
    }
    delete [] buffer;
    //cam.release();

    fftw_destroy_plan(fft1);
    fftw_destroy_plan(fft2);
    fftw_destroy_plan(fft3);
    fftw_destroy_plan(fft4);
    fftw_destroy_plan(fft5);
    fftw_destroy_plan(fft6);
    fftw_destroy_plan(fft7);

    fftw_cleanup_threads(); */
}
