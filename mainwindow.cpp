#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), ct(nullptr, 960, 1280)
{
    ui->setupUi(this);

    ct.moveToThread(&thread1);
    connect(&thread1, &QThread::finished, &ct, &QObject::deleteLater);
    connect(&thread1, &QThread::started, &ct, &ComputationWorker::doWork);

    connect(&ct, SIGNAL(cameraImageReady(QImage)), this, SLOT(cameraImageReceived(QImage)));
    connect(&ct, SIGNAL(magnitudeSpectrumReady(QImage)), this, SLOT(magnitudeSpectrumReceived(QImage)));
    connect(&ct, SIGNAL(phaseAngleReady(QImage)), this, SLOT(phaseAngleReceived(QImage)));

    connect(ui->sliderRectX, SIGNAL(valueChanged(int)), this, SLOT(sliderRectXChanged(int)));
    connect(ui->sliderRectY, SIGNAL(valueChanged(int)), this, SLOT(sliderRectYChanged(int)));
    connect(ui->sliderRectR, SIGNAL(valueChanged(int)), this, SLOT(sliderRectRChanged(int)));

    thread1.start();

}

void MainWindow::sliderRectXChanged(int newval) {
    ct.rectX = newval;
}
void MainWindow::sliderRectYChanged(int newval) {
    ct.rectY = newval;
}
void MainWindow::sliderRectRChanged(int newval) {
    ct.rectR = newval;
}

void MainWindow::cameraImageReceived(QImage img) {
    ui->cameraImage->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::magnitudeSpectrumReceived(QImage img) {
    ui->magnitudeImage->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::phaseAngleReceived(QImage img) {
    ui->phaseAngle->setPixmap(QPixmap::fromImage(img));
}

MainWindow::~MainWindow()
{
    ct.shouldStop = true;
    thread1.quit();
    thread1.wait();
    delete ui;
}
