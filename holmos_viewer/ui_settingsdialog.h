/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label_3;
    QLineEdit *camAddressInput;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout;
    QSpinBox *cropResolutionX;
    QSpinBox *cropResolutionY;
    QCheckBox *checkboxThreeD;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName(QStringLiteral("SettingsDialog"));
        SettingsDialog->resize(640, 480);
        verticalLayout = new QVBoxLayout(SettingsDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        groupBox = new QGroupBox(SettingsDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        formLayout = new QFormLayout(groupBox);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_3);

        camAddressInput = new QLineEdit(groupBox);
        camAddressInput->setObjectName(QStringLiteral("camAddressInput"));

        formLayout->setWidget(0, QFormLayout::FieldRole, camAddressInput);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setToolTipDuration(2);

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        cropResolutionX = new QSpinBox(groupBox);
        cropResolutionX->setObjectName(QStringLiteral("cropResolutionX"));
        cropResolutionX->setMinimum(-1);
        cropResolutionX->setMaximum(16000);

        horizontalLayout->addWidget(cropResolutionX);

        cropResolutionY = new QSpinBox(groupBox);
        cropResolutionY->setObjectName(QStringLiteral("cropResolutionY"));
        cropResolutionY->setMinimum(-1);
        cropResolutionY->setMaximum(16000);

        horizontalLayout->addWidget(cropResolutionY);


        formLayout->setLayout(2, QFormLayout::FieldRole, horizontalLayout);

        checkboxThreeD = new QCheckBox(groupBox);
        checkboxThreeD->setObjectName(QStringLiteral("checkboxThreeD"));

        formLayout->setWidget(3, QFormLayout::LabelRole, checkboxThreeD);


        verticalLayout->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(SettingsDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(SettingsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SettingsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SettingsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QApplication::translate("SettingsDialog", "Dialog", 0));
        groupBox->setTitle(QApplication::translate("SettingsDialog", "Capture parameters", 0));
        label_3->setText(QApplication::translate("SettingsDialog", "Cam-Server", 0));
#ifndef QT_NO_TOOLTIP
        label_2->setToolTip(QApplication::translate("SettingsDialog", "<html><head/><body><p>Allows you to crop the input image to a smaller one, to avoid unsufficient memory issues.</p><p>A value of <span style=\" font-style:italic;\">-1</span> means no cropping in that direction, any other positive value signifies the new width/height of the processed image.</p></body></html>", 0));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("SettingsDialog", "Crop resolution", 0));
        checkboxThreeD->setText(QApplication::translate("SettingsDialog", "Enable 3D", 0));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
