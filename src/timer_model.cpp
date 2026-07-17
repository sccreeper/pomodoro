#include "timer_model.h"
#include <QTimer>
#include <iostream>

TimerModel::TimerModel(QObject *parent) : QObject(parent) {
    m_timer = new QTimer(this);
    m_timer->callOnTimeout([this](){
        this->modifyTime(-1000);
        emit this->timerChanged(m_time);
    });
}

int TimerModel::getTime() {
    return m_time;
}

void TimerModel::modifyTime(int delta) {

    if (m_time + delta <= 0)
    {
        m_time = 0;
        this->stopTimer();
    } else {
        m_time += delta;
    }
    
}

void TimerModel::startPauseTimer() {

    if (m_timer_state == running)
    {
        std::cout << "Timer Active, pausing\n";
        
        m_timer->stop();
        m_timer_state = paused;
    
    } else {
        
        std::cout << "Timer Paused or stopped, starting!\n";

        m_timer->start(1000);
        m_timer_state = running;
    }

    emit timerStateChanged(m_timer_state);
    
}

void TimerModel::stopTimer() {
    m_timer->stop();
    m_time = DEFAULT_WORK_TIMER_VALUE;
    m_timer_state = stopped;

    emit timerStateChanged(m_timer_state);
}
