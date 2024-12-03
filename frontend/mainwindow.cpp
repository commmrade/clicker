#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , user_manager(std::make_unique<UserManager>())
{
    ui->setupUi(this);

    
  
    user_manager->get_user_info();

    QIcon icon("/home/klewy/Pictures/Screenshots/a.png");
    ui->pushButton->setIcon(icon);

    connect(user_manager.get(), &UserManager::user_info_updated, this, &MainWindow::update_balance);
    connect(user_manager.get(), &UserManager::error, this, &MainWindow::error_handler);
    
    user_manager->daily_payment();

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::resizeEvent(QResizeEvent* event) {
    ui->pushButton->setIconSize({ui->pushButton->geometry().width(), ui->pushButton->geometry().height()});
}



void MainWindow::handle_server_dead() {
    setEnabled(false);
 
    QSettings settings;
    QMap<QString, QString> params;
    params.insert("name", settings.value("name").toString());

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, timer]() {

        if (user_manager->check_server_dead()) {

        } else {
         
            setEnabled(true);
             timer->stop();
        }
    });

    // Start the timer to check every 2 seconds
    timer->start(2000);
}


void MainWindow::on_pushButton_clicked()
{
    ui->pushButton->setIconSize({ui->pushButton->geometry().width(), ui->pushButton->geometry().height()});    
    user_manager->clicks++;
    update_balance();
}
void MainWindow::closeEvent(QCloseEvent *event) {
    QEventLoop loop;
    connect(user_manager.get(), &UserManager::save_completed, &loop, &QEventLoop::quit);
    user_manager->save_clicks();

    loop.exec();

    event->accept();
}

void MainWindow::on_pay_mod_button_clicked()
{
    QSettings settings;
    QString name = settings.value("name").toString();

    user_manager->purchase_modifier(name, "pay");

}
void MainWindow::on_click_mod_button_clicked()
{
    QSettings settings;
    QString name = settings.value("name").toString();

    user_manager->purchase_modifier(name, "click");
}

void MainWindow::update_balance() {
    ui->balance_label->setText("Current balance: " + QString::number(user_manager->balance + user_manager->clicks * user_manager->click_mod) + "$");

    ui->click_mod_label->setText("Current click mod: " + QString::number(user_manager->click_mod));
    ui->click_mod_button->setText("Upgrade for: " + QString::number(user_manager->click_mod_price) + "$");
    ui->pay_mod_label->setText("Current pay mod: " + QString::number(user_manager->hourly_pay_mod));
    ui->pay_mod_button->setText("Upgrade for: " + QString::number(user_manager->hourly_pay_mod_price) + "$");

    ui->pushButton->setText("Click");
}

void MainWindow::on_log_out_btn_clicked()
{
    QSettings settings;
    settings.remove("token");
    settings.remove("name");

    QApplication::quit();
}

void MainWindow::error_handler(KindError error_type) {
    switch (error_type) {
        case SERVER_DEAD: {
            handle_server_dead();
            break;
        }
        case NOT_ENOUGH_MONEY: {
            QMessageBox box;
            box.setWindowTitle("Warning");
            box.setIcon(QMessageBox::Information);
            box.setStandardButtons(QMessageBox::Ok);
            box.setText("Not enough money");

            int res = box.exec();

            break;
        }
        case TOKEN_EXPIRED: {
            QMessageBox box;
            box.setWindowTitle("Warning");
            box.setIcon(QMessageBox::Information);
            box.setStandardButtons(QMessageBox::Ok);
            box.setText("Your token has expired, please relogin");

            int res = box.exec();
            QApplication::exit();
            break;
        }
    }
}
