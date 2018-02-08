#ifndef COMPUTATIONWORKER_H
#define COMPUTATIONWORKER_H

#include <cassert>
#include <iostream>
#include <complex>
#include <QDebug>
#include <QObject>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <fftw3.h>

class ComputationWorker : public QObject
{
    Q_OBJECT
public:
    explicit ComputationWorker(QObject *parent = 0);
    void doWork();
    bool shouldStop;
    template<typename T> void fftshift(T* buffer, int n0, int n1);
    template<typename T> void normalize(T* buffer, int number);
    int rectX, rectY, rectR;

public slots:
    void rectXChanged(int);

signals:
    void cameraImageReady(QImage);
    void magnitudeSpectrumReady(QImage);
    void phaseAngleReady(QImage);

public slots:
private:
    const int n0 = 720, n1 = 1280;


};

#endif // COMPUTATIONWORKER_H
