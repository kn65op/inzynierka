#ifndef DISPLAYIMAGE_H
#define DISPLAYIMAGE_H

#include <QtCore/QThread>
#include <QtCore/QEventLoop>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtGUI/QImage>
#include <QtCore/QDebug>
#include <QtCore/QBuffer>
#include <QtGUI/QPixmap>

class displayImage : public QThread {

    Q_OBJECT

    public:
        displayImage();
        ~displayImage();
        void run();

        QPixmap pixmap;

    signals:
        void networkError(const QString errorMessage);
        void parseError(const QString errorMessage);
        void setPixmap(const QPixmap & pixmap);

    private slots:
        void downloadFinished();

    private:
        QNetworkAccessManager *netAccessMgr;
        QNetworkReply *netReply;
};

#endif // DISPLAYIMAGE_H
