#include "dialog.h"
#include "ui_dialog.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include<QTimer>
#include <QJsonObject>
#include <QDebug>
#include<QUrlQuery>
#include<QSettings>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    handler = new HttpHandler();



    QTimer::singleShot(0, this, SLOT(login_through_token()));
}

Dialog::~Dialog()
{
    delete handler;
    delete ui;
}

void Dialog::login_through_token() {
    QSettings settings;
    QString token = settings.value("token").toString();
    qDebug() << "name is " << settings.value("name");
    QMap<QString, QString> query_params;
    query_params.insert("token", token);

    handler->handle_get_request("http://127.0.0.1:8848/api/login_t", [this](int code, QString reply_data) {
        if (code == 200) {
            QJsonDocument jDoc = QJsonDocument::fromJson(reply_data.toUtf8());
            QJsonObject jObj = jDoc.object();
            QSettings settings;
            qDebug() << jObj;
            settings.setValue("name", jObj["name"].toString());
            qDebug() << "Val is" << settings.value("name").toString();

            accept();
        } else if (code == 401) {

        } else {
            qDebug() << code;
            ui->reg_btn->setText("Server is dead, wait");
            ui->login_btn->setText("Server is dead, wait");
        }
    }, query_params);
}


void Dialog::on_login_btn_clicked()
{
    login_user();
    QSettings settings;
    settings.setValue("name", ui->name_edit->text());
}
void Dialog::on_switch_to_log_btn_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void Dialog::on_reg_btn_clicked()
{
    register_user();
    QSettings settings;
    settings.setValue("name", ui->reg_name_edit->text());
}


void Dialog::on_switch_to_log_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Dialog::register_user() {
    if (ui->reg_name_edit->text().size() < 3) {
        ui->reg_name_edit->clear();
        ui->reg_name_edit->setPlaceholderText("Must be longer than 3");
    } else if (ui->reg_pass_edit->text().size() < 3) {
        ui->reg_pass_edit->clear();
        ui->reg_pass_edit->setPlaceholderText("Must be longer than 3");
    } else if (ui->reg_name_edit->text().size() > 16) {
        ui->reg_name_edit->clear();
        ui->reg_name_edit->setPlaceholderText("Must be shorter than 16");
    } else if (ui->reg_pass_edit->text().size() > 32) {
        ui->reg_pass_edit->clear();
        ui->reg_pass_edit->setPlaceholderText("Must be shorter than 32");
    }


    QMap<QString, QString> query_par;
    QMap<QString, QString> header_par;

    query_par.insert("name", ui->reg_name_edit->text());
    query_par.insert("password", ui->reg_pass_edit->text());

    handler->handle_post_request("http://127.0.0.1:8848/api/reg", [this](int code, QString reply_data) {
        if (code == 200) {
            QJsonDocument jDoc = QJsonDocument::fromJson(reply_data.toUtf8());
            QJsonObject jObj = jDoc.object();
            QSettings settings;
            settings.setValue("token", jObj["token"].toString());
            settings.setValue("name", ui->reg_name_edit->text());
            accept();
        } else if (code == 400){
            qDebug() << "Unknown error 400";
        } else {
            ui->reg_btn->setText("Server is likely to be dead, please wait");
        }
    }, query_par, header_par);


}
void Dialog::login_user() {
    if (ui->name_edit->text().size() < 3) {
        ui->name_edit->clear();
        ui->name_edit->setPlaceholderText("Must be longer than 3");
    } else if (ui->pass_edit->text().size() < 3) {
        ui->pass_edit->clear();
        ui->pass_edit->setPlaceholderText("Must be longer than 3");
    } else if (ui->name_edit->text().size() > 16) {
        ui->name_edit->clear();
        ui->name_edit->setPlaceholderText("Must be shorter than 16");
    } else if (ui->pass_edit->text().size() > 32) {
        ui->pass_edit->clear();
        ui->pass_edit->setPlaceholderText("Must be shorter than 32");
    }

    QMap<QString, QString> query_par;

    query_par.insert("name", ui->name_edit->text());
    query_par.insert("password", ui->pass_edit->text());

    handler->handle_get_request("http://127.0.0.1:8848/api/login", [this](int code, QString reply_data) {
        if (code == 200) {
            QJsonDocument jDoc = QJsonDocument::fromJson(reply_data.toUtf8());
            QJsonObject jObj = jDoc.object();
            QSettings settings;
            settings.setValue("token", jObj["token"].toString());
            settings.setValue("name", ui->name_edit->text());

            accept();
        } else if (code == 400){

            qDebug() << "LOGIN CODE" << code;
            ui->login_btn->setText("Wrong password or username");
        } else {
            ui->login_btn->setText("Server is dead, please wait");
        }
    }, query_par);
}

