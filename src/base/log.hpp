#pragma once

#include <QQueue>
#include <QString>
#include <QMutex>


// Colored escape sequences
#define INFO "\033[38;5;245m[INFO]:\033[0m %1"
#define WARN "\033[38;5;172m[WARN]:\033[0m %1"
#define CRIT "\033[38;5;124m[CRIT]:\033[0m %1"


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
