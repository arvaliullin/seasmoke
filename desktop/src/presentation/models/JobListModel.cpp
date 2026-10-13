#include "JobListModel.h"

namespace presentation {

JobListModel::JobListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int JobListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_jobs.size();
}

QVariant JobListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_jobs.size())
        return {};

    const domain::Job &job = m_jobs.at(index.row());
    switch (role) {
    case IdRole:
        return QVariant::fromValue(job.id);
    case StatusRole:
        return job.status;
    case WorkerCountRole:
        return job.workerCount;
    case IsRunningRole:
        return job.status == QLatin1String("running");
    case SummaryRole:
        return QStringLiteral("Job #%1 - %2 (%3 workers)")
            .arg(job.id)
            .arg(job.status)
            .arg(job.workerCount);
    case WorkersRole: {
        QVariantList workers;
        workers.reserve(job.workers.size());
        for (const domain::WorkerResult &worker : job.workers) {
            workers.push_back(QVariantMap{
                {QStringLiteral("id"), worker.id},
                {QStringLiteral("result"), QVariant::fromValue(worker.result)},
                {QStringLiteral("elapsedMs"), QVariant::fromValue(worker.elapsedMs)},
            });
        }
        return workers;
    }
    default:
        return {};
    }
}

QHash<int, QByteArray> JobListModel::roleNames() const
{
    return {
        {IdRole, "jobId"},
        {StatusRole, "status"},
        {WorkerCountRole, "workerCount"},
        {WorkersRole, "workers"},
        {IsRunningRole, "isRunning"},
        {SummaryRole, "summary"},
    };
}

void JobListModel::setJobs(QVector<domain::Job> jobs)
{
    beginResetModel();
    m_jobs = std::move(jobs);
    endResetModel();
    emit countChanged();
}

} // namespace presentation
