#include "mainwindow.h"
#include "ui_window.h"
#include "timer_model.h"
#include "utils.h"
#include <iostream>
#include <format>

MainAppWindow::MainAppWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(new Ui::MainAppWindow)
{
    m_ui->setupUi(this);

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
                    break;
                case running:
                    m_ui->startPauseButton->setText(TIMER_PAUSE);
                    break;
                default:
                    break;
                } });

    connect(m_timer_model, &TimerModel::statsChanged, this, &MainAppWindow::setStatsText);
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