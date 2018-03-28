#ifndef MIMAGE_H
#define MIMAGE_H

#include <QObject>
#include <cmath>
#include <algorithm>
#include <vector>
#include <QImage>
#include <opencv2/opencv.hpp>

using std::min;

class MGrayImage : public QObject
{
    Q_OBJECT
public:
    explicit MGrayImage(unsigned int n0, unsigned int n1, QObject *parent = nullptr);
    MGrayImage(const MGrayImage&);

    static MGrayImage loadFromFile(const char *filename);

    double *getData();
    double getAt(unsigned int i) const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;

    void sineInplace();
    void cosineInplace();
    void zeros();
    void initValue(double);
    void normalize();

    MGrayImage sine() const;
    MGrayImage cosine() const;

    MGrayImage operator+(const MGrayImage &b) const;
    MGrayImage operator-(const MGrayImage &b) const;
    MGrayImage operator*(const MGrayImage &b) const;
    MGrayImage operator/(const MGrayImage &b) const;
    MGrayImage operator=(const MGrayImage &b);

    MGrayImage operator+(const double b) const;
    MGrayImage operator-(const double b) const;
    MGrayImage operator*(const double b) const;
    MGrayImage operator/(const double b) const;

    MGrayImage operator+=(const double b);
    MGrayImage operator-=(const double b);
    MGrayImage operator*=(const double b);
    MGrayImage operator/=(const double b);

    QImage asQImage() const;
    //MComplexImage asComplex() const;
signals:

public slots:

private:
    unsigned int n0, n1;
    std::vector<double> data;
};

#endif // MIMAGE_H
