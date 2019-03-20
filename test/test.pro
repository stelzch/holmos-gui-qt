QT += testlib

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testimglib.cpp


TARGET = test_imglib

LIBS += `pkg-config --static --libs opencv fftw3 fftw3f`
CONFIG += c++14
QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp

RESOURCES += \
    testdata.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../holmos_imglib/release/ -lholmos_imglib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../holmos_imglib/debug/ -lholmos_imglib
else:unix: LIBS += -L$$OUT_PWD/../holmos_imglib/ -lholmos_imglib

INCLUDEPATH += $$PWD/../holmos_imglib
DEPENDPATH += $$PWD/../holmos_imglib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../holmos_imglib/release/libholmos_imglib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../holmos_imglib/debug/libholmos_imglib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../holmos_imglib/release/holmos_imglib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../holmos_imglib/debug/holmos_imglib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../holmos_imglib/libholmos_imglib.a
