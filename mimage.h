#ifndef MIMAGE_H
#define MIMAGE_H

#include <QObject>
#include <cmath>
#include <algorithm>
#include <vector>
#include <QImage>

using std::min;

class MGrayImage : public QObject
{
    Q_OBJECT
public:
    explicit MGrayImage(unsigned int n0, unsigned int n1, QObject *parent = nullptr);
    MGrayImage(const MGrayImage&);
    double *getData();
    double getAt(unsigned int i) const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;

    void sineInplace();
    void cosineInplace();
    void zeros();
    void initValue(double);

    MGrayImage sine() const;
    MGrayImage cosine() const;

    MGrayImage operator+(const MGrayImage &b) const;
    MGrayImage operator-(const MGrayImage &b) const;
    MGrayImage operator*(const MGrayImage &b) const;
    MGrayImage operator/(const MGrayImage &b) const;
signals:

public slots:

private:
    unsigned int n0, n1;
    std::vector<double> data;
};

#endif // MIMAGE_H
