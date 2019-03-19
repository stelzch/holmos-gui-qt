#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QToolBar>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings("FRSEM", "HolMOS GUI")
{
    ui->setupUi(this);

    bool settingsNew = settings.value("misc/settingsNew", true).value<bool>();
    if(settingsNew) {
        // Initialize default values
        settings.setValue("capture/camUrl", "http://127.0.0.1:3000");
        settings.setValue("capture/cropX", -1);
        settings.setValue("capture/cropY", -1);

        settings.setValue("satellite/rect_x", 0);
        settings.setValue("satellite/rect_y", 0);
        settings.setValue("satellite/rect_r", 0);

        settings.setValue("misc/settingsNew", false);
        settings.setValue("misc/enable_3d", false);
        settings.sync();
    }

    // Start the computation worker in the background
    initComputation();

    threedEnabled = settings.value("misc/enable_3d", false).value<bool>();

    connect(ui->sliderRectX, SIGNAL(valueChanged(int)), this, SLOT(sliderRectXChanged(int)));
    connect(ui->sliderRectY, SIGNAL(valueChanged(int)), this, SLOT(sliderRectYChanged(int)));
    connect(ui->sliderRectR, SIGNAL(valueChanged(int)), this, SLOT(sliderRectRChanged(int)));

    connect(ui->phaseUnwrapCheck, SIGNAL(stateChanged(int)), this, SLOT(phaseUnwrapCheckChanged(int)));

    connect(&satelliteSelector, SIGNAL(pointSelected(QPoint)), this, SLOT(satellitePointSelected(QPoint)));

    connect(ui->actionSettings, SIGNAL(triggered(bool)), this, SLOT(actionSettingsPressed(bool)));

    ui->scrollAreaTab1->setWidget(&cameraViewer);
    ui->scrollAreaTab2->setWidget(&satelliteSelector);
    ui->scrollArea_3->setWidget(&phaseViewer);

    if(threedEnabled) {
        threedViewer = new MOpenGLWidget();
        ui->verticalLayout_4->addWidget(threedViewer);
        connect(ui->heightScale, SIGNAL(valueChanged(int)), this, SLOT(sliderHeightChanged(int)));
    }

    /* Setup start/stop computation button */
    ui->mainToolBar->addAction(ui->actionCompute);
    ui->actionCompute->setIcon(QIcon::fromTheme("media-playback-start-symbolic"));
    connect(ui->actionCompute, SIGNAL(triggered(bool)), this, SLOT(computeButtonPressed()));
}

void MainWindow::initComputation(void) {
    deleteComputation();

    thread1 = new QThread();
    ct = new ComputationWorker();

    // Make the computations in a seperate thread to avoid GUI blocking
    ct->moveToThread(thread1);
    connect(thread1, &QThread::started, ct, &ComputationWorker::doWork);

    // Load satellite position and camera server url from settings
    ct->camUrl = settings.value("capture/camUrl", "http://127.0.0.1:3000").value<QString>();
    ct->rectX = settings.value("satellite/rect_x", 0).toInt();
    ct->rectY = settings.value("satellite/rect_y", 0).toInt();
    ct->rectR = settings.value("satellite/rect_r", 0).toInt();

    // Connect the computationRunningStateChanged signal to inform the GUI
    // about changes and errors in the computation worker
    connect(ct, SIGNAL(computeRunningStateChanged(bool)), this, SLOT(computeRunningState(bool)));

    // Connect the slots of various results produced in the computation worker
    // with the GUI functions that display them accordingly
    connect(ct, SIGNAL(dimensionsChanged(int,int)), this, SLOT(dimensionsChanged(int,int)));
    connect(ct, SIGNAL(statusMessage(QString)), this, SLOT(computationStatusMessage(QString)));

    connect(ct, SIGNAL(cameraImageReady(QImage)), this, SLOT(cameraImageReceived(QImage)));
    connect(ct, SIGNAL(magnitudeSpectrumReady(QImage)), this, SLOT(magnitudeSpectrumReceived(QImage)));
    connect(ct, SIGNAL(phaseAngleReady(QImage)), this, SLOT(phaseAngleReceived(QImage)));


    thread1->start();
}
void MainWindow::deleteComputation(void) {
    if(ct != nullptr) {
        ct->shouldStop = true;
        if(thread1 != nullptr) {
            thread1->quit();
            thread1->wait();
        }
    }
    delete ct;
    delete thread1;

    ct = nullptr;
    thread1 = nullptr;
}


/**
 * The Settings-Button was pressed. Stop the computation, display the settings
 * dialog. The user might adjust the crop region that the ComputationWorker processes
 * or the URL of the camera server might change, so we need to reinitiate the
 * ComputationWorker afterwards, to ensure the new settings are used accordingly.
 */
void MainWindow::actionSettingsPressed(bool) {
    deleteComputation();

    SettingsDialog d(this);
    d.exec();

    initComputation();
}

void MainWindow::computeButtonPressed() {
    // Toggle button state
    if(!computeButtonRunning) {
        // Button displays start, should start ct
        qDebug() << "Requested ct start";
        initComputation();
    } else {
        // Button displays stop, should stop ct
        qDebug() << "Requested ct stop";
        deleteComputation();
    }

}
void MainWindow::computeRunningState(bool running) {
    if(running) {
        // ComputationThread started, display stop button
        qDebug() << "CT started";
        computeButtonRunning = true;
        ui->actionCompute->setIcon(QIcon::fromTheme("media-playback-stop-symbolic"));
        ui->actionCompute->setText("Stop");
    } else {
        // ComputationThread quit, show start button
        computationStatusMessage("Computation finished");
        computeButtonRunning = false;
        ui->actionCompute->setIcon(QIcon::fromTheme("media-playback-start-symbolic"));
        ui->actionCompute->setText("Start");
    }
}

void MainWindow::sliderRectXChanged(int newval) {
    if(ct != nullptr) {
        ct->rectX = ui->sliderRectX->value();
        satelliteSelector.updateXPos(newval);
    }
}
void MainWindow::sliderRectYChanged(int newval) {
    if(ct != nullptr) {
        ct->rectY = ui->sliderRectY->value();
        satelliteSelector.updateYPos(newval);
    }
}
void MainWindow::sliderRectRChanged(int newval) {
    if(ct != nullptr) {
        ct->rectR = ui->sliderRectR->value();
        satelliteSelector.setIndicatorRadius(newval);
    }
}

void MainWindow::sliderHeightChanged(int val) {
    if(threedViewer != nullptr) {
        threedViewer->setHeightScale(val);
    }
}

void MainWindow::phaseUnwrapCheckChanged(int newval) {
    if(ct != nullptr) {
        ct->shouldUnwrapPhase = newval;
    }
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
    if(threedEnabled)
        threedViewer->showImage(img.convertToFormat(QImage::Format_RGB888));
}

void MainWindow::dimensionsChanged(int width, int height) {
    qDebug() << "Resetting slider maxes";
    ui->sliderRectX->setMaximum(width);
    ui->sliderRectY->setMaximum(height);
    ui->sliderRectR->setMaximum(height / 4);
}

void MainWindow::computationStatusMessage(QString str) {
    if(str.startsWith("[ERROR]")) {
        QMessageBox mb;
        mb.critical(0, "Error", str);
        return;
    }
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


    deleteComputation();
    if(threedEnabled)
        delete threedViewer;
    delete ui;
}
