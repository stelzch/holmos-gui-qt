/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSettings;
    QAction *actionCompute;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QHBoxLayout *horizontalLayout_2;
    QScrollArea *scrollAreaTab1;
    QWidget *scrollAreaWidgetContents;
    QHBoxLayout *horizontalLayout_3;
    QLabel *cameraImage;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QSlider *sliderRectX;
    QSlider *sliderRectY;
    QLabel *label_2;
    QLabel *label_3;
    QSlider *sliderRectR;
    QScrollArea *scrollAreaTab2;
    QWidget *scrollAreaWidgetContents_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *magnitudeImage;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *phaseUnwrapCheck;
    QScrollArea *scrollArea_3;
    QWidget *scrollAreaWidgetContents_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *phaseAngle;
    QWidget *tab_4;
    QHBoxLayout *horizontalLayout_6;
    QHBoxLayout *horizontalLayout_8;
    QVBoxLayout *verticalLayout_4;
    QSlider *heightScale;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(825, 468);
        actionSettings = new QAction(MainWindow);
        actionSettings->setObjectName(QStringLiteral("actionSettings"));
        actionCompute = new QAction(MainWindow);
        actionCompute->setObjectName(QStringLiteral("actionCompute"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        horizontalLayout_2 = new QHBoxLayout(tab);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        scrollAreaTab1 = new QScrollArea(tab);
        scrollAreaTab1->setObjectName(QStringLiteral("scrollAreaTab1"));
        scrollAreaTab1->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 783, 332));
        horizontalLayout_3 = new QHBoxLayout(scrollAreaWidgetContents);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        cameraImage = new QLabel(scrollAreaWidgetContents);
        cameraImage->setObjectName(QStringLiteral("cameraImage"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(cameraImage->sizePolicy().hasHeightForWidth());
        cameraImage->setSizePolicy(sizePolicy);
        cameraImage->setScaledContents(true);

        horizontalLayout_3->addWidget(cameraImage);

        scrollAreaTab1->setWidget(scrollAreaWidgetContents);

        horizontalLayout_2->addWidget(scrollAreaTab1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout = new QVBoxLayout(tab_2);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label = new QLabel(tab_2);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        sliderRectX = new QSlider(tab_2);
        sliderRectX->setObjectName(QStringLiteral("sliderRectX"));
        sliderRectX->setMaximum(1024);
        sliderRectX->setOrientation(Qt::Horizontal);

        formLayout->setWidget(0, QFormLayout::FieldRole, sliderRectX);

        sliderRectY = new QSlider(tab_2);
        sliderRectY->setObjectName(QStringLiteral("sliderRectY"));
        sliderRectY->setMaximum(1024);
        sliderRectY->setOrientation(Qt::Horizontal);

        formLayout->setWidget(1, QFormLayout::FieldRole, sliderRectY);

        label_2 = new QLabel(tab_2);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        label_3 = new QLabel(tab_2);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        sliderRectR = new QSlider(tab_2);
        sliderRectR->setObjectName(QStringLiteral("sliderRectR"));
        sliderRectR->setMaximum(100);
        sliderRectR->setOrientation(Qt::Horizontal);

        formLayout->setWidget(2, QFormLayout::FieldRole, sliderRectR);


        verticalLayout->addLayout(formLayout);

        scrollAreaTab2 = new QScrollArea(tab_2);
        scrollAreaTab2->setObjectName(QStringLiteral("scrollAreaTab2"));
        scrollAreaTab2->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QStringLiteral("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 783, 255));
        horizontalLayout_4 = new QHBoxLayout(scrollAreaWidgetContents_2);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        magnitudeImage = new QLabel(scrollAreaWidgetContents_2);
        magnitudeImage->setObjectName(QStringLiteral("magnitudeImage"));
        sizePolicy.setHeightForWidth(magnitudeImage->sizePolicy().hasHeightForWidth());
        magnitudeImage->setSizePolicy(sizePolicy);
        magnitudeImage->setScaledContents(true);

        horizontalLayout_4->addWidget(magnitudeImage);

        scrollAreaTab2->setWidget(scrollAreaWidgetContents_2);

        verticalLayout->addWidget(scrollAreaTab2);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        verticalLayout_2 = new QVBoxLayout(tab_3);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        phaseUnwrapCheck = new QCheckBox(tab_3);
        phaseUnwrapCheck->setObjectName(QStringLiteral("phaseUnwrapCheck"));

        verticalLayout_3->addWidget(phaseUnwrapCheck);


        verticalLayout_2->addLayout(verticalLayout_3);

        scrollArea_3 = new QScrollArea(tab_3);
        scrollArea_3->setObjectName(QStringLiteral("scrollArea_3"));
        scrollArea_3->setWidgetResizable(true);
        scrollAreaWidgetContents_3 = new QWidget();
        scrollAreaWidgetContents_3->setObjectName(QStringLiteral("scrollAreaWidgetContents_3"));
        scrollAreaWidgetContents_3->setGeometry(QRect(0, 0, 783, 299));
        horizontalLayout_5 = new QHBoxLayout(scrollAreaWidgetContents_3);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        phaseAngle = new QLabel(scrollAreaWidgetContents_3);
        phaseAngle->setObjectName(QStringLiteral("phaseAngle"));
        phaseAngle->setScaledContents(true);

        horizontalLayout_5->addWidget(phaseAngle);

        scrollArea_3->setWidget(scrollAreaWidgetContents_3);

        verticalLayout_2->addWidget(scrollArea_3);

        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        horizontalLayout_6 = new QHBoxLayout(tab_4);
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));

        horizontalLayout_8->addLayout(verticalLayout_4);


        horizontalLayout_6->addLayout(horizontalLayout_8);

        heightScale = new QSlider(tab_4);
        heightScale->setObjectName(QStringLiteral("heightScale"));
        heightScale->setMaximum(1024);
        heightScale->setOrientation(Qt::Vertical);

        horizontalLayout_6->addWidget(heightScale);

        tabWidget->addTab(tab_4, QString());

        horizontalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 825, 24));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        MainWindow->insertToolBarBreak(mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionSettings);
        mainToolBar->addSeparator();

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        actionSettings->setText(QApplication::translate("MainWindow", "Settings", 0));
        actionCompute->setText(QApplication::translate("MainWindow", "Compute", 0));
#ifndef QT_NO_TOOLTIP
        actionCompute->setToolTip(QApplication::translate("MainWindow", "Start/Stop computation", 0));
#endif // QT_NO_TOOLTIP
        actionCompute->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0));
        cameraImage->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Camera", 0));
        label->setText(QApplication::translate("MainWindow", "Rect X", 0));
        label_2->setText(QApplication::translate("MainWindow", "Rect Y", 0));
        label_3->setText(QApplication::translate("MainWindow", "Rect R", 0));
        magnitudeImage->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Fourier", 0));
        phaseUnwrapCheck->setText(QApplication::translate("MainWindow", "Unwrap phase", 0));
        phaseAngle->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "Phase", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("MainWindow", "Pseudo-3D", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
