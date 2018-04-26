#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QDebug>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

public slots:
    void captureResolutionChanged(int);

private:
    Ui::SettingsDialog *ui;
    QSettings settings;
};

#endif // SETTINGSDIALOG_H
