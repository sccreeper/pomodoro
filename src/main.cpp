#include "mainwindow.h"
#include "timer_model.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainAppWindow w;
    w.show(); // Because 'w' has no parent, it becomes a visible window

    qDebug() << "Widget Geometry:" << w.geometry();
    
    return a.exec();
}