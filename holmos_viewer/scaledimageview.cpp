#include "scaledimageview.h"

ScaledImageView::ScaledImageView(QWidget *parent) : QWidget(parent),
    imgX(0), imgY(0)
{

}

void ScaledImageView::paintEvent(QPaintEvent *e) {
    QPainter painter(this);

    imgX = 0;
    imgY = 0;
    scaledImg = img.scaled(size(), Qt::KeepAspectRatio);
    if(scaledImg.width()<width())
        imgX = (width() - scaledImg.width()) / 2;
    if(scaledImg.height()<height())
        imgY = (height() - scaledImg.height()) / 2;

    painter.drawImage(imgX, imgY, scaledImg);
}

void ScaledImageView::setImage(QImage &img) {
    this->img = img;

    this->update();
}
