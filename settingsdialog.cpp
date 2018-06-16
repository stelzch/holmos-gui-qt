#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

//    connect(ui->captureResolutionBox, SIGNAL(currentIndexChanged(int)), this, SLOT(captureResolutionChanged(int)));

//    ui->captureResolutionBox->setCurrentText(settings.value("capture/captureRes").value<QString>());
//    ui->cropResolutionX->setValue(settings.value("capture/cropResX", 1024).value<int>());
//    ui->cropResolutionY->setValue(settings.value("capture/cropResY", 1024).value<int>());
//    ui->

//    if(ui->captureResolutionBox->currentText() == "Raspberry Camera v1") {
//        ui->cropResolutionX->setMaximum(2592);
//        ui->cropResolutionY->setMaximum(1944);
//    } else if(ui->captureResolutionBox->currentText() == "Raspberry Camera v2") {
//        ui->cropResolutionX->setMaximum(3280);
//        ui->cropResolutionY->setMaximum(2464);
//    }

    ui->camAddressInput->setText(settings.value("capture/camUrl").value<QString>());
}

void SettingsDialog::captureResolutionChanged(int) {
//    settings.setValue("capture/captureRes", ui->captureResolutionBox->currentText());
//    if(ui->captureResolutionBox->currentText() == "Raspberry Camera v1") {
//        settings.setValue("capture/captureResX", 2592);
//        settings.setValue("capture/captureResY", 1944);
//        ui->cropResolutionX->setMaximum(2592);
//        ui->cropResolutionY->setMaximum(1944);
//    } else if(ui->captureResolutionBox->currentText() == "Raspberry Camera v2") {
//        settings.setValue("capture/captureResX", 3280);
//        settings.setValue("capture/captureResY", 2464);
//        ui->cropResolutionX->setMaximum(3280);
//        ui->cropResolutionY->setMaximum(2464);
//    }
}

SettingsDialog::~SettingsDialog()
{
//    qDebug() << "Saving settings...";
//    settings.setValue("capture/cropResX", ui->cropResolutionX->value());
//    settings.setValue("capture/cropResY", ui->cropResolutionY->value());
//    settings.setValue("satellite/rect_x", 10);
//    settings.setValue("satellite/rect_y", 10);
//    settings.setValue("satellite/rect_r", 5);
//    settings.sync();
    settings.setValue("capture/camUrl", ui->camAddressInput->text());
    settings.sync();

    delete ui;
}
