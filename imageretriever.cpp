#include "imageretriever.h"
#include "exceptions.h"

ImageRetriever::ImageRetriever(QString camServerUrl) : QObject()
{
    manager = new QNetworkAccessManager();
    req.setUrl(QUrl(camServerUrl + "/raw"));

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &sync, SLOT(quit()));


}


ImageRetriever::~ImageRetriever() {
    delete manager;
}

Mat ImageRetriever::retrieve() {
    QNetworkReply *rep = manager->get(req);
    sync.exec();

    if(rep->error() != QNetworkReply::NoError || rep->attribute(QNetworkRequest::HttpStatusCodeAttribute) != 200) {
        qDebug() << "[ERROR] " << rep->errorString();
        emit retrieveError();

        throw new ImageRetrivalException(rep->errorString());

        return Mat();
    }

    // Check if all headers are provided
    int w, h, c;

    try {
        foreach(QNetworkReply::RawHeaderPair header, rep->rawHeaderPairs()) {
            qDebug() << header.first << header.second;
            if(header.first == "X-Array-Width")
                w = header.second.toInt();
            if(header.first == "X-Array-Height")
                h = header.second.toInt();
            if(header.first == "X-Array-Channels")
                c = header.second.toInt();
        }
    } catch(...) {
        throw new ImageRetrivalException("Headers malformed");
        return Mat();
    }
    qDebug() << w << h << c << rep->bytesAvailable();
    Mat res(h, w, CV_16UC3);
    qDebug() << "Mat created";

    // 16bit, so * 2
    assert(rep->bytesAvailable() == w*h*c*2);

    rep->read(res.ptr<char>(), rep->bytesAvailable());

    qDebug() << res.at<uint16_t>(50);
    Mat out;
    res.convertTo(out, CV_8UC3, 0.25);
    cv::imwrite("/tmp/test.png", out);

    return res;
}

