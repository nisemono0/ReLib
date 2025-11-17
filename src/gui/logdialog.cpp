#include "gui/logdialog.hpp"

#include "base/log.hpp"

#include "utils/str.hpp"

#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>


LogDialog::LogDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LogDialog) {
    this->ui->setupUi(this);

    // Process log queue timer
    this->log_timer = new QTimer(this);
    this->log_timer->setInterval(1);

    connect(this->log_timer, &QTimer::timeout, this, &LogDialog::log_timer_timeout);
    this->log_timer->start();

    // Dialog buttons
    connect(this->ui->saveLogButton, &QPushButton::pressed, this, &LogDialog::saveLogButton_pressed);
    connect(this->ui->clearLogButton, &QPushButton::pressed, this, &LogDialog::clearLogButton_pressed);
    // Log text changed
    connect(this->ui->logPlainTextEdit, &QPlainTextEdit::blockCountChanged, this, &LogDialog::logPlainTextEdit_blockcountchanged);
    // Close window on close button press
    connect(this->ui->pushButtonCloseDialog, &QPushButton::clicked, this, &LogDialog::hide);
}

LogDialog::~LogDialog() {
    this->log_timer->deleteLater();
    delete this->ui;
}

void LogDialog::receive_showLogDialog_request() {
    if (this->isVisible()) {
        this->activateWindow();
    } else {
        this->show();
    }
}

void LogDialog::saveLogButton_pressed() {
    QFileDialog file_dialog = QFileDialog(this, QStringLiteral("Select file"), QDir::homePath());
    file_dialog.setAcceptMode(QFileDialog::AcceptSave);
    file_dialog.setFileMode(QFileDialog::AnyFile);
    file_dialog.setNameFilters({
            QStringLiteral("Log files (*.log)"),
            QStringLiteral("Text files (*.txt)")
            });
    file_dialog.setDefaultSuffix(QStringLiteral(".log"));

    QString save_path = QString();
    if (file_dialog.exec()) {
        save_path = file_dialog.selectedFiles()[0];
    }

    if (Utils::Str::isNullOrEmpty(save_path)) {
        return;
    }

    QFile file(save_path);
    if (file.open(QFile::ReadWrite)) {
        QTextStream textStream(&file);
        textStream << this->ui->logPlainTextEdit->toPlainText();
        textStream.flush();
        if (textStream.status()) {
            QMessageBox::information(this, QStringLiteral("Info"), QStringLiteral("Log written to file"));
        } else if (textStream.status() == QTextStream::WriteFailed) {
            QMessageBox::warning(this, QStringLiteral("Warning"), QStringLiteral("Couldn't write log file"));
        }
        file.close();
    }
}

void LogDialog::clearLogButton_pressed() {
    this->ui->logPlainTextEdit->clear();
}

void LogDialog::logPlainTextEdit_blockcountchanged(int newBlockCount) {
    this->ui->logStatusLabel->setText(QStringLiteral("Lines: %1").arg(QString::number(newBlockCount)));
}

void LogDialog::log_timer_timeout() {
    if (!Log::isLogQueueEmpty()) {
        this->ui->logPlainTextEdit->appendHtml(Log::getQueuedMessage());
    }
}

