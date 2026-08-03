#include "mainwindow.h"
#include "settingsdialog.h"
#include "ui_window.h"
#include "timer_model.h"
#include "utils.h"
#include "config.h"
#include <iostream>
#include <format>
#include <QCloseEvent>
#include "statsdialog.h"

MainAppWindow::MainAppWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(new Ui::MainAppWindow)
{
    this->setWindowIcon(QIcon(":resources/icon.png"));
    
    m_ui->setupUi(this);
    m_timer_model = new TimerModel(this);

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
    QAction *settings_dialog_action = new QAction("Settings", this);
    QAction *view_stats_action = new QAction("Statistics", this);
    QAction *quit_app_action = new QAction("Quit", this);
    system_tray_menu->addAction(refocus_app_action);
    system_tray_menu->addAction(view_stats_action);
    system_tray_menu->addAction(settings_dialog_action);
    system_tray_menu->addAction(quit_app_action);

    m_sys_tray->setContextMenu(system_tray_menu);

    connect(quit_app_action, &QAction::triggered, this, [this](){
        this->m_timer_model->saveConfig();
        QApplication::quit();
    });
    connect(refocus_app_action, &QAction::triggered, this, &QMainWindow::show);

    connect(settings_dialog_action, &QAction::triggered, this, &MainAppWindow::openSettingsDialog);
    connect(view_stats_action, &QAction::triggered, this, &MainAppWindow::openStatsDialog);

    m_ui->stopButton->setText(TIMER_SKIP);
    m_ui->startPauseButton->setText(TIMER_START);

    m_ui->timerText->setText(QString::fromStdString(utils::formatMMSS(m_timer_model->getTime())));
    setStatsText(m_timer_model->getStats());

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

void MainAppWindow::openSettingsDialog() {

    SettingsDialog dialog;

    dialog.setWorkSessionDuration(m_timer_model->m_config.work_session_duration);
    dialog.setShortBreakDuration(m_timer_model->m_config.short_break_duration);
    dialog.setLongBreakDuration(m_timer_model->m_config.long_break_duration);

    int result = dialog.exec();

    if (result == QDialog::Accepted)
    {
        
        this->m_timer_model->m_config.work_session_duration = dialog.getWorkSessionDuration();
        this->m_timer_model->m_config.short_break_duration = dialog.getShortBreakDuration();
        this->m_timer_model->m_config.long_break_duration = dialog.getLongBreakDuration();

        this->m_timer_model->saveConfig();

    }
    
}

void MainAppWindow::openStatsDialog() {

    StatsDialog *dialog = new StatsDialog(this->m_timer_model, std::chrono::seconds{utils::getUnixTimestamp(-7)}, std::chrono::seconds{utils::getUnixTimestamp()}, this);
    dialog->show();

}