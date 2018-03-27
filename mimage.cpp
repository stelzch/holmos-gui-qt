#include "mimage.h"

MGrayImage::MGrayImage(QObject *parent, unsigned int n0, unsigned int n1) : QObject(parent), n0(n0), n1(n1)
{
    data.reserve(n0 * n1);
}

MGrayImage::MGrayImage(const MGrayImage &src)  : QObject() {
    n0 = src.getHeight();
    n1 = src.getWidth();
    data = src.data;
}

double *MGrayImage::getData() {
    return data.data();
}

unsigned int MGrayImage::getWidth() const {
    return n1;
}
unsigned int MGrayImage::getHeight() const {
    return n0;
}

void MGrayImage::sineInplace() {
    for(unsigned int i=0; i<n0*n1; i++)
        data[i] = sin(data[i]);
}

void MGrayImage::cosineInplace() {
    for(unsigned int i=0; i<n0*n1; i++)
        data[i] = cos(data[i]);
}

MGrayImage MGrayImage::sine() const {
    MGrayImage sineImg(*this);
    sineImg.sineInplace();

    return sineImg;
}

MGrayImage MGrayImage::cosine() const {
    MGrayImage cosineImg(*this);
    cosineImg.cosineInplace();

    return cosineImg;
}
