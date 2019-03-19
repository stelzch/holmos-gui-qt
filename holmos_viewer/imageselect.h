#ifndef IMAGESELECT_H
#define IMAGESELECT_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QImage>
#include <QDebug>
#include <QPoint>

class ImageSelect : public QWidget
{
    Q_OBJECT
public:
    explicit ImageSelect(QWidget *parent = nullptr);
    QSize sizeHint() const override;

signals:
    void pointSelected(QPoint);
public slots:
    void setImage(QImage&);
    void setIndicatorRadius(unsigned int);
    void updateXPos(unsigned int);
    void updateYPos(unsigned int);
protected:
    void paintEvent(QPaintEvent*) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;

private:
    QImage img, scaledImg;
    unsigned int imgX, imgY, selectX, selectY, indicatorR;

};

#endif // IMAGESELECT_H
