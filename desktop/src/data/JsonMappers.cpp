#include "JsonMappers.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace data {
namespace {

bool ensureArray(const QJsonDocument &doc, QString *error)
{
    if (!doc.isArray()) {
        if (error)
            *error = QStringLiteral("expected JSON array");
        return false;
    }
    return true;
}

domain::WorkerResult parseWorker(const QJsonObject &object)
{
    domain::WorkerResult worker;
    worker.id = static_cast<quint32>(object.value(QStringLiteral("id")).toInteger());
    worker.result = static_cast<quint64>(object.value(QStringLiteral("result")).toInteger());
    worker.elapsedMs = static_cast<quint64>(object.value(QStringLiteral("elapsed_ms")).toInteger());
    return worker;
}

domain::Job parseJob(const QJsonObject &object)
{
    domain::Job job;
    job.id = static_cast<quint64>(object.value(QStringLiteral("id")).toInteger());
    job.status = object.value(QStringLiteral("status")).toString();
    job.workerCount = static_cast<quint32>(object.value(QStringLiteral("worker_count")).toInteger());
    job.startedAt = static_cast<quint64>(object.value(QStringLiteral("started_at")).toInteger());

    const QJsonValue finished = object.value(QStringLiteral("finished_at"));
    if (!finished.isNull() && !finished.isUndefined())
        job.finishedAt = static_cast<quint64>(finished.toInteger());

    const QJsonArray workers = object.value(QStringLiteral("workers")).toArray();
    job.workers.reserve(workers.size());
    for (const QJsonValue &value : workers)
        job.workers.push_back(parseWorker(value.toObject()));

    return job;
}

domain::RequestEntry parseRequest(const QJsonObject &object)
{
    domain::RequestEntry entry;
    entry.method = object.value(QStringLiteral("method")).toString();
    entry.path = object.value(QStringLiteral("path")).toString();
    entry.status = static_cast<quint16>(object.value(QStringLiteral("status")).toInteger());
    entry.latencyMs = static_cast<quint64>(object.value(QStringLiteral("latency_ms")).toInteger());
    entry.atUnixMs = static_cast<quint64>(object.value(QStringLiteral("at_unix_ms")).toInteger());
    return entry;
}

} // namespace

std::optional<QString> parseHealthStatus(const QByteArray &json, QString *error)
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        if (error)
            *error = parseError.errorString();
        return std::nullopt;
    }
    if (!doc.isObject()) {
        if (error)
            *error = QStringLiteral("expected JSON object");
        return std::nullopt;
    }

    const QString status = doc.object().value(QStringLiteral("status")).toString();
    if (status.isEmpty())
        return QStringLiteral("unknown");
    return status;
}

std::optional<QVector<domain::Job>> parseJobs(const QByteArray &json, QString *error)
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        if (error)
            *error = parseError.errorString();
        return std::nullopt;
    }
    if (!ensureArray(doc, error))
        return std::nullopt;

    QVector<domain::Job> jobs;
    const QJsonArray array = doc.array();
    jobs.reserve(array.size());
    for (const QJsonValue &value : array)
        jobs.push_back(parseJob(value.toObject()));
    return jobs;
}

std::optional<QVector<domain::RequestEntry>> parseRequests(const QByteArray &json, QString *error)
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        if (error)
            *error = parseError.errorString();
        return std::nullopt;
    }
    if (!ensureArray(doc, error))
        return std::nullopt;

    QVector<domain::RequestEntry> requests;
    const QJsonArray array = doc.array();
    requests.reserve(array.size());
    for (const QJsonValue &value : array)
        requests.push_back(parseRequest(value.toObject()));
    return requests;
}

std::optional<quint64> parseFanoutJobId(const QByteArray &json, QString *error)
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        if (error)
            *error = parseError.errorString();
        return std::nullopt;
    }
    if (!doc.isObject()) {
        if (error)
            *error = QStringLiteral("expected JSON object");
        return std::nullopt;
    }

    return static_cast<quint64>(doc.object().value(QStringLiteral("job_id")).toInteger());
}

} // namespace data
