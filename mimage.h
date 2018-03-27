#ifndef MIMAGE_H
#define MIMAGE_H

#include <QObject>
#include <cmath>
#include <vector>

class MGrayImage : public QObject
{
    Q_OBJECT
public:
    explicit MGrayImage(QObject *parent = nullptr, unsigned int n0 = 1024, unsigned int n1 = 1024);
    MGrayImage(const MGrayImage&);
    double *getData();
    unsigned int getWidth() const;
    unsigned int getHeight() const;

    void sineInplace();
    void cosineInplace();

    MGrayImage sine() const;
    MGrayImage cosine() const;


signals:

public slots:

private:
    unsigned int n0, n1;
    std::vector<double> data;
};

#endif // MIMAGE_H
