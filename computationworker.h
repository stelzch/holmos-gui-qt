#ifndef COMPUTATIONWORKER_H
#define COMPUTATIONWORKER_H

#include <cassert>
#include <iostream>
#include <complex>
#include <chrono>
#include <QDebug>
#include <QObject>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <fftw3.h>
#include "raspicam/raspicam.h"

class ComputationWorker : public QObject
{
    Q_OBJECT
public:
    explicit ComputationWorker(QObject *parent, int n0, int n1);
    void doWork();
    bool shouldStop;
    template<typename T> void fftshift(T* buffer);
    template<typename T> void normalize(T* buffer, int number);
    int rectX, rectY, rectR;

signals:
    void cameraImageReady(QImage);
    void magnitudeSpectrumReady(QImage);
    void phaseAngleReady(QImage);

public slots:
private:
    const int n0, n1;


};

#endif // COMPUTATIONWORKER_H
