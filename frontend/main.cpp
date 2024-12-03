#include "mainwindow.h"

#include <QApplication>
#include "dialog.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // w.show();
    Dialog dialog;
    bool show = false;
    if (dialog.exec() != QDialog::Accepted) {
        qDebug() << "stopped\n";
        exit(0);
        return a.exec();
    }
    MainWindow w;

    w.show();


    return a.exec();
}
