#pragma once

#include "ui/ui_logdialog.h"

#include "utils/defs.hpp"

#include <QDialog>
#include <QWidget>
#include <QThread>
#include <QString>
#include <QStringList>
#include <QProgressDialog>
#include <QPointer>
#include <QQueue>
#include <QTimer>


namespace Ui {
    class LogDialog;
}

class LogDialog : public QDialog {
    Q_OBJECT;
public:
    explicit LogDialog(QWidget *parent = nullptr);
    ~LogDialog() override;


private:
    Ui::LogDialog *ui;

    // Log queue timer
    QTimer *log_timer;

public slots:
    void receive_showLogDialog_request();

private slots:
    // Logdialog buttons
    void saveLogButton_pressed();
    void clearLogButton_pressed();

    // Text changed
    void logPlainTextEdit_blockcountchanged(int newBlockCount);

    // Process log queue loop
    void log_timer_timeout();
};

