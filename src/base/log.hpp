#pragma once

#include <QQueue>
#include <QString>
#include <QMutex>


class Log {
public:
    static void info(const QString &message);
    static void warning(const QString &message);
    static void error(const QString &message);
    static bool isLogQueueEmpty();
    static QString getQueuedMessage();
private:
    static QQueue<QString> log_queue;
    static QMutex qmutex;
};
