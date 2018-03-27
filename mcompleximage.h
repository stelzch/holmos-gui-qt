#ifndef MCOMPLEXIMAGE_H
#define MCOMPLEXIMAGE_H

#include <QObject>
#include <vector>
#include <complex>
#include <cassert>
#include "mimage.h"

class MComplexImage : public QObject
{
    Q_OBJECT
public:
    explicit MComplexImage(unsigned int n0, unsigned int n1, QObject *parent = nullptr);
    MComplexImage(const MComplexImage&);
    MComplexImage(const MGrayImage&);

    std::complex<double> *getData();
    std::complex<double> getAt(unsigned int i) const;
    unsigned int getWidth() const;
        unsigned int getHeight() const;

    MComplexImage operator+(const MGrayImage &b) const;
    MComplexImage operator-(const MGrayImage &b) const;
    MComplexImage operator*(const MGrayImage &b) const;
    MComplexImage operator/(const MGrayImage &b) const;

    MGrayImage getMagnitudeSpectrum() const;
    MComplexImage getSector(unsigned int x, unsigned int y, unsigned int size) const;
    void setSector(unsigned int x, unsigned int y, MComplexImage&);


signals:

public slots:

private:
    unsigned int n0, n1;
    std::vector<std::complex<double>> data;
};

#endif // MCOMPLEXIMAGE_H
