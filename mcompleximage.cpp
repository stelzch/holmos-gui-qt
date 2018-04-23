#include "mcompleximage.h"

#include <QDebug>

MComplexImage::MComplexImage(unsigned int n0, unsigned int n1,
                             QObject *parent) : QObject(parent), n0(n0), n1(n1)
{
    data.resize(n0 * n1);
    initValue(std::complex<double>(0.0, 0.0));

}

MComplexImage::MComplexImage(const MComplexImage &src) : QObject() {
    n0 = src.getHeight();
    n1 = src.getWidth();
    data = src.data;
}

MComplexImage::MComplexImage(const MGrayImage &src) : QObject() {
    n0 = src.getHeight();
    n1 = src.getWidth();
    data.reserve(n0 * n1);

 
    for(unsigned int i=0; i<n0*n1; i++) {
        data.at(i).real(src.getAt(i));
        data.at(i).imag(0.0);
    }
}

MComplexImage MComplexImage::fromGrayImage(MGrayImage& img) {
    MComplexImage cimg(img.getHeight(), img.getWidth());
    cimg.initValue(std::complex<double>(0.0, 0.0));

     
    for(unsigned int i=0; i<img.getWidth()*img.getHeight(); i++)
        cimg.getData()[i].real(img.getAt(i));

    return cimg;
}

void MComplexImage::loadGrayImage(MGrayImage img) {
    assert(img.getWidth() == getWidth());
    assert(img.getHeight() == getHeight());
    initValue(std::complex<double>(0.0, 0.0));

 
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] = img.getAt(i);
}

std::complex<double> *MComplexImage::getData() {
    return data.data();
}

std::complex<double> MComplexImage::getAt(unsigned int i) const {
    return data.at(i);
}

std::complex<double> MComplexImage::getAt(unsigned int x, unsigned int y) const {
    return data.at(y*n1+x);
}

unsigned int MComplexImage::getWidth() const {
    return n1;
}

unsigned int MComplexImage::getHeight() const {
    return n0;
}

void MComplexImage::initValue(std::complex<double> val) {
     
    for(unsigned int i=0; i<getWidth() * getHeight(); i++)
        data.at(i) = val;
}

void MComplexImage::normalize() {
    double minval=0.0, maxval=1.0;


    for(auto it : data) {
        if(it.real() > maxval) maxval = it.real();
        if(it.real() < minval) minval = it.real();
    }

    if(minval<0.0) {
        *this = *this + minval;
        maxval += minval;
    } else {
        *this = *this - minval;
        maxval -= minval;
    }

    *this = *this / maxval;
}

void MComplexImage::fftshift() {
    unsigned int yhalf = getHeight() / 2;
    unsigned int xhalf = getWidth() / 2;

    auto buffer = getData();

    /* Swap upper-left with bottom-right */
     
    for(int y=0; y<yhalf; y++) {
        for(int x=0; x<xhalf; x++) {
            std::complex<double> tmp;
            int coord_upper_left = y*n1+x;
            int coord_bottom_right = (y+yhalf)*n1+x+xhalf;
            tmp = buffer[coord_upper_left];
            buffer[coord_upper_left] = buffer[coord_bottom_right];
            buffer[coord_bottom_right] = tmp;
        }
    }

    /* Swap bottom-left with upper-right */
     
    for(int y=yhalf; y<n0; y++) {
        for(int x=0; x<xhalf; x++) {
            std::complex<double> tmp;
            int coord_bottom_left = y*n1+x;
            int coord_upper_right = (y-yhalf)*n1+x+xhalf;
            tmp = buffer[coord_bottom_left];
            buffer[coord_bottom_left] = buffer[coord_upper_right];
            buffer[coord_upper_right] = tmp;
        }
    }
}

MComplexImage MComplexImage::operator+(const MGrayImage &b) const {
    unsigned int new_width = min(getWidth(), b.getWidth());
    unsigned int new_height = min(getHeight(), b.getHeight());

    MComplexImage sum(new_height, new_width);

     
    for(unsigned int y=0; y<new_height; y++) {
        for(unsigned int x=0; x<new_width; x++) {
            sum.getData()[y*new_width+x].real(getAt(y*getWidth()+x).real() +
                    b.getAt(y*b.getWidth()+x));
            sum.getData()[y*new_width+x].imag(getAt(y*getWidth()+x).imag());
        }
    }

    return sum;
}

MComplexImage MComplexImage::operator-(const MGrayImage &b) const {
    unsigned int new_width = min(getWidth(), b.getWidth());
    unsigned int new_height = min(getHeight(), b.getHeight());

    MComplexImage diff(new_height, new_width);

     
    for(unsigned int y=0; y<new_height; y++) {
        for(unsigned int x=0; x<new_width; x++) {
            diff.getData()[y*new_width+x].real(getAt(y*getWidth()+x).real() -
                    b.getAt(y*b.getWidth()+x));
            diff.getData()[y*new_width+x].imag(getAt(y*getWidth()+x).imag());
        }
    }

    return diff;
}

