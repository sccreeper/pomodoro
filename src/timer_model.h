#pragma once

#include <QObject>
#include <QTimer>

enum TimerState {running, paused, stopped};

const int DEFAULT_WORK_TIMER_VALUE = 25 * 60 * 1000;
const int DEFAULT_BREAK_TIMER_VALUE = 5 * 60 * 1000;
const int DEFAULT_LONG_BREAK_TIMER_VALUE = 15 * 60 * 1000;

class TimerModel : public QObject {
    Q_OBJECT

public:
    explicit TimerModel(QObject *parent = nullptr);

    int getTime();

public slots:
    void startPauseTimer();
    void stopTimer();

signals:
    void timerChanged(int newTime);
    void timerStateChanged(TimerState state);

private:
    /// @brief Current time left on timer, in msec
    int m_time = DEFAULT_WORK_TIMER_VALUE;
    /// @brief QTimer object used for intervals
    QTimer *m_timer;

    TimerState m_timer_state = stopped;

    void modifyTime(int delta);
};