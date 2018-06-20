#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    ui->camAddressInput->setText(settings.value("capture/camUrl").value<QString>());
    ui->cropResolutionX->setValue(settings.value("capture/cropX", -1).value<int>());
    ui->cropResolutionY->setValue(settings.value("capture/cropY", -1).value<int>());
}

SettingsDialog::~SettingsDialog()
{
    settings.setValue("capture/camUrl", ui->camAddressInput->text());
    settings.setValue("capture/cropX", ui->cropResolutionX->value());
    settings.setValue("capture/cropY", ui->cropResolutionY->value());
    settings.sync();

    delete ui;
}
