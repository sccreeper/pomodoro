#include "mainwindow.h"
#include "ui_window.h"
#include "timer_model.h"
#include "utils.h"
#include <iostream>

MainAppWindow::MainAppWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(new Ui::MainAppWindow)
{
    m_ui->setupUi(this);

    m_ui->stopButton->setText(TIMER_SKIP);
    m_ui->startPauseButton->setText(TIMER_START);
    
    m_timer_model = new TimerModel(this);
    m_ui->timerText->setText(QString::fromStdString(utils::formatMs(m_timer_model->getTime())));

    connect(m_ui->startPauseButton, &QPushButton::clicked, m_timer_model, &TimerModel::startPauseTimer);
    connect(m_ui->stopButton, &QPushButton::clicked, m_timer_model, &TimerModel::stopTimer);

    connect(m_timer_model, &TimerModel::timerChanged, this, [this](int newTime)
            { m_ui->timerText->setText(QString::fromStdString(utils::formatMs(newTime))); });

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
                }
            });
}

MainAppWindow::~MainAppWindow()
{
    delete m_ui;
}