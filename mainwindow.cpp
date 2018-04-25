#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), ct(nullptr, 1944, 2592),
    settings("FRSEM", "HolMOS GUI")
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

    connect(ui->phaseUnwrapCheck, SIGNAL(stateChanged(int)), this, SLOT(phaseUnwrapCheckChanged(int)));

    connect(&satelliteSelector, SIGNAL(pointSelected(QPoint)), this, SLOT(satellitePointSelected(QPoint)));

    ui->scrollAreaTab1->setWidget(&cameraViewer);
    ui->scrollAreaTab2->setWidget(&satelliteSelector);
    ui->scrollArea_3->setWidget(&phaseViewer);

    /* Load slider positions from settings */
    settings.sync();
    ui->sliderRectX->setValue(settings.value("satellite/rect_x").value<int>());
    ui->sliderRectY->setValue(settings.value("satellite/rect_y").value<int>());
    ui->sliderRectR->setValue(settings.value("satellite/rect_r").value<int>());
	ui->sliderRectX->setMaximum(2592);
	ui->sliderRectY->setMaximum(1944);
    ct.rectX = settings.value("satellite/rect_x").value<int>();
    ct.rectY = settings.value("satellite/rect_y").value<int>();
    ct.rectR = settings.value("satellite/rect_r").value<int>();
    qDebug() << "RectX: " << ct.rectX;

    thread1.start();

}

void MainWindow::sliderRectXChanged(int newval) {
    ct.rectX = newval;
    satelliteSelector.updateXPos(newval);
}
void MainWindow::sliderRectYChanged(int newval) {
    ct.rectY = newval;
    satelliteSelector.updateYPos(newval);
}
void MainWindow::sliderRectRChanged(int newval) {
    ct.rectR = newval;
    satelliteSelector.setIndicatorRadius(newval);
}

void MainWindow::phaseUnwrapCheckChanged(int newval) {
    ct.shouldUnwrapPhase = newval;
}

void MainWindow::satellitePointSelected(QPoint p) {
    emit sliderRectXChanged(p.x());
    emit sliderRectYChanged(p.y());

    ui->sliderRectX->setValue(p.x());
    ui->sliderRectY->setValue(p.y());
}

void MainWindow::cameraImageReceived(QImage img) {
    cameraViewer.setImage(img);
}

void MainWindow::magnitudeSpectrumReceived(QImage img) {
    satelliteSelector.setImage(img);
    //ui->magnitudeImage->setPixmap(QPixmap::fromImage(img).scaled(ui->scrollAreaTab2->width(), ui->cameraImage->height(), Qt::KeepAspectRatio));
}

void MainWindow::phaseAngleReceived(QImage img) {
    phaseViewer.setImage(img);
}

MainWindow::~MainWindow()
{
    qDebug() << "Ending main window";
    qDebug() << settings.value("satellite/rect_x", QVariant::Int);
    settings.setValue("satellite/rect_x", ct.rectX);
    settings.setValue("satellite/rect_y", ct.rectY);
    settings.setValue("satellite/rect_r", ct.rectR);
    settings.sync();
    qDebug() << "Settings written";
    ct.shouldStop = true;
    thread1.quit();
    thread1.wait();
    delete ui;
}
