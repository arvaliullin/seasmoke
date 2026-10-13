#pragma once

#include "domain/Entities.h"

#include <QAbstractListModel>
#include <QVector>

namespace presentation {

class RequestListModel final : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum Roles {
        MethodRole = Qt::UserRole + 1,
        PathRole,
        StatusRole,
        LatencyMsRole,
        LineRole,
    };

    explicit RequestListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setRequests(QVector<domain::RequestEntry> requests, int maxItems = 20);

signals:
    void countChanged();

private:
    QVector<domain::RequestEntry> m_requests;
};

} // namespace presentation
