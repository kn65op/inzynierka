#include "displayimage.h"

displayImage::displayImage() {
    moveToThread(this);
}

displayImage::~displayImage() {
    netAccessMgr->deleteLater();
    netReply->deleteLater();
}

void displayImage::run()
{
    netAccessMgr = new QNetworkAccessManager;
    // QUrl urlCapture("http://usteni.gliczarow.info.pl/webcam.jpeg?1293535363894");
    QUrl urlCapture("http://192.168.0.1/now.jpg?snap=pre?ww=1600?wh=1200?wx=0?wy=0?ds=4");

    forever {
        netReply = netAccessMgr->get(QNetworkRequest(urlCapture));
        connect(netReply, SIGNAL(finished()), this, SLOT(downloadFinished()));
        exec();

        emit setPixmap(pixmap);
        //this->sleep(1);
    }
}

void displayImage::downloadFinished()
{
    netReply->deleteLater();

    if (netReply->error()) {
        qDebug() << netReply->errorString();
        emit networkError(netReply->errorString());
        return;
    }

    QByteArray jpegData = netReply->readAll();
    QPixmap tmp;
    tmp.loadFromData(jpegData);
    pixmap = tmp.scaled(700, 525);
    quit();
}