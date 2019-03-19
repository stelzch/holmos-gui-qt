#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QPoint>
#include <QImage>
#include "imageselect.h"
#include "scaledimageview.h"
#include "computationworker.h"
#include "settingsdialog.h"
#include "mopenglwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void cameraImageReceived(QImage img);
    void magnitudeSpectrumReceived(QImage img);
    void phaseAngleReceived(QImage img);
    void sliderRectXChanged(int);
    void sliderRectYChanged(int);
    void sliderRectRChanged(int);
    void sliderHeightChanged(int);
    void phaseUnwrapCheckChanged(int);
    void satellitePointSelected(QPoint);
    void actionSettingsPressed(bool);

    void dimensionsChanged(int, int);
    void computationStatusMessage(QString);

    void computeButtonPressed();
    void computeRunningState(bool);

private:
    Ui::MainWindow *ui;
    ComputationWorker *ct=nullptr;
    QThread *thread1=nullptr;
    ImageSelect satelliteSelector;
    ScaledImageView cameraViewer, phaseViewer;
    QSettings settings;
    int n0, n1;
    MOpenGLWidget *threedViewer;
    bool computeButtonRunning=false, threedEnabled=false;

    void initComputation();
    void deleteComputation();

};

#endif // MAINWINDOW_H
