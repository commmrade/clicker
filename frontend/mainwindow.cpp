#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include<QTimer>
#include<QSettings>
#include<QMessageBox>
#include<QCloseEvent>
#include <qjsonobject.h>
#include<thread>
#include<QUrlQuery>
#include "dialog.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , clicks(0)
    , raw_clicks(0)
    , raw_total(0.0)

{
    ui->setupUi(this);

    handler = new HttpHandler();

    get_user_info();
    daily_payment(); //it also gets user info

    update_balance();

    QIcon icon("/home/klewy/Pictures/Screenshots/a.png");
    ui->pushButton->setIcon(icon);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    ui->pushButton->setIconSize({ui->pushButton->geometry().width(), ui->pushButton->geometry().height()});
}

MainWindow::~MainWindow()
{
    qDebug() << "Dstr";
    save_clicks();
    delete handler;
    delete ui;
}
void MainWindow::handle_server_dead() {
    
    setEnabled(false);
    ui->click_mod_button->setText("Server is dead, wait");
    ui->pay_mod_button->setText("Serve is dead, please wait");

    QSettings settings;
    QMap<QString, QString> params;
    params.insert("name", settings.value("name").toString());

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, params, timer]() {
        handler->handle_get_request("http://127.0.0.1:8848/api/user",
            [this, timer](int code, QString reply_data) {
                if (code == 200) {
                    // Handle successful response
                    setEnabled(true);
                    timer->stop();
                } else if (code == 400) {
                    // Handle specific error response
                }
            },
            params);
    });


    // Start the timer to check every 2 seconds
    timer->start(2000);
}



void MainWindow::get_user_info() {
    QSettings settings;
    QMap<QString, QString> params;
    params.insert("name", settings.value("name").toString());

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


                raw_total = balance;
                update_balance();

            } else if (code == 400) {
                qDebug() << "Error get user info";
            } else {
                qDebug() << "get user info serv dead";
                handle_server_dead();
            }
        },
        params);
}

void MainWindow::daily_payment() {
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
            handle_server_dead();
        }
    }, {}, header_par, j_obj);
}

void MainWindow::purchase_modifier(const QString &name, const QString &mod_type) {
    if (mod_type == "click" && clicks > 0) {
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
            get_user_info();

        } else if (code == 401) {
            QMessageBox box;
            box.setWindowTitle("Warning");
            box.setIcon(QMessageBox::Information);
            box.setStandardButtons(QMessageBox::Ok);
            box.setText("Your token is not correct, please restart the game");

            int ret = box.exec();
            exit(1);
        } else if (code == 400) {
            if (mod_type == "click") {
                ui->click_mod_button->setText("Not enough money");
                std::thread thr([this]() {
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    ui->click_mod_button->setText("Upgrade for: " + QString::number(click_mod_price) + "$");
                });
                thr.detach(); // Temporarily set text to not enough money (async)
            } else if (mod_type == "pay") {
                ui->pay_mod_button->setText("Not enough money");
                std::thread thr([this]() {
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    ui->pay_mod_button->setText("Upgrade for: " + QString::number(hourly_pay_mod_price) + "$");
                });
                thr.detach(); // Temporarily set text to not enough money (async)
            } else {
                qDebug() << "Purchase POST what??";
            }
        } else {
            handle_server_dead();
            ui->click_mod_button->setText("Server is likely to be dead, please wait");
            ui->pay_mod_button->setText("Server is likely to be dead, please wait");
        }
        }, {}, header_par, json_obj);
}



void MainWindow::save_clicks() {

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
            handle_server_dead();
        }
        emit save_completed();
    }, {}, header_par, json_obj);
}
void MainWindow::on_pushButton_clicked()
{
    ui->pushButton->setIconSize({ui->pushButton->geometry().width(), ui->pushButton->geometry().height()});


    raw_total += std::round(1.0 * click_mod * 100.0f) / 100.0f;
    clicks++;
    raw_clicks++;

    update_balance();


}
void MainWindow::closeEvent(QCloseEvent *event) {
    QEventLoop loop;
    connect(this, &MainWindow::save_completed, &loop, &QEventLoop::quit);

    save_clicks();

    loop.exec();

    event->accept();
}



void MainWindow::on_pay_mod_button_clicked()
{
    QSettings settings;
    QString name = settings.value("name").toString();

    purchase_modifier(name, "pay");
}
void MainWindow::on_click_mod_button_clicked()
{
    QSettings settings;
    QString name = settings.value("name").toString();

    purchase_modifier(name, "click");
}

void MainWindow::update_balance() {
    ui->balance_label->setText("Current balance: " + QString::number(raw_total) + "$");

    ui->click_mod_label->setText("Current click mod: " + QString::number(click_mod));
    ui->click_mod_button->setText("Upgrade for: " + QString::number(click_mod_price) + "$");

    ui->pay_mod_label->setText("Current pay mod: " + QString::number(hourly_pay_mod));

    ui->pay_mod_button->setText("Upgrade for: " + QString::number(hourly_pay_mod_price) + "$");

    ui->pushButton->setText("Click");
}


void MainWindow::on_log_out_btn_clicked()
{
    QSettings settings;
    settings.remove("token");
    settings.remove("name");

    QApplication::quit();
}

