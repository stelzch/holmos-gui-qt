#include "imageselect.h"
#include <algorithm>

using std::min;
using std::max;

ImageSelect::ImageSelect(QWidget *parent) : QWidget(parent),
    selectX(0), selectY(0), imgX(0), imgY(0), indicatorR(0)
{
    QImage img(1, 1, QImage::Format_RGB888);
    emit setImage(img);
}

void ImageSelect::paintEvent(QPaintEvent *ev) {
    QPainter painter(this);

    painter.setBrush(QBrush(QColor::fromRgb(255, 0, 0)));
    painter.fillRect(0, 0, width(), height(), Qt::black);

    scaledImg = img.scaled(size(), Qt::KeepAspectRatio);
    imgX = 0;
    imgY = 0;

    if(scaledImg.width()<width())
        imgX = (width() - scaledImg.width()) / 2;
    if(scaledImg.height()<height())
        imgY = (height() - scaledImg.height()) / 2;

    painter.drawImage(imgX, imgY, scaledImg);

    QPen outlinePen(Qt::white, 2, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(outlinePen);
    painter.setBrush(Qt::transparent);

    if(indicatorR != 0) {
        auto topLeft = QPoint(imgX + (selectX - indicatorR) * (scaledImg.width() / float(img.width())),
                              imgY + (selectY - indicatorR) * (scaledImg.height() / float(img.height())));
        painter.drawRect(QRect(topLeft, QSize(indicatorR*2 * (scaledImg.width() / float(img.width())),
                                              indicatorR*2 * (scaledImg.height() / float(img.height())))));
    }
}

void ImageSelect::mouseDoubleClickEvent(QMouseEvent *evt) {
    int selectedX = (evt->pos().x() - imgX) * (img.width() / float(scaledImg.width()));
    int selectedY = (evt->pos().y() - imgY) * (img.height() / float(scaledImg.height()));

    /* Clamp the values to the image coordinate space */
    selectedX = max(0, min(selectedX, img.width()));
    selectedY = max(0, min(selectedY, img.height()));

    this->repaint();
    this->update();
    emit pointSelected(QPoint(selectedX, selectedY));

    selectX = selectedX;
    selectY = selectedY;
}

void ImageSelect::setIndicatorRadius(unsigned int r) {
    indicatorR = r;
    this->repaint();
}

void ImageSelect::updateXPos(unsigned int x) {
    selectX = x;
    this->repaint();
}

void ImageSelect::updateYPos(unsigned int y) {
    selectY = y;
    this->repaint();
}

QSize ImageSelect::sizeHint() const {
    return QSize(500, 500);
}

void ImageSelect::setImage(QImage &img) {
    this->img = img;

    this->update();
}
