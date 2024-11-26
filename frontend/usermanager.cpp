#include "usermanager.h"
#include "httphandler.h"
#include <memory>
#include <qmessagebox.h>

UserManager::UserManager(QObject *parent)
    : QObject{parent}, clicks(0), handler(std::make_unique<HttpHandler>())
  
{
    balance = 0;
    click_mod = 0;
    hourly_pay_mod = 0;
    click_mod_price = 0;
    hourly_pay_mod_price = 0;

}

void UserManager::get_user_info() {
    QSettings settings;
    QMap<QString, QString> params;
    params.insert("name", settings.value("name").toString());

    QMap<QString, QString> header_par;
    header_par.insert("Authorization", settings.value("token").toString());

    handler->handle_get_request("http://127.0.0.1:8848/api/user",
    [this](int code, QString reply_data) {

        if (code == 200) {
            QJsonDocument jDoc = QJsonDocument::fromJson(reply_data.toUtf8());
            QJsonObject jObj = jDoc.object();
            balance = jObj["balance"].toDouble();
            click_mod = jObj["click_mod"].toDouble();
            hourly_pay_mod = jObj["hourly_payout_mod"].toDouble();
            click_mod_price = jObj["click_mod_price"].toDouble();
            hourly_pay_mod_price = jObj["hourly_payout_mod_price"].toDouble();


            emit user_info_updated();

        } else if (code == 400) {
            qDebug() << "Error get user info";
        } else if (code == 0) {
            emit error(SERVER_DEAD);
            
        }
    },params, header_par
    );
}

void UserManager::purchase_modifier(const QString &name, const QString &mod_type) {
    if ((mod_type == "click" || mod_type == "pay") && clicks > 0) {
        qDebug() << "save clicks";
        save_clicks();
    }
    QSettings settings;

    QJsonObject json_obj;
    json_obj["name"] = settings.value("name").toString();
    json_obj["modifier_type"] = mod_type;

    QMap<QString, QString> header_par;
    header_par.insert("Authorization", settings.value("token").toString());

    handler->handle_post_request("http://127.0.0.1:8848/api/purchase", [this, mod_type](int code, QString reply_data) {
    if (code == 200) {
        clicks = 0;
        get_user_info();

    } else if (code == 401) {

        emit error(TOKEN_EXPIRED);
        
        exit(1);
    } else if (code == 400) {
        emit error(NOT_ENOUGH_MONEY);
    } else {
        emit error(SERVER_DEAD);
    }
    }, {}, header_par, json_obj);
}

void UserManager::save_clicks() {
    qDebug() << "clicks " << clicks;
    if (clicks <= 0) {
        emit save_completed();
        return;
    }
    QSettings settings;

    QJsonObject json_obj;
    json_obj["name"] = settings.value("name").toString();
    json_obj["click"] = QString::number(clicks);

    QMap<QString, QString> header_par;
    header_par.insert("Authorization", settings.value("token").toString());

    handler->handle_post_request("http://127.0.0.1:8848/api/clicks", [this](int code, QString reply_data) {
        if (code == 200) {
            clicks = 0;
            get_user_info();
        } else if (code == 401) {
            QMessageBox box;
            box.setWindowTitle("Warning");
            box.setIcon(QMessageBox::Information);
            box.setStandardButtons(QMessageBox::Ok);
            box.setText("Your token is not correct, please restart the game");

            int ret = box.exec();

            exit(1);
        } else if (code == 400){
            qDebug() << "Clicks 400 error";
        } else {
            emit error(SERVER_DEAD);
        }
        emit save_completed();
    }, {}, header_par, json_obj);
}

void UserManager::daily_payment() {
    QSettings settings;



    QMap<QString, QString> header_par;
    header_par.insert("Authorization", settings.value("token").toString());

    QJsonObject j_obj;
    j_obj["name"] = settings.value("name").toString();


    handler->handle_post_request("http://127.0.0.1:8848/api/daily-pay", [this](int code, QString reply_data) {
        if (code == 200) {
            get_user_info();
        } else if (code == 401) {
            QMessageBox box;
            box.setWindowTitle("Warning");
            box.setIcon(QMessageBox::Information);
            box.setStandardButtons(QMessageBox::Ok);
            box.setText("Your token is not correct, please restart the game");

            int ret = box.exec();

            exit(1);
        } else if (code == 400){

            //qDebug() << "daily payment unknown error"; //This error usually means that user was just registered
        } else {
            qDebug() << "Backend server is dead";
            emit error(SERVER_DEAD);
        }
    }, {}, header_par, j_obj);
}

bool UserManager::check_server_dead() {
    qDebug() << "checking...";
    QSettings settings;
    QMap<QString, QString> params;
    params.insert("name", settings.value("name").toString());


    handler->handle_get_request("http://127.0.0.1:8848/api/user",
        [this](int code, QString reply_data) {
            qDebug() << code;
            return code == 0;
        },
        params);

    return false;
}