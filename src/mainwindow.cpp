#include "mainwindow.h"
#include "ui_window.h" 
#include "timer_model.h"
#include "utils.h"
#include <iostream>

MainAppWindow::MainAppWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainAppWindow)
{
    m_ui->setupUi(this);

    m_timer_model = new TimerModel(this);

    connect(m_ui->startPauseButton, &QPushButton::clicked, m_timer_model, &TimerModel::startPauseTimer);
    connect(m_ui->stopButton, &QPushButton::clicked, m_timer_model, &TimerModel::stopTimer);

    connect(m_timer_model, &TimerModel::timerChanged, this, [this](int newTime){
        m_ui->timerText->setText(QString::fromStdString(utils::formatMs(newTime)));
    });

}

MainAppWindow::~MainAppWindow()
{
    delete m_ui;
}