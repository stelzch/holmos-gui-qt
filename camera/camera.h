#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

class Camera {
public:
    virtual void set_resolution(int res_x, int res_y) = 0;
    virtual void open() = 0;
    virtual void release() = 0;
    virtual std::vector<unsigned char> capture() = 0;
    virtual ~Camera() {}

protected:
    int res_x, res_y;
};

#endif // CAMERA_H
