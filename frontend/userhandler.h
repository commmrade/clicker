#ifndef USERHANDLER_H
#define USERHANDLER_H
#include <string>
#include<QSettings>
#include<QObject>
#include<QNetworkAccessManager>
#include <QNetworkReply>
#include<QUrlQuery>

enum Error {
    NONE,
    TOKEN,
    REQUEST
};


class UserHandler : public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager *manager;
public:
    UserHandler() {
        manager = new QNetworkAccessManager();
    }

    Error daily_payment() {
        QSettings settings;
        QString name = settings.value("name").toString();

        //QNetworkAccessManager *manager = new QNetworkAccessManager();

        QUrl url("http://127.0.0.1:8848/api/daily-pay");
        QUrlQuery query;
        query.addQueryItem("name", name);
        url.setQuery(query);

        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        QNetworkReply *reply = manager->post(request, QByteArray());

        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            //qDebug() << "ELFKDJDFKSJFDSJFDSJKFDS";
            if (reply->error() == QNetworkReply::NoError) {
                int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                if (code == 200) {
                    // Handle successful response
                    qDebug() << "Payment successful:" << reply->readAll();
                } else if (code == 400) {
                    // Handle Bad Request
                    qDebug() << "Bad Request (400):" << reply->readAll();
                } else if (code == 401) {
                    // Handle Unauthorized
                    qDebug() << "Unauthorized (401):" << reply->readAll();
                } else {
                    // Handle unexpected HTTP status codes
                    qDebug() << "Unexpected HTTP status code:" << code;
                }
            } else {
                // Handle network errors
                qDebug() << "Network error occurred:" << reply->errorString();
            }
            reply->deleteLater(); // Clean up the reply

        });
    }
};

#endif // USERHANDLER_H
