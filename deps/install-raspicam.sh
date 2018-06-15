#!/bin/sh
git clone https://github.com/cedricve/raspicam
mkdir raspicam-build
cd raspicam-build
cmake ../raspicam
make
make install

