#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {class SettingsDialog;}
QT_END_NAMESPACE

class SettingsDialog : public QDialog {
    Q_OBJECT

    public:
        explicit SettingsDialog(QWidget *parent = nullptr);
        ~SettingsDialog();
        int getWorkSessionDuration();
        void setWorkSessionDuration(int value);
        int getShortBreakDuration();
        void setShortBreakDuration(int value);
        int getLongBreakDuration();
        void setLongBreakDuration(int value);

    private:
        Ui::SettingsDialog *m_ui;
};