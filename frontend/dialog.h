#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include<QNetworkAccessManager>
#include "httphandler.h"
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



    void on_login_btn_clicked();

    void on_switch_to_log_btn_2_clicked();

    void on_reg_btn_clicked();

    void on_switch_to_log_btn_clicked();

private:
    Ui::Dialog *ui;
    //QNetworkAccessManager *manager;
    HttpHandler *handler;


    void register_user();
    void login_user();

};
#endif // DIALOG_H
