#include "mimage.h"
#include <QDebug>

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

MGrayImage MGrayImage::loadFromFile(const char *filename) {
    cv::Mat cvimg = cv::imread(filename, cv::IMREAD_GRAYSCALE);
    MGrayImage img(cvimg.rows, cvimg.cols);

    for(unsigned int i=0; i<cvimg.rows*cvimg.cols; i++) {
        img.getData()[i] = static_cast<double>(cvimg.data[i]);
    }

    return img;
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

double MGrayImage::getMax() const {
    double val=0.0;
    for(unsigned int i=0; i<n0*n1; i++)
        if(val<getAt(i)) val = getAt(i);
    return val;
}

double MGrayImage::getMin() const {
    double val=0.0;
    for(unsigned int i=0; i<n0*n1; i++)
        if(val>getAt(i)) val = getAt(i);
    return val;
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

void MGrayImage::normalize() {
    double minval=0.0, maxval=1.0;

    for(auto it : data) {
        if(it > maxval) maxval = it;
        if(it < minval) minval = it;
    }
    //qDebug() << "Max: " << maxval << " Min: " << minval;
    for(unsigned int i=0; i<getWidth()*getHeight(); i++) {
        if(minval < 0.0) {
            getData()[i] = (getAt(i)+abs(minval)) / (maxval+abs(minval));
        } else {
            getData()[i] = (getAt(i)-minval) / (maxval-minval);
        }
    }
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

MGrayImage MGrayImage::operator=(const MGrayImage &b) {
    return MGrayImage(b);
}


MGrayImage MGrayImage::operator+(const double b) const {
    MGrayImage img(*this);

    for(unsigned int i=0; i<img.getWidth()*img.getHeight(); i++)
        img.getData()[i] = getAt(i) + b;

    return img;
}

MGrayImage MGrayImage::operator-(const double b) const {
    MGrayImage img(*this);

    for(unsigned int i=0; i<img.getWidth()*img.getHeight(); i++)
        img.getData()[i] = getAt(i) - b;

    return img;
}

MGrayImage MGrayImage::operator*(const double b) const {
    MGrayImage img(*this);

    for(unsigned int i=0; i<img.getWidth()*img.getHeight(); i++)
        img.getData()[i] = getAt(i) * b;

    return img;
}

MGrayImage MGrayImage::operator/(const double b) const {
    MGrayImage img(*this);

    for(unsigned int i=0; i<img.getWidth()*img.getHeight(); i++)
        img.getData()[i] = getAt(i) / b;

    return img;
}

MGrayImage MGrayImage::operator+=(const double b) {
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] = getAt(i) + b;

    return *this;
}
MGrayImage MGrayImage::operator-=(const double b) {
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] = getAt(i) - b;

    return *this;
}
MGrayImage MGrayImage::operator*=(const double b) {
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] = getAt(i) * b;

    return *this;
}
MGrayImage MGrayImage::operator/=(const double b) {
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] = getAt(i) / b;

    return *this;
}


QImage MGrayImage::asQImage() const {
    QImage img(n1, n0, QImage::Format_Grayscale8);

    for(unsigned int i=0; i<n0*n1; i++)
        img.bits()[i] = (getAt(i));

    return img;
}
/*
MComplexImage MGrayImage::asComplex() const {
    MComplexImage cimg(n0, n1);
    cimg.initValue(std::complex<double>(0.0, 0.0));

    for(unsigned int i=0; i<n0*n1; i++)
        cimg.getData()[i].real(getAt(i));

    return cimg;
}*/
