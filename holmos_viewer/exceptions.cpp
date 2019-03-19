#include "exceptions.h"

ImageRetrivalException::ImageRetrivalException(QString msg) {
    this->msg = msg;
}

void ImageRetrivalException::raise() const {
    throw *this;
}

ImageRetrivalException *ImageRetrivalException::clone() const {
    return new ImageRetrivalException(*this);
}
