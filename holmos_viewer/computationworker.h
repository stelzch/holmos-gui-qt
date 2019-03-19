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
#include <vector>
#include <algorithm>
#include <QSettings>
#include "fpscounter.h"


class ComputationWorker : public QObject
{
    Q_OBJECT
public:
    explicit ComputationWorker(QObject *parent = 0);
    void doWork();
    void fftshift(cv::Mat);
    template<typename T> void multiplyReal(cv::Mat& input, cv::Mat& factor, cv::Mat& out);
    template<typename T> void divideReal(cv::Mat& input, cv::Mat& factor, cv::Mat& out);
    template<typename T> QImage asQImage(cv::Mat);
    bool shouldStop;
    int rectX, rectY, rectR;
    bool shouldUnwrapPhase;
    QString camUrl;

signals:
    void cameraImageReady(QImage);
    void magnitudeSpectrumReady(QImage);
    void phaseAngleReady(QImage);
    void dimensionsChanged(int width, int height);
    void statusMessage(QString);
    void computeRunningStateChanged(bool);

public slots:
private:
    FPSCounter fps;


};

#endif // COMPUTATIONWORKER_H
