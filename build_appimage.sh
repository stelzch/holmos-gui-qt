#!/bin/sh
APPDIR=Holmos_Viewer-x86_64.AppDir
EXEC=gui-qt
LDQT=./linuxdeployqt
LDQT_URL=https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
AIT=./appimagetool-x86_64.AppImage
AIT_URL=https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
if [ -d $APPDIR ]; then
    echo Removing $APPDIR...
    rm -rvf $APPDIR
fi

if [ ! -f $EXEC ]; then
    echo You must build gui-qt first...
    exit -1
fi

mkdir -p $APPDIR/usr/share/applications $APPDIR/usr/share/icons/hicolor/256x256/apps $APPDIR/usr/bin
cp holmos_viewer.desktop $APPDIR/usr/share/applications
cp gui-qt $APPDIR/usr/bin/holmos-viewer
cp icon.png $APPDIR/usr/share/icons/hicolor/256x256/apps/holmosviewer.png
cp icon.png $APPDIR/holmos-viewer.png

if [ ! -f $LDQT ]; then
    wget -O $LDQT $LDQT_URL 
    chmod +x $LDQT
fi

if [ ! -f $AIT ]; then
    wget -O $AIT $AIT_URL
    chmod +x $AIT
fi

$LDQT $APPDIR/usr/share/applications/holmos_viewer.desktop
$AIT $APPDIR
