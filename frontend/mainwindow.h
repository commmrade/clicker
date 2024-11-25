#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QNetworkAccessManager>
#include <memory>
#include<unordered_map>
#include "usermanager.h"


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

    void on_log_out_btn_clicked();

private:
    Ui::MainWindow *ui;
 
    //double balance;

    //QNetworkAccessManager *manager;
    


    std::unique_ptr<UserManager> user_manager;

    //void get_user_info();
  


    

    void handle_server_dead();
protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent* event);
signals:

public slots:
    void update_balance();
    void error_handler(KindError error_type);
 

};
#endif // MAINWINDOW_H
