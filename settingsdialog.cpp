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
    ui->checkboxThreeD->setChecked(settings.value("misc/enable_3d", false).value<bool>());
}

SettingsDialog::~SettingsDialog()
{
    settings.setValue("capture/camUrl", ui->camAddressInput->text());
    settings.setValue("capture/cropX", ui->cropResolutionX->value());
    settings.setValue("capture/cropY", ui->cropResolutionY->value());
    settings.setValue("misc/enable_3d", ui->checkboxThreeD->isChecked());
    settings.sync();

    delete ui;
}