MComplexImage MComplexImage::operator*(const MGrayImage &b) const  {
    unsigned int new_width = min(getWidth(), b.getWidth());
    unsigned int new_height = min(getHeight(), b.getHeight());

    MComplexImage mul(new_height, new_width);

     
    for(unsigned int y=0; y<new_height; y++) {
        for(unsigned int x=0; x<new_width; x++) {
            mul.getData()[y*new_width+x].real(getAt(y*getWidth()+x).real() *
                    b.getAt(y*b.getWidth()+x));
            mul.getData()[y*new_width+x].imag(getAt(y*getWidth()+x).imag() *
                    b.getAt(y*b.getWidth()+x));
        }
    }

    return mul;
}

MComplexImage MComplexImage::operator/(const MGrayImage &b) const {
    unsigned int new_width = min(getWidth(), b.getWidth());
    unsigned int new_height = min(getHeight(), b.getHeight());

    MComplexImage div(new_height, new_width);

     
    for(unsigned int y=0; y<new_height; y++) {
        for(unsigned int x=0; x<new_width; x++) {
            div.getData()[y*new_width+x].real(getAt(y*getWidth()+x).real() /
                    b.getAt(y*b.getWidth()+x));
            div.getData()[y*new_width+x].imag(getAt(y*getWidth()+x).imag() /
                    b.getAt(y*b.getWidth()+x));
        }
    }

    return div;
}

MComplexImage MComplexImage::operator+(const MComplexImage &b) const {
    unsigned int new_width = min(getWidth(), b.getWidth());
    unsigned int new_height = min(getHeight(), b.getHeight());

    MComplexImage sum(new_height, new_width);

     
    for(unsigned int y=0; y<new_height; y++) {
        for(unsigned int x=0; x<new_width; x++) {
            sum.getData()[y*new_width+x] = getAt(x, y) + b.getAt(x, y);
        }
    }

    return sum;
}

MComplexImage MComplexImage::operator-(const MComplexImage &b) const {
    unsigned int new_width = min(getWidth(), b.getWidth());
    unsigned int new_height = min(getHeight(), b.getHeight());

    MComplexImage diff(new_height, new_width);

     
    for(unsigned int y=0; y<new_height; y++) {
        for(unsigned int x=0; x<new_width; x++) {
            diff.getData()[y*new_width+x] = getAt(x, y) - b.getAt(x, y);
        }
    }

    return diff;
}

MComplexImage MComplexImage::operator*(const MComplexImage &b) const {
    unsigned int new_width = min(getWidth(), b.getWidth());
    unsigned int new_height = min(getHeight(), b.getHeight());

    MComplexImage mul(new_height, new_width);

     
    for(unsigned int y=0; y<new_height; y++) {
        for(unsigned int x=0; x<new_width; x++) {
            mul.getData()[y*new_width+x] = getAt(x, y) * b.getAt(x, y);
        }
    }

    return mul;
}

MComplexImage MComplexImage::operator/(const MComplexImage &b) const {
    unsigned int new_width = min(getWidth(), b.getWidth());
    unsigned int new_height = min(getHeight(), b.getHeight());

    MComplexImage div(new_height, new_width);

     
    for(unsigned int y=0; y<new_height; y++) {
        for(unsigned int x=0; x<new_width; x++) {
            div.getData()[y*new_width+x] = getAt(x, y) / b.getAt(x, y);
        }
    }

    return div;
}

MComplexImage MComplexImage::operator+(const double b) const {
    MComplexImage img(*this);

     
    for(unsigned int i=0; i<img.getWidth()*img.getHeight(); i++)
        img.getData()[i] = getAt(i) + b;

    return img;
}

MComplexImage MComplexImage::operator-(const double b) const {
    MComplexImage img(*this);

     
    for(unsigned int i=0; i<img.getWidth()*img.getHeight(); i++)
        img.getData()[i] = getAt(i) - b;

    return img;
}

MComplexImage MComplexImage::operator*(const double b) const {
    MComplexImage img(*this);

     
    for(unsigned int i=0; i<img.getWidth()*img.getHeight(); i++)
        img.getData()[i] = getAt(i) * b;

    return img;
}

MComplexImage MComplexImage::operator/(const double b) const {
    MComplexImage img(*this);

     
    for(unsigned int i=0; i<img.getWidth()*img.getHeight(); i++)
        img.getData()[i] = getAt(i) / b;

    return img;
}


MComplexImage MComplexImage::operator+=(const double b) {
     
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] = getAt(i) + b;

    return *this;
}

MComplexImage MComplexImage::operator-=(const double b) {
     
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] = getAt(i) - b;

    return *this;
}

MComplexImage MComplexImage::operator*=(const double b) {
     
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] = getAt(i) * b;

    return *this;
}

