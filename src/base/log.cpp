#include "base/log.hpp"

#include <QMutexLocker>

void Log::info(const QString &message) {
    QMutexLocker locker(&Log::qmutex);
    Log::log_queue.enqueue(QStringLiteral("<font color=\"gray\">[INFO]: </font>%1").arg(message));
}

void Log::warning(const QString &message) {
    QMutexLocker locker(&Log::qmutex);
    Log::log_queue.enqueue(QStringLiteral("<font color=\"orange\">[WARN]: </font>%1").arg(message));
}

void Log::error(const QString &message) {
    QMutexLocker locker(&Log::qmutex);
    Log::log_queue.enqueue(QStringLiteral("<font color=\"red\">[CRIT]: </font>%1").arg(message));
}

bool Log::isLogQueueEmpty() {
    QMutexLocker locker(&Log::qmutex);
    return Log::log_queue.isEmpty();
}

QString Log::getQueuedMessage() {
    QMutexLocker locker(&Log::qmutex);
    if (!Log::log_queue.isEmpty()) {
        return Log::log_queue.dequeue();
    }
    return QString();
}

