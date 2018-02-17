#include "picamera.h"

PiCamera::PiCamera()
{
    res_x = 0;
    res_y = 0;
}

void PiCamera::set_resolution(int res_x, int res_y) {
    this->res_x = res_x;
    this->res_y = res_y;
    picam.setWidth(res_x);
    picam.setHeight(res_y);
    frame.resize(res_x, res_y, 3);
    buffer = new unsigned char[res_x*res_y*3];
}

void PiCamera::open() {
    picam.setFormat(raspicam::RASPICAM_FORMAT_RGB);
    picam.open();
}

void PiCamera::release() {
    picam.release();
    free(buffer);
}

Image<float> &PiCamera::capture() {
    qDebug() << picam.getWidth() << picam.getHeight();
    assert(res_x == picam.getWidth());
    assert(res_y == picam.getHeight());
    picam.grab();

    picam.retrieve(buffer);
    for(int i=0; i<res_x*res_y*3; i++) {
        frame.modifyVector()[i] = buffer[i];
    }

    return frame;
}

void PiCamera::captureInplace(Image<float> *img) {
    picam.grab();
    img->resize(res_x, res_y, 3);
    picam.retrieve(buffer);
    for(int i=0; i<res_x*res_y*3; i++) {
        frame.modifyVector()[i] = buffer[i];
    }
}
