#pragma once

#include "Entities.h"

#include <QObject>
#include <QVector>

namespace domain {

/// Port: application boundary for SeaSmoke backend API.
class IApiService : public QObject
{
    Q_OBJECT

public:
    explicit IApiService(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    ~IApiService() override = default;

    virtual void fetchHealth() = 0;
    virtual void fetchJobs() = 0;
    virtual void fetchRequests() = 0;
    virtual void startFanout(quint32 workers) = 0;

signals:
    void healthReceived(const QString &status);
    void healthFailed(const QString &error);

    void jobsReceived(const QVector<domain::Job> &jobs);
    void requestsReceived(const QVector<domain::RequestEntry> &requests);

    void fanoutStarted(quint64 jobId);
    void fanoutFailed(const QString &error);
};

} // namespace domain
