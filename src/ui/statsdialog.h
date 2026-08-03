#pragma once

#include "ui_stats.h"
#include <QChart>
#include <QLineSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <chrono>
#include <array>
#include <vector>
#include "timer_model.h"

QT_BEGIN_NAMESPACE
namespace Ui {class StatsDialog;}
QT_END_NAMESPACE

class StatsDialog : public QDialog {
    Q_OBJECT

    public:
        explicit StatsDialog(TimerModel *model, std::chrono::seconds data_start, std::chrono::seconds data_end, QWidget *parent = nullptr);
        ~StatsDialog();

    public slots:
        void refreshData();

    private:
        Ui::StatsDialog *m_ui;
        std::array<std::chrono::seconds, 2> m_start_end;
        TimerModel *m_model;

        QChart *m_chart;
        QBarCategoryAxis *m_axis_x;
        QValueAxis *m_axis_y;
};