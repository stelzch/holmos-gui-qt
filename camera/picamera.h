#ifndef PICAMERA_H
#define PICAMERA_H

#include "camera.h"
#include "raspicam/raspicam.h"
#include <QDebug>

class PiCamera : public Camera
{
public:
    PiCamera();
    void set_resolution(int res_x, int res_y);
    void open();
    void release();
    Image<float> &capture();
    void captureInplace(Image<float> *img);
    virtual ~PiCamera() {}

private:
    raspicam::RaspiCam picam;
    Image<float> frame;
    unsigned char *buffer;
};

#endif // PICAMERA_H
