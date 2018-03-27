#include "mimage.h"

MGrayImage::MGrayImage(unsigned int n0, unsigned int n1, QObject *parent) : QObject(parent), n0(n0), n1(n1)
{
    data.resize(n0 * n1);
    zeros();
}

MGrayImage::MGrayImage(const MGrayImage &src)  : QObject() {
    n0 = src.getHeight();
    n1 = src.getWidth();
    data = src.data;
}

double *MGrayImage::getData() {
    return data.data();
}

double MGrayImage::getAt(unsigned int i) const {
    return data.at(i);
}
unsigned int MGrayImage::getWidth() const {
    return n1;
}
unsigned int MGrayImage::getHeight() const {
    return n0;
}

void MGrayImage::sineInplace() {
    for(unsigned int i=0; i<n0*n1; i++)
        data.data()[i] = sin(getAt(i));
}

void MGrayImage::cosineInplace() {
    for(unsigned int i=0; i<n0*n1; i++)
        data.data()[i] = cos(data[i]);
}

MGrayImage MGrayImage::sine() const {
    MGrayImage sineImg = *this;
    sineImg.sineInplace();

    return sineImg;
}

MGrayImage MGrayImage::cosine() const {
    MGrayImage cosineImg = *this;
    cosineImg.cosineInplace();

    return cosineImg;
}

void MGrayImage::zeros() {
    initValue(0.0);
}
void MGrayImage::initValue(double x) {
    for(unsigned int i=0; i<getWidth() * getHeight(); i++)
        getData()[i] = x;
}

/* ==== OPERATORS ======= */

MGrayImage MGrayImage::operator+(const MGrayImage &b) const {
    unsigned int new_width = min(getWidth(), b.getWidth());
    unsigned int new_height = min(getHeight(), b.getHeight());

    MGrayImage sum(new_height, new_width);

    for(unsigned int y=0; y<new_height; y++) {
        for(unsigned int x=0; x<new_width; x++) {
            sum.getData()[y*new_width+x] = getAt(y*getWidth()+x) +
                b.getAt(y*b.getWidth()+x);
        }
    }

    return sum;
}
MGrayImage MGrayImage::operator-(const MGrayImage &b) const {
    unsigned int new_width = min(getWidth(), b.getWidth());
    unsigned int new_height = min(getHeight(), b.getHeight());

    MGrayImage diff(new_height, new_width);

    for(unsigned int y=0; y<new_height; y++) {
        for(unsigned int x=0; x<new_width; x++) {
            diff.getData()[y*new_width+x] = getAt(y*getWidth()+x) -
                b.getAt(y*b.getWidth()+x);
        }
    }

    return diff;
}
MGrayImage MGrayImage::operator*(const MGrayImage &b) const {
    unsigned int new_width = min(getWidth(), b.getWidth());
    unsigned int new_height = min(getHeight(), b.getHeight());

    MGrayImage mul(new_height, new_width);

    for(unsigned int y=0; y<new_height; y++) {
        for(unsigned int x=0; x<new_width; x++) {
            mul.getData()[y*new_width+x] = getAt(y*getWidth()+x) *
                b.getAt(y*b.getWidth()+x);
        }
    }

    return mul;
}
MGrayImage MGrayImage::operator/(const MGrayImage &b) const {
    unsigned int new_width = min(getWidth(), b.getWidth());
    unsigned int new_height = min(getHeight(), b.getHeight());

    MGrayImage div(new_height, new_width);

    for(unsigned int y=0; y<new_height; y++) {
        for(unsigned int x=0; x<new_width; x++) {
            div.getData()[y*new_width+x] = getAt(y*getWidth()+x) /
                b.getAt(y*b.getWidth()+x);
        }
    }

    return div;
}
