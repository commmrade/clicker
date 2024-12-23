#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "httphandler.h"
#include <QObject>
#include <memory>
#include<thread>
#include <qmessagebox.h>

enum KindError {
    SERVER_DEAD,
    NOT_ENOUGH_MONEY,
    TOKEN_EXPIRED
};


class UserManager : public QObject
{
    Q_OBJECT
public:
    explicit UserManager(QObject *parent = nullptr);




    void get_user_info();
    void purchase_modifier(const QString &name, const QString &mod_type);
    void save_clicks();
    void daily_payment();

    bool check_server_dead();


    int get_clicks() const { return clicks; }
    double get_balance() const { return balance; }
    double get_click_mod() const { return click_mod; } 
    double get_hourly_pay_mod() const { return hourly_pay_mod; }
    double get_hourly_pay_mod_price() const { return hourly_pay_mod_price; }
    double get_click_mod_price() const { return click_mod_price; }
    void set_clicks(int new_val) {
        clicks = new_val;
    }

private:
    std::unique_ptr<HttpHandler> handler;
    
    int clicks;
    double balance;
    double click_mod;
    double hourly_pay_mod;
    double click_mod_price;
    double hourly_pay_mod_price;

    

signals:
    void user_info_updated();
    void error(KindError err_type);
    void save_completed();


};

#endif // USERMANAGER_H