MComplexImage MComplexImage::operator/=(const double b) {
     
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] = getAt(i) / b;

    return *this;
}

MComplexImage MComplexImage::operator+=(const MGrayImage &b) {
    assert(b.getWidth() == getWidth());
    assert(b.getHeight() == getHeight());
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] = getAt(i) + b.getAt(i);

    return *this;
}

MComplexImage MComplexImage::operator-=(const MGrayImage &b) {
    assert(b.getWidth() == getWidth());
    assert(b.getHeight() == getHeight());
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] = getAt(i) - b.getAt(i);

    return *this;
}

MComplexImage MComplexImage::operator*=(const MGrayImage &b) {
    assert(b.getWidth() == getWidth());
    assert(b.getHeight() == getHeight());
     
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] = getAt(i) * b.getAt(i);

    return *this;
}

MComplexImage MComplexImage::operator/=(const MGrayImage &b) {
    assert(b.getWidth() == getWidth());
    assert(b.getHeight() == getHeight());
     
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] = getAt(i) / b.getAt(i);

    return *this;
}

MComplexImage MComplexImage::operator+=(const MComplexImage &b) {
    assert(b.getWidth() == getWidth());
    assert(b.getHeight() == getHeight());
     
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] += b.getAt(i);

    return *this;
}

MComplexImage MComplexImage::operator-=(const MComplexImage &b) {
    assert(b.getWidth() == getWidth());
    assert(b.getHeight() == getHeight());
     
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] -= b.getAt(i);

    return *this;
}

MComplexImage MComplexImage::operator*=(const MComplexImage &b) {
    assert(b.getWidth() == getWidth());
    assert(b.getHeight() == getHeight());
     
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] *= b.getAt(i);

    return *this;
}

MComplexImage MComplexImage::operator/=(const MComplexImage &b) {
    assert(b.getWidth() == getWidth());
    assert(b.getHeight() == getHeight());
     
    for(unsigned int i=0; i<getWidth()*getHeight(); i++)
        getData()[i] /= b.getAt(i);

    return *this;
}

MComplexImage MComplexImage::operator=(const MComplexImage &b) {
    return MComplexImage(b);
}

MGrayImage MComplexImage::getMagnitudeSpectrum() const {
    MGrayImage magspec(n0, n1);

     
    for(unsigned int i=0; i<n0*n1; i++) {
        magspec.getData()[i] = abs(getAt(i));
        //magspec.getData()[i] = pow(getAt(i).real(), 2) + pow(getAt(i).imag(), 2);
    }

    return magspec;
}

MGrayImage MComplexImage::getAngle() const {
    MGrayImage angle(n0, n1);

     
    for(unsigned int i=0; i<n0*n1; i++) {
        angle.getData()[i] = arg(getAt(i));
    }

    return angle;
}

MGrayImage MComplexImage::getReal() {
    MGrayImage real(n0, n1);

     
    for(unsigned int i=0; i<n0*n1; i++)
        real.getData()[i] = getAt(i).real();

    return real;
}

MComplexImage MComplexImage::getSector(unsigned int x, unsigned int y, unsigned int size) const {
    assert(y+size <= n0);
    assert(x+size <= n1);

    MComplexImage sector(size, size);

     
    for(unsigned int my=y; my<y+size; my++) {
        for(unsigned int mx=x; mx<x+size; mx++) {
            /* Sector coordinates */
            unsigned int sy = my - y;
            unsigned int sx = mx - x;

            sector.getData()[sy * size + sx] = getAt(mx, my);
        }
    }

    return sector;
}

void MComplexImage::setSector(unsigned int x, unsigned int y, MComplexImage &sector) {
    assert(y+sector.getHeight() <= n0);
    assert(x+sector.getWidth() <= n1);

     
    for(unsigned int my=y; my<y+sector.getHeight(); my++) {
        for(unsigned int mx=x; mx<x+sector.getWidth(); mx++) {
            /* Sector coordinates */
            unsigned sy = my - y;
            unsigned sx = mx - x;

            getData()[my*getWidth()+mx] = sector.getAt(sx, sy);
        }
    }
}

void MComplexImage::fftInplace() {
    fftw_complex *buf = fftw_alloc_complex(n0*n1);

     
    for(unsigned int i=0; i<n0*n1; i++) {
        buf[i][0] = getAt(i).real();
        buf[i][1] = getAt(i).imag();
    }
    //buf[100*1024+100][0] = 2000.0;
    fftw_plan p1 = fftw_plan_dft_2d(
                n0, n1,
                buf, buf,
                FFTW_BACKWARD, FFTW_ESTIMATE  );
    fftw_execute(p1);
    fftw_destroy_plan(p1);

     
    for(unsigned int i=0; i<n0*n1; i++)
        getData()[i] = std::complex<double>(buf[i][0], buf[i][1]);

    fftw_free(buf);
}

