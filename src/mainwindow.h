#pragma once

#include <QMainWindow>
#include "timer_model.h"

QT_BEGIN_NAMESPACE
namespace Ui {class MainAppWindow;}
QT_END_NAMESPACE

const auto TIMER_START = "Start";
const auto TIMER_PAUSE = "Pause";
const auto TIMER_SKIP = "Skip";

class MainAppWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainAppWindow(QWidget *parent = nullptr);
        ~MainAppWindow();

    private:
        Ui::MainAppWindow *m_ui;
        TimerModel *m_timer_model;
};