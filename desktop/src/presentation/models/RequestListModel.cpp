#include "RequestListModel.h"

namespace presentation {

RequestListModel::RequestListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int RequestListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_requests.size();
}

QVariant RequestListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_requests.size())
        return {};

    const domain::RequestEntry &entry = m_requests.at(index.row());
    switch (role) {
    case MethodRole:
        return entry.method;
    case PathRole:
        return entry.path;
    case StatusRole:
        return entry.status;
    case LatencyMsRole:
        return QVariant::fromValue(entry.latencyMs);
    case LineRole:
        return QStringLiteral("%1 %2 %3 %4ms")
            .arg(entry.method, entry.path)
            .arg(entry.status)
            .arg(entry.latencyMs);
    default:
        return {};
    }
}

QHash<int, QByteArray> RequestListModel::roleNames() const
{
    return {
        {MethodRole, "method"},
        {PathRole, "path"},
        {StatusRole, "status"},
        {LatencyMsRole, "latencyMs"},
        {LineRole, "line"},
    };
}

void RequestListModel::setRequests(QVector<domain::RequestEntry> requests, int maxItems)
{
    if (maxItems >= 0 && requests.size() > maxItems)
        requests.resize(maxItems);

    beginResetModel();
    m_requests = std::move(requests);
    endResetModel();
    emit countChanged();
}

} // namespace presentation
