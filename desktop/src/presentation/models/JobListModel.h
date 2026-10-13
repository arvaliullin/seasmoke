#pragma once

#include "domain/Entities.h"

#include <QAbstractListModel>
#include <QVector>

namespace presentation {

class JobListModel final : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        StatusRole,
        WorkerCountRole,
        WorkersRole,
        IsRunningRole,
        SummaryRole,
    };

    explicit JobListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setJobs(QVector<domain::Job> jobs);

signals:
    void countChanged();

private:
    QVector<domain::Job> m_jobs;
};

} // namespace presentation
