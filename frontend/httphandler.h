#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include <QObject>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<functional>
#include<QMap>
#include<QUrlQuery>
#include<QUrl>
#include<QJsonObject>
#include<QSettings>
#include<QJsonDocument>

class HttpHandler : public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager *manager;
public:

    HttpHandler();
    ~HttpHandler();

    void handle_get_request(const QString &url_link, std::function<void(int, QString)> callback, const QMap<QString, QString> &query_params = {}, const QMap<QString, QString> &header_params = {});
    void handle_post_request(const QString &url_link, std::function<void(int, QString)> callback, const QMap<QString, QString> &query_params = {}, const QMap<QString, QString> &header_params = {}, const QJsonObject &json_obj = {});
};

#endif // HTTPHANDLER_H
