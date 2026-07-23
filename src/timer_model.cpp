#include "timer_model.h"
#include <QTimer>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include "config.h"

TimerModel::TimerModel(QObject *parent) : QObject(parent)
{
    m_timer = new QTimer(this);
    m_timer->callOnTimeout([this]()
                           {
        if (m_in_work_session)
        {
            m_timer_stats.ms_worked += 1000;
            m_config.total_time_elapsed += 1000;
            m_db->updateEntry(ColumnType::time_worked, 1000);
            emit statsChanged(m_timer_stats);
        }
        this->modifyTime(-1000);
        emit this->timerChanged(m_time); });

    // Load config

    QString data_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir dir(data_path);
    if (!dir.exists() && !dir.mkpath(data_path))
    {
        qWarning() << "Failed to create data directory at " << data_path;
    }

    m_full_config_path = dir.filePath("config.json").toStdString();

    qInfo() << "Data directory located at " << data_path;

    if (QFile::exists(QString::fromStdString(m_full_config_path)))
    {
        qInfo() << "Config file found at" << m_full_config_path;
        
        m_config = Config::loadConfig(m_full_config_path);
    }
    else
    {

        qInfo() << "Writing default configuration file to " << m_full_config_path;

        QFile default_config(":resources/default_config.json");

        if (!default_config.copy(QString::fromStdString(m_full_config_path)))
            qWarning() << "Failed to copy default configuration file.";

        QFile::setPermissions(QString::fromStdString(m_full_config_path), QFileDevice::ReadOwner|QFileDevice::WriteOwner);

        m_config = Config::loadConfig(m_full_config_path);
    }

    m_time = m_config.work_session_duration;
    m_timer_stats.breaks = m_config.total_breaks;
    m_timer_stats.work_sessions = m_config.total_work_sessions;
    m_timer_stats.ms_worked = m_config.total_time_elapsed;

    // Instantiate database

    m_db = new Database(this);
    std::string full_db_path = dir.filePath("stats.db").toStdString();
    m_db->initDb(full_db_path);
    m_db->newEntry();

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
            m_time = m_config.short_break_duration;
        }
        else
        {
            m_work_sessions_elapsed = 0;
            m_time = m_config.long_break_duration;
        }

        m_timer_stats.work_sessions++;
        m_config.total_work_sessions++;
        m_db->updateEntry(ColumnType::sessions, 1);
    }
    else
    {

        m_in_work_session = true;
        m_time = m_config.work_session_duration;
        m_timer_stats.breaks++;
        m_config.total_breaks++;
        m_db->updateEntry(ColumnType::breaks, 1);
    }

    emit statsChanged(m_timer_stats);
    emit timerStateChanged(m_timer_state);
    emit timerChanged(m_time);
}

TimerStats TimerModel::getStats()
{
    return m_timer_stats;
}

bool TimerModel::inWorkSession()
{
    return m_in_work_session;
}

void TimerModel::saveConfig()
{
    Config::saveConfig(m_full_config_path, m_config);
}