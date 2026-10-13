#pragma once

#include "domain/IApiService.h"
#include "models/JobListModel.h"
#include "models/RequestListModel.h"

#include <QObject>
#include <QTimer>

namespace presentation {

/// Presentation layer: state + intents for the main screen.
class MainViewModel final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString health READ health NOTIFY healthChanged)
    Q_PROPERTY(bool starting READ starting NOTIFY startingChanged)
    Q_PROPERTY(QString baseUrl READ baseUrl CONSTANT)
    Q_PROPERTY(presentation::JobListModel *jobs READ jobs CONSTANT)
    Q_PROPERTY(presentation::RequestListModel *requests READ requests CONSTANT)

public:
    explicit MainViewModel(domain::IApiService *api,
                           QString baseUrl,
                           QObject *parent = nullptr);

    QString health() const;
    bool starting() const;
    QString baseUrl() const;
    JobListModel *jobs();
    RequestListModel *requests();

    Q_INVOKABLE void start();
    Q_INVOKABLE void refreshHealth();
    Q_INVOKABLE void refreshAll();
    Q_INVOKABLE void runFanout(int workers = 8);

signals:
    void healthChanged();
    void startingChanged();

private:
    void setHealth(const QString &value);
    void setStarting(bool value);

    domain::IApiService *m_api = nullptr;
    QString m_baseUrl;
    QString m_health = QStringLiteral("loading...");
    bool m_starting = false;
    JobListModel m_jobs;
    RequestListModel m_requests;
    QTimer m_pollTimer;
};

} // namespace presentation
