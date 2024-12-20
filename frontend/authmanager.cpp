#include "authmanager.h"
#include "httphandler.h"


authmanager::authmanager(QObject *parent)
    : QObject{parent},
    handler(std::make_unique<HttpHandler>())
{

}



void authmanager::login(const QString &name, const QString &password) {
    
    QJsonObject json_obj;
    json_obj["name"] = name;
    json_obj["password"] = password;

    handler->handle_post_request("http://127.0.0.1:8848/auth/login", [this, name](int code, QString reply_data) {
        if (code == 200) {
            QJsonDocument jDoc = QJsonDocument::fromJson(reply_data.toUtf8());
            QJsonObject jObj = jDoc.object();
            QSettings settings;
            settings.setValue("token", jObj["token"].toString());
            settings.setValue("name", name);

            emit auth_attempt(true);
        } else {
            emit auth_attempt(false);
        }
    }, {}, {}, json_obj);
}

void authmanager::reg(const QString &name, const QString &password) {
    QJsonObject json_obj;
    json_obj["name"] = name;
    json_obj["password"] = password;

    handler->handle_post_request("http://127.0.0.1:8848/auth/reg", [this, name](int code, QString reply_data) {
        if (code == 200) {
            QJsonDocument jDoc = QJsonDocument::fromJson(reply_data.toUtf8());
            QJsonObject jObj = jDoc.object();
            QSettings settings;
            settings.setValue("token", jObj["token"].toString());
            settings.setValue("name", name);
            
            emit auth_attempt(true);
        } else {
            emit auth_attempt(false);
        }
    }, {}, {}, json_obj);
}

void authmanager::login_token(const QString &auth_token) {

    QMap<QString, QString> header_params;
    header_params.insert("Authorization", auth_token);

    handler->handle_get_request("http://127.0.0.1:8848/auth/login", [this](int code, QString reply_data) {
        if (code == 200) {
            QJsonDocument jDoc = QJsonDocument::fromJson(reply_data.toUtf8());
            QJsonObject jObj = jDoc.object();
            QSettings settings;

            settings.setValue("name", jObj["name"].toString());

            emit auth_attempt(true);
        } else {
            emit auth_attempt(false);
        }
    }, {}, header_params);
}