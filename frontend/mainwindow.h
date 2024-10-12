#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QNetworkAccessManager>
#include<unordered_map>
#include "httphandler.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_click_mod_button_clicked();

    void on_pay_mod_button_clicked();

private:
    Ui::MainWindow *ui;
    int clicks;
    int raw_clicks;
    double raw_total;
    //double balance;

    //QNetworkAccessManager *manager;
    HttpHandler *handler;


    //std::unordered_map<std::string, QString> user_info;
    void get_user_info();
    double balance;
    double click_mod;
    double hourly_pay_mod;
    double click_mod_price;
    double hourly_pay_mod_price;

    void purchase_modifier(const QString &name, const QString &mod_type);

    void save_clicks();
    void update_balance();
    void daily_payment();

    void handle_server_dead();
protected:
    void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
