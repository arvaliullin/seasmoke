#include "MainViewModel.h"

namespace presentation {

MainViewModel::MainViewModel(domain::IApiService *api, QString baseUrl, QObject *parent)
    : QObject(parent)
    , m_api(api)
    , m_baseUrl(std::move(baseUrl))
    , m_jobs(this)
    , m_requests(this)
{
    Q_ASSERT(m_api != nullptr);

    connect(m_api, &domain::IApiService::healthReceived, this, [this](const QString &status) {
        setHealth(status);
    });
    connect(m_api, &domain::IApiService::healthFailed, this, [this](const QString &error) {
        setHealth(error);
    });
    connect(m_api, &domain::IApiService::jobsReceived, this, [this](const QVector<domain::Job> &jobs) {
        m_jobs.setJobs(jobs);
    });
    connect(m_api, &domain::IApiService::requestsReceived, this,
            [this](const QVector<domain::RequestEntry> &requests) {
                m_requests.setRequests(requests);
            });
    connect(m_api, &domain::IApiService::fanoutStarted, this, [this](quint64) {
        setStarting(false);
        refreshAll();
    });
    connect(m_api, &domain::IApiService::fanoutFailed, this, [this](const QString &) {
        setStarting(false);
    });

    m_pollTimer.setInterval(2000);
    connect(&m_pollTimer, &QTimer::timeout, this, &MainViewModel::refreshAll);
}

QString MainViewModel::health() const
{
    return m_health;
}

bool MainViewModel::starting() const
{
    return m_starting;
}

QString MainViewModel::baseUrl() const
{
    return m_baseUrl;
}

JobListModel *MainViewModel::jobs()
{
    return &m_jobs;
}

RequestListModel *MainViewModel::requests()
{
    return &m_requests;
}

void MainViewModel::start()
{
    refreshHealth();
    refreshAll();
    m_pollTimer.start();
}

void MainViewModel::refreshHealth()
{
    m_api->fetchHealth();
}

void MainViewModel::refreshAll()
{
    m_api->fetchJobs();
    m_api->fetchRequests();
}

void MainViewModel::runFanout(int workers)
{
    if (m_starting)
        return;

    setStarting(true);
    const quint32 count = static_cast<quint32>(qBound(1, workers, 32));
    m_api->startFanout(count);
}

void MainViewModel::setHealth(const QString &value)
{
    if (m_health == value)
        return;
    m_health = value;
    emit healthChanged();
}

void MainViewModel::setStarting(bool value)
{
    if (m_starting == value)
        return;
    m_starting = value;
    emit startingChanged();
}

} // namespace presentation
