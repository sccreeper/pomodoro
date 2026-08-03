#include "ui/mainwindow.h"
#include "timer_model.h"
#include <QApplication>
#include <QDebug>
#include <QErrorMessage>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("oscarcp");
    QCoreApplication::setApplicationName("pomodoro");

    try
    {
        MainAppWindow w;
        w.show();
        return a.exec();
    }
    catch (const std::exception &e)
    {
        QErrorMessage::qtHandler()->showMessage(
            QString("Failed to start Pomodoro: %1").arg(e.what()));
        return a.exec(); // keep event loop alive so the dialog actually shows
    }

}