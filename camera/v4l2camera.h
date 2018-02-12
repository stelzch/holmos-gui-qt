#ifndef V4L2CAMERA_H
#define V4L2CAMERA_H

#include "camera.h"
#include "image.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <exception>
#include <QDebug>

class V4L2Camera : public Camera
{
public:
    V4L2Camera();
    virtual void set_resolution(int res_x, int res_y) override;
    virtual void open() override;
    virtual void release() override;
    virtual Image<float> capture() override;
    virtual ~V4L2Camera();
private:
    cv::VideoCapture cap;
    cv::Mat frame;
    Image<float> image;
};

#endif // V4L2CAMERA_H
