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
    explicit ComputationWorker(QObject *parent, int n0, int n1, int captureX, int captureY);
    void doWork();
    void fftshift(cv::Mat);
    void multiplyComplex(cv::Mat& input, cv::Mat& factor, cv::Mat& out);
    void multiplyReal(cv::Mat& input, cv::Mat& factor, cv::Mat& out);
    void divideComplex(cv::Mat& input, cv::Mat& factor, cv::Mat& out);
    void divideReal(cv::Mat& input, cv::Mat& factor, cv::Mat& out);
    void saveMat(QString filename, cv::Mat& mat);

    QImage asQImage(cv::Mat);
    bool shouldStop;
    int rectX, rectY, rectR;
    bool shouldUnwrapPhase;

signals:
    void cameraImageReady(QImage);
    void magnitudeSpectrumReady(QImage);
    void phaseAngleReady(QImage);

public slots:
private:
    const int n0, n1;
    const int captureX, captureY;
    FPSCounter fps;


};

#endif // COMPUTATIONWORKER_H
