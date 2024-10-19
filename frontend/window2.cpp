#include "window2.h"
#include "ui_window2.h"

window2::window2(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::window2)
{
    ui->setupUi(this);
}

window2::~window2()
{
    delete ui;
}
