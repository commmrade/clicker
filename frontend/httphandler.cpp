#include "httphandler.h"

HttpHandler::HttpHandler() : QObject(nullptr) {
    manager = new QNetworkAccessManager();
}
HttpHandler::~HttpHandler() {
    delete manager;
}

void HttpHandler::handle_get_request(const QString &url_link, std::function<void(int, QString)> callback, const QMap<QString, QString> &query_params, const QMap<QString, QString> &header_params) {


    QSettings settings;
    QString name = settings.value("name").toString();
    QUrl url(url_link);
    QUrlQuery query;
    for (auto it = query_params.begin(); it != query_params.end(); ++it) {
        query.addQueryItem(it.key(), it.value());
    }
    url.setQuery(query);

    QNetworkRequest request(url);
    for (auto it = header_params.begin(); it != header_params.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }
    

    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [callback, reply]() {
        
        int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString data = reply->readAll();

        callback(code, data);        
        reply->deleteLater();
    });


}



void HttpHandler::handle_post_request(const QString &url_link, std::function<void(int, QString)> callback, const QMap<QString, QString> &query_params, const QMap<QString, QString> &header_params, const QJsonObject &json_obj) {
    QSettings settings;
    QString name = settings.value("name").toString();

    QUrl url(url_link);
    QUrlQuery query;

    for (auto it = query_params.begin(); it != query_params.end(); ++it) {
        query.addQueryItem(it.key(), it.value());
    }
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    for (auto it = header_params.begin(); it != header_params.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    QJsonDocument json_doc(json_obj);
    QNetworkReply *reply = manager->post(request, QString{json_doc.toJson(QJsonDocument::Compact)}.toUtf8());
    connect(reply, &QNetworkReply::finished, this, [query_params, callback, reply]() {
        int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString data = reply->readAll();

        callback(code, data);
        reply->deleteLater();
    });
}
