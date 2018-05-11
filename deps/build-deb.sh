#!/bin/sh

BINARY=$1

rm -r tmp
mkdir tmp
mkdir -p tmp/usr/bin
cp $BINARY tmp/usr/bin/holmos-viewer
INSTALLED_SIZE=$(du -s tmp/usr | cut -f1)

cd tmp
tar cvf data.tar ./usr
gzip data.tar

cat > control << EOF
Package: holmos-viewer
Version: 0.1
Section: science
Priority: optional
Architecture: armhf
Depends: libfftw3-bin, libfftw3-single3, libqt5core5a, libqt5gui5,libqt5widgets5, libqt5test5
Maintainer: Christoph Stelz <mail@ch-st.de>
Description: Imaging Program for the HolMOS project
Installed-Size: $INSTALLED_SIZE
EOF

rm md5sums
touch md5sums
find usr -type f -exec md5sum {} >> md5sums +

tar cvf control.tar control md5sums
gzip control.tar

echo "2.0\n" > debian-binary

ar r holmos-viewer.deb debian-binary control.tar.gz data.tar.gz
mv holmos-viewer.deb ..
rm -r tmp
