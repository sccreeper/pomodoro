#include "settingsdialog.h"
#include "ui_settings.h"
#include <QWidget>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), m_ui(new Ui::SettingsDialog)
{

    this->setWindowIcon(QIcon(":resources/icon.png"));
    this->setWindowTitle("Settings");
    m_ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

int SettingsDialog::getWorkSessionDuration()
{
    return m_ui->work_session_input->value() * 60 * 1000;
}

void SettingsDialog::setWorkSessionDuration(int value) {
    m_ui->work_session_input->setValue(value / 60 / 1000);
}

int SettingsDialog::getShortBreakDuration()
{
    return m_ui->short_break_input->value() * 60 * 1000;
}

void SettingsDialog::setShortBreakDuration(int value) {
    m_ui->short_break_input->setValue(value / 60 / 1000);
}

int SettingsDialog::getLongBreakDuration()
{
    return m_ui->long_break_input->value() * 60 * 1000;
}

void SettingsDialog::setLongBreakDuration(int value) {
    m_ui->long_break_input->setValue(value / 60 / 1000);
}