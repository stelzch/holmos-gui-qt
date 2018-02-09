#include "v4l2camera.h"

V4L2Camera::V4L2Camera()
{
    res_x = res_y = 0;
}

void V4L2Camera::open() {
    /** TODO: Fix default camera number */
    cap.open(0);
    if(cap.isOpened() == false)
        qDebug() << "[ERROR] No camera found";
}

void V4L2Camera::release() {
    cap.release();
}

void V4L2Camera::set_resolution(int res_x, int res_y) {
    this->res_x = res_x;
    this->res_y = res_y;

    cap.set(CV_CAP_PROP_FRAME_WIDTH, res_x);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, res_y);
}

std::vector<unsigned char> V4L2Camera::capture() {

    cap.grab();
    cap.retrieve(frame);
    qDebug() << frame.cols << frame.rows;
    qDebug() << res_x << res_y;
    assert(frame.cols == res_x);
    assert(frame.rows == res_y);

    std::vector<unsigned char> tmp;
    tmp.reserve(res_x * res_y * 3);
    memcpy(tmp.data(), frame.ptr(), res_x * res_y * 3 * sizeof(uchar));

    return tmp;
}

V4L2Camera::~V4L2Camera() {

}
