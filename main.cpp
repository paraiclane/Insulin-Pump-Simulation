#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ProfileManager pm;
    Home h;
    Log l;

    Pump p(&pm, &h, &l);

    MainWindow w;

    w.show();
    return a.exec();
}
