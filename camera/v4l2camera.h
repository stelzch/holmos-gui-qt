#ifndef V4L2CAMERA_H
#define V4L2CAMERA_H

#include "camera.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <QDebug>

class V4L2Camera : public Camera
{
public:
    V4L2Camera();
    virtual void set_resolution(int res_x, int res_y) override;
    virtual void open() override;
    virtual void release() override;
    virtual std::vector<unsigned char> capture() override;
    virtual ~V4L2Camera();
private:
    cv::VideoCapture cap;
    cv::Mat frame;
};

#endif // V4L2CAMERA_H
