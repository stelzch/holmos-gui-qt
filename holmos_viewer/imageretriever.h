#ifndef IMAGERETRIEVER_H
#define IMAGERETRIEVER_H

#include <QtNetwork>
#include <QObject>
#include <QTimer>
#include <opencv2/opencv.hpp>


using cv::Mat;

class ImageRetriever : public QObject
{
    Q_OBJECT
public:
    ImageRetriever(QString camServerUrl);
    ~ImageRetriever();
    Mat retrieve();

signals:
    void retrieveError();
public slots:
    void timeout();
private:
    QNetworkRequest req;
    QNetworkAccessManager *manager;
    QEventLoop sync;
    QTimer timeouter;

};

#endif // IMAGERETRIEVER_H
