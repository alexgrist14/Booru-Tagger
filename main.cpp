#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("MyCorp");              // имя компании
    a.setOrganizationDomain("metanit.com");       // домен компании
    a.setApplicationName("Test Application");     // имя приложения
    a.setApplicationVersion("1.0.0");             // версия приложения
    MainWindow w;
    w.show();
    return a.exec();
}
