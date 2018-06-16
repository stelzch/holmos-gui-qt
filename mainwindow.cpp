#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings("FRSEM", "HolMOS GUI")
{
    ui->setupUi(this);

    bool settingsNew = settings.value("misc/settingsWritten", true).value<bool>();
    if(settingsNew) {

    }

    ct = new ComputationWorker();
    ct->camUrl = settings.value("capture/camUrl", "http://127.0.0.1:3000").value<QString>();

    ct->moveToThread(&thread1);
    connect(&thread1, &QThread::finished, ct, &QObject::deleteLater);
    connect(&thread1, &QThread::started, ct, &ComputationWorker::doWork);

    connect(ct, SIGNAL(dimensionsChanged(int,int)), this, SLOT(dimensionsChanged(int,int)));
    connect(ct, SIGNAL(statusMessage(QString)), this, SLOT(computationStatusMessage(QString)));

    connect(ct, SIGNAL(cameraImageReady(QImage)), this, SLOT(cameraImageReceived(QImage)));
    connect(ct, SIGNAL(magnitudeSpectrumReady(QImage)), this, SLOT(magnitudeSpectrumReceived(QImage)));
    connect(ct, SIGNAL(phaseAngleReady(QImage)), this, SLOT(phaseAngleReceived(QImage)));

    connect(ui->sliderRectX, SIGNAL(valueChanged(int)), this, SLOT(sliderRectXChanged(int)));
    connect(ui->sliderRectY, SIGNAL(valueChanged(int)), this, SLOT(sliderRectYChanged(int)));
    connect(ui->sliderRectR, SIGNAL(valueChanged(int)), this, SLOT(sliderRectRChanged(int)));

    connect(ui->phaseUnwrapCheck, SIGNAL(stateChanged(int)), this, SLOT(phaseUnwrapCheckChanged(int)));

    connect(&satelliteSelector, SIGNAL(pointSelected(QPoint)), this, SLOT(satellitePointSelected(QPoint)));

    connect(ui->actionSettings, SIGNAL(triggered(bool)), this, SLOT(actionSettingsPressed(bool)));

    ui->scrollAreaTab1->setWidget(&cameraViewer);
    ui->scrollAreaTab2->setWidget(&satelliteSelector);
    ui->scrollArea_3->setWidget(&phaseViewer);

    /* Load slider positions from settings */

    thread1.start();

    threedViewer = new MOpenGLWidget();
    ui->verticalLayout_4->addWidget(threedViewer);

}

void MainWindow::actionSettingsPressed(bool) {
    SettingsDialog d(this);
    d.exec();

    //qDebug() << "Returned ";
}

void MainWindow::sliderRectXChanged(int newval) {
    ct->rectX = ui->sliderRectX->value();
    satelliteSelector.updateXPos(newval);
}
void MainWindow::sliderRectYChanged(int newval) {
    ct->rectY = ui->sliderRectY->value();
    satelliteSelector.updateYPos(newval);
}
void MainWindow::sliderRectRChanged(int newval) {
    ct->rectR = ui->sliderRectR->value();
    satelliteSelector.setIndicatorRadius(newval);
}

void MainWindow::phaseUnwrapCheckChanged(int newval) {
    ct->shouldUnwrapPhase = newval;
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
}

void MainWindow::phaseAngleReceived(QImage img) {
    QImage conv = img.convertToFormat(QImage::Format_RGB888);
    phaseViewer.setImage(conv);
    threedViewer->showImage(img.convertToFormat(QImage::Format_RGB888));
}

void MainWindow::dimensionsChanged(int width, int height) {
    qDebug() << "Resetting slider maxes";
    ui->sliderRectX->setMaximum(width);
    ui->sliderRectY->setMaximum(height);
    ui->sliderRectR->setMaximum(height / 4);
}

void MainWindow::computationStatusMessage(QString str) {
    ui->statusBar->showMessage(str);
}

MainWindow::~MainWindow()
{
    qDebug() << "Ending main window";
    qDebug() << settings.value("satellite/rect_x", QVariant::Int);
    settings.setValue("satellite/rect_x", ct->rectX);
    settings.setValue("satellite/rect_y", ct->rectY);
    settings.setValue("satellite/rect_r", ct->rectR);
    settings.sync();
    qDebug() << "Settings written";
    ct->shouldStop = true;
    thread1.quit();
    thread1.wait();
    delete ui;
    delete ct;
    delete threedViewer;
}
