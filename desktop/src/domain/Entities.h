#pragma once

#include <QMetaType>
#include <QString>
#include <QVector>
#include <optional>

namespace domain {

struct WorkerResult {
    quint32 id = 0;
    quint64 result = 0;
    quint64 elapsedMs = 0;
};

struct Job {
    quint64 id = 0;
    QString status;
    quint32 workerCount = 0;
    QVector<WorkerResult> workers;
    quint64 startedAt = 0;
    std::optional<quint64> finishedAt;
};

struct RequestEntry {
    QString method;
    QString path;
    quint16 status = 0;
    quint64 latencyMs = 0;
    quint64 atUnixMs = 0;
};

} // namespace domain

Q_DECLARE_METATYPE(domain::Job)
Q_DECLARE_METATYPE(domain::RequestEntry)
Q_DECLARE_METATYPE(QVector<domain::Job>)
Q_DECLARE_METATYPE(QVector<domain::RequestEntry>)
