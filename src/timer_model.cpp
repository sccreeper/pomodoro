#include "timer_model.h"
#include <QTimer>

TimerModel::TimerModel(QObject *parent) : QObject(parent)
{
    m_timer = new QTimer(this);
    m_timer->callOnTimeout([this]()
                           {
        if (m_in_work_session)
        {
            m_timer_stats.ms_worked += 1000;
            emit statsChanged(m_timer_stats);
        }
        this->modifyTime(-1000);
        emit this->timerChanged(m_time); });
}

int TimerModel::getTime()
{
    return m_time;
}

void TimerModel::modifyTime(int delta)
{

    if (m_time + delta <= 0)
    {
        m_time = 0;
        this->stopTimer();
    }
    else
    {
        m_time += delta;
    }
}

void TimerModel::startPauseTimer()
{

    if (m_timer_state == running)
    {

        m_timer->stop();
        m_timer_state = paused;
    }
    else
    {

        m_timer->start(1000);
        m_timer_state = running;
    }

    emit timerStateChanged(m_timer_state);
}

void TimerModel::stopTimer()
{
    m_timer->stop();
    m_timer_state = stopped;

    if (m_in_work_session)
    {
        m_work_sessions_elapsed++;
        m_in_work_session = false;

        if (m_work_sessions_elapsed < 4)
        {
            m_time = DEFAULT_BREAK_TIMER_VALUE;
        }
        else
        {
            m_work_sessions_elapsed = 0;
            m_time = DEFAULT_LONG_BREAK_TIMER_VALUE;
        }

        m_timer_stats.work_sessions++;
    }
    else
    {

        m_in_work_session = true;
        m_time = DEFAULT_WORK_TIMER_VALUE;
        m_timer_stats.breaks++;
    }

    emit statsChanged(m_timer_stats);
    emit timerStateChanged(m_timer_state);
    emit timerChanged(m_time);
}

TimerStats TimerModel::getStats()
{
    return m_timer_stats;
}
