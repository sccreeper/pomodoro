#include "statsdialog.h"
#include <QWidget>
#include <QChartView>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <format>
#include <chrono>
#include "utils.h"

StatsDialog::StatsDialog(TimerModel *model, std::chrono::seconds data_start, std::chrono::seconds data_end, QWidget *parent) : QDialog(parent), m_ui(new Ui::StatsDialog)
{

    m_ui->setupUi(this);
    m_chart = new QChart();
    m_model = model;

    if (data_start > data_end)
    {
        throw std::invalid_argument("start cannot be greater than end");
    }
    

    m_start_end[0] = data_start;
    m_start_end[1] = data_end;

    // Initial ch
    m_chart->setTitle("Time Worked");
    m_axis_x = new QBarCategoryAxis();
    m_axis_y = new QValueAxis();
    m_chart->addAxis(m_axis_x, Qt::AlignBottom);
    m_chart->addAxis(m_axis_y, Qt::AlignLeft);
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);

    m_ui->chart->setChart(m_chart);
    m_ui->chart->setRenderHint(QPainter::Antialiasing);

    m_ui->selected_stat->addItem("Sessions", QVariant(ColumnType::sessions));
    m_ui->selected_stat->addItem("Breaks", QVariant(ColumnType::breaks));
    m_ui->selected_stat->addItem("Time worked", QVariant(ColumnType::time_worked));
    m_ui->selected_stat->setCurrentIndex(0);

    connect(m_ui->selected_stat, &QComboBox::currentIndexChanged, this, &StatsDialog::refreshData);

    refreshData();
}

void StatsDialog::refreshData()
{
    
    m_chart->setTitle(
        std::format("{} from {} to {}",
                    m_ui->selected_stat->currentText().toStdString(),
                    utils::formatDDMMYYY(m_start_end[0].count() * 1000),
                    utils::formatDDMMYYY(m_start_end[1].count() * 1000))
            .c_str());
    m_chart->removeAllSeries();

    auto data = m_model->getData(
        static_cast<ColumnType>(m_ui->selected_stat->currentData().toInt()), 
        m_start_end[0], 
        m_start_end[1]
    );
    
    QStringList days;
    for (auto &&i : data)
    {
        days << utils::formatDDMMYYY(i[0] * 1000).c_str();
    }

    QBarSet *set = new QBarSet(m_ui->selected_stat->currentText());
    for (auto &&i : data)
    {
        *set << i[1];
    }

    m_axis_x->clear();
    m_axis_x->append(days);
    
    QBarSeries *series = new QBarSeries();
    series->append(set);

    auto max_item = std::max_element(data.begin(), data.end(), [](auto &&a, auto &&b) { return a[1] < b[1]; });
    qreal max_y = (max_item != data.end()) ? (*max_item)[1] : 0;
    m_axis_y->setRange(0, max_y * 1.1);

    m_chart->addSeries(series);
    series->attachAxis(m_axis_x);
    series->attachAxis(m_axis_y);

}

StatsDialog::~StatsDialog()
{
    delete m_ui;
}