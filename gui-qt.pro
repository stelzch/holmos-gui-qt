#-------------------------------------------------
#
# Project created by QtCreator 2018-02-05T18:28:03
#
#-------------------------------------------------

QT       += core gui testlib network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    mainwindow.cpp \
    computationworker.cpp \
    imageselect.cpp \
    scaledimageview.cpp \
    fpscounter.cpp \
    settingsdialog.cpp \
    imageretriever.cpp \
    exceptions.cpp \
    mopenglwidget.cpp

HEADERS  += mainwindow.h \
    computationworker.h \
    imageselect.h \
    scaledimageview.h \
    fpscounter.h \
    settingsdialog.h \
    imageretriever.h \
    exceptions.h \
    mopenglwidget.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

test {
    CONFIG += debug
    TARGET = gui-qt-tests

    TEMPLATE = app
    message(Configuring test build...)
    SOURCES += test/testmimage.cpp
} else {
    TARGET = gui-qt
    TEMPLATE = app
    message(Normal build...)
    SOURCES += main.cpp

}

CONFIG += link_pkgconfig
CONFIG += c++14
CONFIG += testcase
unix: LIBS += `pkg-config --static --libs opencv fftw3f`
unix: QMAKE_CXXFLAGS += -fopenmp
unix: QMAKE_LFLAGS += -fopenmp
DISTFILES += \
    simple.vsh \
    simple.fsh \
    simple.vsh

RESOURCES += \
    res.qrc
