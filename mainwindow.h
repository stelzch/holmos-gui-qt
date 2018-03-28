#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QPoint>
#include <QImage>
#include "imageselect.h"
#include "computationworker.h"

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
    void phaseUnwrapCheckChanged(int);
    void satellitePointSelected(QPoint);

private:
    Ui::MainWindow *ui;
    ComputationWorker ct;
    QThread thread1;
    ImageSelect satelliteSelector;

};

#endif // MAINWINDOW_H
