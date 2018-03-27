#include "mcompleximage.h"

MComplexImage::MComplexImage(unsigned int n0, unsigned int n1,
                             QObject *parent) : QObject(parent), n0(n0), n1(n1)
{
    data.reserve(n0 * n1);
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

std::complex<double> *MComplexImage::getData() {
    return data.data();
}

std::complex<double> MComplexImage::getAt(unsigned int i) const {
    return data.at(i);
}

unsigned int MComplexImage::getWidth() const {
    return n1;
}

unsigned int MComplexImage::getHeight() const {
    return n0;
}

MComplexImage MComplexImage::operator+(const MGrayImage &b) const {
    unsigned int new_width = min(getWidth(), b.getWidth());
    unsigned int new_height = min(getHeight(), b.getHeight());

    MComplexImage sum(new_height, new_width);

    for(unsigned int y=0; y<new_height; y++) {
        for(unsigned int x=0; x<new_height; x++) {
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
        for(unsigned int x=0; x<new_height; x++) {
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
        for(unsigned int x=0; x<new_height; x++) {
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
        for(unsigned int x=0; x<new_height; x++) {
            div.getData()[y*new_width+x].real(getAt(y*getWidth()+x).real() /
                    b.getAt(y*b.getWidth()+x));
            div.getData()[y*new_width+x].imag(getAt(y*getWidth()+x).imag() /
                    b.getAt(y*b.getWidth()+x));
        }
    }

    return div;
}

MGrayImage MComplexImage::getMagnitudeSpectrum() const {
    MGrayImage magspec(n0, n1);

    for(unsigned int i=0; i<n0*n1; i++) {
        magspec.getData()[i] = abs(getAt(i));
    }

    return magspec;
}

MComplexImage MComplexImage::getSector(unsigned int x, unsigned int y, unsigned int size) const {
    assert(y+size <= n0);
    assert(x+size <= n1);

    MComplexImage sector(size, size);

    for(unsigned int my=y; y<y+size; y++) {
        for(unsigned int mx=x; x<x+size; x++) {
            /* Sector coordinates */
            unsigned int sy = my - y;
            unsigned int sx = mx - x;

            sector.getData()[sy * size + sx] = getAt(my*getWidth()+mx);
        }
    }

    return sector;
}

void MComplexImage::setSector(unsigned int x, unsigned int y, MComplexImage &sector) {
    assert(y+sector.getHeight() <= n0);
    assert(x+sector.getWidth() <= n1);

    for(unsigned int my=y; y<y+sector.getHeight(); y++) {
        for(unsigned int mx=x; x<x+sector.getWidth(); x++) {
            /* Sector coordinates */
            unsigned sy = my - y;
            unsigned sx = mx - x;

            getData()[my*getWidth()+mx] = sector.getAt(sy*sector.getWidth()+sx);
        }
    }
}

