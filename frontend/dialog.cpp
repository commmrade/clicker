#include "dialog.h"
#include <ui_dialog.h>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
    , auth_manager(std::make_unique<authmanager>())
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
    
    connect(auth_manager.get(), &authmanager::auth_attempt, this, &Dialog::auth);
    auth_manager->login_token(token);
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

    QJsonObject json_obj;
    json_obj["name"] = ui->reg_name_edit->text();
    json_obj["password"] = ui->reg_pass_edit->text();



    connect(auth_manager.get(), &authmanager::auth_attempt, this, &Dialog::auth);
    auth_manager->reg(ui->reg_name_edit->text(), ui->reg_pass_edit->text());
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



    connect(auth_manager.get(), &authmanager::auth_attempt, this, &Dialog::auth);
    auth_manager->login(ui->name_edit->text(), ui->pass_edit->text());
}

void Dialog::auth(bool success) {
    if (success) {
        accept();
    } else {
        QMessageBox box;
        box.setWindowTitle("Warning");
        box.setIcon(QMessageBox::Critical);
        box.setStandardButtons(QMessageBox::Ok);
        box.setText("Authorization error");

        int res = box.exec();
    }
}
