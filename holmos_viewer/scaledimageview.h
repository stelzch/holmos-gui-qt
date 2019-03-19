#ifndef SCALEDIMAGEVIEW_H
#define SCALEDIMAGEVIEW_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

class ScaledImageView : public QWidget
{
    Q_OBJECT
public:
    explicit ScaledImageView(QWidget *parent = nullptr);

signals:

public slots:
    void setImage(QImage&);
protected:
    QImage img, scaledImg;
    void paintEvent(QPaintEvent*) override;
    unsigned int imgX, imgY;
};

#endif // SCALEDIMAGEVIEW_H
