#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include<QNetworkAccessManager>
#include <memory>
#include "authmanager.h"
#include "httphandler.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include<QTimer>
#include <QJsonObject>
#include <QDebug>
#include<QUrlQuery>
#include<QSettings>
#include <memory>
#include <qmessagebox.h>


namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void login_through_token();


    void on_login_btn_clicked();

    void on_switch_to_log_btn_2_clicked();

    void on_reg_btn_clicked();

    void on_switch_to_log_btn_clicked();

private:
    Ui::Dialog *ui;
    //QNetworkAccessManager *manager;
    HttpHandler *handler;

    std::unique_ptr<authmanager> auth_manager;

    void register_user();
    void login_user();


    void auth(bool success);

};
#endif // DIALOG_H
