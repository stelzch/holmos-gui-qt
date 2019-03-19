QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testimglib.cpp


TARGET = test_imglib

unix:!macx: LIBS += -L$$OUT_PWD/../holmos_imglib/ -lholmos_imglib

INCLUDEPATH += $$PWD/../holmos_imglib
DEPENDPATH += $$PWD/../holmos_imglib

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../holmos_imglib/libholmos_imglib.a

LIBS += `pkg-config --static --libs opencv`
CONFIG += c++14
QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp
