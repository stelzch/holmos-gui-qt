#include "computationworker.h"
#include "mimage.h"
#include "mcompleximage.h"
#include "fouriertransformer.h"

ComputationWorker::ComputationWorker(QObject *parent, int n0, int n1) : QObject(parent), shouldStop(false), n0(n0), n1(n1), shouldUnwrapPhase(false),
    rectX(0), rectY(0), rectR(0)
{

}

void ComputationWorker::doWork() {

    cv::Mat image = cv::imread("holmos_raw.png", 0);
    assert(image.rows == n0);
    assert(image.cols == n1);


    MGrayImage img = MGrayImage::loadFromFile("/home/cstelz/mtestimage.png");
    MComplexImage cimg = MComplexImage::fromGrayImage(img);
    MComplexImage cropped(n0, n1);
    MComplexImage mirrored(n0*2, n1*2);
    MComplexImage holo_cos(mirrored), holo_sin(mirrored), buf1(mirrored), buf2(mirrored);
    MGrayImage r2s(n0*2, n1*2);
    r2s.initValue(1e-9);
    qDebug() << cimg.getWidth() << "x" << cimg.getHeight();

    /* Create r2s array */
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

}
