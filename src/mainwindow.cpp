#include "mainwindow.h"
#include "ui_window.h"
#include "timer_model.h"
#include "utils.h"
#include <iostream>
#include <format>
#include <QCloseEvent>

MainAppWindow::MainAppWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(new Ui::MainAppWindow)
{
    this->setWindowIcon(QIcon(":resources/icon.png"));
    
    m_ui->setupUi(this);

    m_sys_tray = new QSystemTrayIcon(QIcon(":resources/icon.png"), this);
    m_sys_tray->show();
    m_sys_tray->setToolTip("Pomodoro");

    QMenu *system_tray_menu = new QMenu(this);
    QAction *header_action = new QAction("Pomodoro");
    header_action->setEnabled(false);
    header_action->setIcon(QIcon(":resources/icon.png"));
    system_tray_menu->addAction(header_action);
    system_tray_menu->addSeparator();

    QAction *refocus_app_action = new QAction("Open", this);
    QAction *quit_app_action = new QAction("Quit", this);
    system_tray_menu->addAction(refocus_app_action);
    system_tray_menu->addAction(quit_app_action);

    m_sys_tray->setContextMenu(system_tray_menu);

    connect(quit_app_action, &QAction::triggered, this, [](){
        QApplication::quit();
    });
    connect(refocus_app_action, &QAction::triggered, this, &QMainWindow::show);

    m_ui->stopButton->setText(TIMER_SKIP);
    m_ui->startPauseButton->setText(TIMER_START);

    m_timer_model = new TimerModel(this);
    m_ui->timerText->setText(QString::fromStdString(utils::formatMMSS(m_timer_model->getTime())));
    setStatsText(TimerStats{});

    connect(m_ui->startPauseButton, &QPushButton::clicked, m_timer_model, &TimerModel::startPauseTimer);
    connect(m_ui->stopButton, &QPushButton::clicked, m_timer_model, &TimerModel::stopTimer);

    connect(m_timer_model, &TimerModel::timerChanged, this, [this](int newTime)
            { m_ui->timerText->setText(QString::fromStdString(utils::formatMMSS(newTime))); });
    connect(m_timer_model, &TimerModel::timerStateChanged, this, [this](TimerState state)
            {
                switch (state)
                {
                case paused:
                case stopped:
                    m_ui->startPauseButton->setText(TIMER_START);

                    // Flimsy logic to detect if we've finished a break or work session in the background
                    if (!this->hasFocus() && state == stopped)
                    {
                        if (!m_timer_model->inWorkSession())
                        {
                            m_sys_tray->showMessage("Work session finished", "Take a break", QSystemTrayIcon::Information, 3000);
                        } else {
                            m_sys_tray->showMessage("Break finished", "Back to work", QSystemTrayIcon::Information, 3000);
                        }
                    }
                    
                    break;
                case running:
                    m_ui->startPauseButton->setText(TIMER_PAUSE);
                    break;
                default:
                    break;
                } });

    connect(m_timer_model, &TimerModel::statsChanged, this, &MainAppWindow::setStatsText);

    connect(m_sys_tray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason){

        if (reason == QSystemTrayIcon::Trigger)
        {   
            this->show();
        }
        
    });
}

MainAppWindow::~MainAppWindow()
{
    delete m_ui;
}

void MainAppWindow::setStatsText(TimerStats timer_stats)
{
    m_ui->pomoStats->setText(
        QString::fromStdString(std::format("Sessions: {} Breaks: {} Time worked: {}", timer_stats.work_sessions, timer_stats.breaks, utils::formatHHMMSS(timer_stats.ms_worked))));
}

void MainAppWindow::closeEvent(QCloseEvent *event) {
    event->ignore();
    this->hide();
}