#include "ui/mainwindow.h"
#include "timer_model.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("oscarcp");
    QCoreApplication::setApplicationName("pomodoro");

    MainAppWindow w;
    w.show();
    
    return a.exec();
}