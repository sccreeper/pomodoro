#pragma once

#include <QObject>
#include <QTimer>
#include "config.h"
#include <string>

enum TimerState {running, paused, stopped};

const int DEFAULT_WORK_TIMER_VALUE = 25 * 60 * 1000;
const int DEFAULT_BREAK_TIMER_VALUE = 5 * 60 * 1000;
const int DEFAULT_LONG_BREAK_TIMER_VALUE = 15 * 60 * 1000;

struct TimerStats {
    int work_sessions;
    int breaks;
    int ms_worked;
};

class TimerModel : public QObject {
    Q_OBJECT

public:
    explicit TimerModel(QObject *parent = nullptr);

    int getTime();
    TimerStats getStats();
    bool inWorkSession();

    Config::PomoConfig m_config{};
    void saveConfig();

public slots:
    void startPauseTimer();
    void stopTimer();

signals:
    void timerChanged(int newTime);
    void timerStateChanged(TimerState state);
    void statsChanged(TimerStats newStats);

private:
    /// @brief Current time left on timer, in msec
    int m_time = DEFAULT_WORK_TIMER_VALUE;
    /// @brief QTimer object used for intervals
    QTimer *m_timer;

    TimerState m_timer_state = stopped;

    int m_work_sessions_elapsed = 0;
    bool m_in_work_session = true;

    std::string m_full_config_path;
    
    TimerStats m_timer_stats{};

    void modifyTime(int delta);
};