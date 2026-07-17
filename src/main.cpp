#include "mainwindow.h"
#include "timer_model.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainAppWindow w;
    w.show();
    
    return a.exec();
}