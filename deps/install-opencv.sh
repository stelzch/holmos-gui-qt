#!/bin/sh
VERSION=3.4.1
wget -c https://github.com/opencv/opencv/archive/$VERSION.tar.gz

tar xf $VERSION.tar.gz

mkdir build
cd build

cmake -D BUILD_SHARED_LIBS=OFF -D WITH_OPENMP=ON \
    -D CMAKE_BUILD_TYPE=RELEASE -D BUILD_EXAMPLES=OFF -D BUILD_DOCS=OFF \
    -D BUILD_PERF_TESTS=OFF -D BUILD_TESTS=OFF -D BUILD_ITT=OFF \
    -D BUILD_JAVA=OFF -D BUILD_IPP_IW=OFF -D BUILD_opencv_apps=OFF \
    -D BUILD_opencv_calib3d=OFF -D BUILD_opencv_dnn=OFF -D BUILD_opencv_features2d=OFF \
    -D BUILD_opencv_flann=OFF -D BUILD_opencv_highgui=OFF \
    -D BUILD_opencv_java_bindings_gen=OFF -D BUILD_opencv_js=OFF \
    -D BUILD_opencv_ml=OFF -D BUILD_opencv_objdetect=OFF \
    -D BUILD_opencv_stitching=OFF -D BUILD_opencv_superres=OFF \
    -D BUILD_opencv_ts=OFF -D BUILD_opencv_imgproc=OFF -D BUILD_opencv_viz=OFF -D ENABLE_CXX11=ON \
    ../opencv-$VERSION

make -j$(nproc)

if [ `id -g` -eq 0 ]; then
    make install
else
    sudo make install
fi
