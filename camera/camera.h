#ifndef CAMERA_H
#define CAMERA_H

#include <exception>
#include <string>
#include <stdexcept>
#include "image.h"

class Camera {
public:
    virtual void set_resolution(int res_x, int res_y) = 0;
    virtual void open() = 0;
    virtual void release() = 0;
    virtual Image<float> capture() = 0;
    virtual ~Camera() {}

protected:
    int res_x, res_y;
};

class CameraNotFoundException : public std::runtime_error {
public:
    explicit CameraNotFoundException(const std::string& what_arg) : std::runtime_error(what_arg) {}
};

#endif // CAMERA_H
