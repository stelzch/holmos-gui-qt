# gui-qt
Used on the raspberry to analyse the image.

## Installation
This program has several dependencies:

  * [OpenCV](https://opencv.org) with C++11-support for lambda expressions
  * [raspicam](https://www.uco.es/investiga/grupos/ava/node/40) library
  * [fftw](http://fftw.org/)
  * Qt5

Qt and fftw can be installed from debian packages:
```
sudo apt update && \
sudo apt install qt5-default qmake libfftw3-dev
```

There is an OpenCV version in the debian repository, however it is really old and does not support the shiny new stuff this program uses. In addition, if OpenCV is compiled by hand on the raspberry, we can activate several optimizations to improve performance with ARM Neon.

Install the debian packages provided with the latest release using
```
dpkg -i OpenCV*.deb
```

Compiling raspicam is pretty straightforward, the detailed instructions can be followed on their homepage.

When all these requirements are obtained, you can clone this git repo and issue the following commands to compile the software:
```
qmake CONFIG+=rpicam_v2 &&
make -j2 &&
sudo make install
```
