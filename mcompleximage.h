#ifndef MCOMPLEXIMAGE_H
#define MCOMPLEXIMAGE_H

#include <QObject>
#include <vector>
#include <complex>
#include <cassert>
#include <fftw3.h>
#include "mimage.h"

class MComplexImage : public QObject
{
    Q_OBJECT
public:
    explicit MComplexImage(unsigned int n0, unsigned int n1, QObject *parent = nullptr);
    MComplexImage(const MComplexImage&);
    MComplexImage(const MGrayImage&);

    static MComplexImage fromGrayImage(MGrayImage&);
    void loadGrayImage(MGrayImage);

    std::complex<double> *getData();
    std::complex<double> getAt(unsigned int i) const;
    std::complex<double> getAt(unsigned int x, unsigned int y) const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;

    void initValue(std::complex<double>);
    void normalize();
    void fftshift();

    MComplexImage operator+(const MGrayImage &b) const;
    MComplexImage operator-(const MGrayImage &b) const;
    MComplexImage operator*(const MGrayImage &b) const;
    MComplexImage operator/(const MGrayImage &b) const;

    MComplexImage operator+(const MComplexImage &b) const;
    MComplexImage operator-(const MComplexImage &b) const;
    MComplexImage operator*(const MComplexImage &b) const;
    MComplexImage operator/(const MComplexImage &b) const;

    MComplexImage operator+(const double b) const;
    MComplexImage operator-(const double b) const;
    MComplexImage operator*(const double b) const;
    MComplexImage operator/(const double b) const;

    MComplexImage operator+=(const double b);
    MComplexImage operator-=(const double b);
    MComplexImage operator*=(const double b);
    MComplexImage operator/=(const double b);

    MComplexImage operator+=(const MGrayImage &b);
    MComplexImage operator-=(const MGrayImage &b);
    MComplexImage operator*=(const MGrayImage &b);
    MComplexImage operator/=(const MGrayImage &b);

    MComplexImage operator+=(const MComplexImage &b);
    MComplexImage operator-=(const MComplexImage &b);
    MComplexImage operator*=(const MComplexImage &b);
    MComplexImage operator/=(const MComplexImage &b);

    MComplexImage operator=(const MComplexImage &b);

    MGrayImage getMagnitudeSpectrum() const;
    MGrayImage getAngle() const;
    MGrayImage getReal();
    MComplexImage getSector(unsigned int x, unsigned int y, unsigned int size) const;
    void setSector(unsigned int x, unsigned int y, MComplexImage&);

    void fftInplace();


signals:

public slots:

private:
    unsigned int n0, n1;
    std::vector<std::complex<double>> data;
};

#endif // MCOMPLEXIMAGE_H
