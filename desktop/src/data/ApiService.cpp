#include "ApiService.h"

#include "JsonMappers.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace data {

ApiService::ApiService(ApiClient *client, QObject *parent)
    : domain::IApiService(parent)
    , m_client(client)
{
    Q_ASSERT(m_client != nullptr);
}

void ApiService::fetchHealth()
{
    m_client->get(QStringLiteral("/api/health"), [this](int status, const QByteArray &body, const QString &error) {
        if (!error.isEmpty()) {
            emit healthFailed(error);
            return;
        }
        if (status < 200 || status >= 300) {
            emit healthFailed(QStringLiteral("HTTP %1").arg(status));
            return;
        }

        QString parseError;
        const auto health = parseHealthStatus(body, &parseError);
        if (!health) {
            emit healthFailed(parseError.isEmpty() ? QStringLiteral("invalid JSON") : parseError);
            return;
        }
        emit healthReceived(*health);
    });
}

void ApiService::fetchJobs()
{
    m_client->get(QStringLiteral("/api/demo/jobs"), [this](int status, const QByteArray &body, const QString &error) {
        if (!error.isEmpty() || status < 200 || status >= 300)
            return;

        const auto jobs = parseJobs(body, nullptr);
        if (jobs)
            emit jobsReceived(*jobs);
    });
}

void ApiService::fetchRequests()
{
    m_client->get(QStringLiteral("/api/requests"), [this](int status, const QByteArray &body, const QString &error) {
        if (!error.isEmpty() || status < 200 || status >= 300)
            return;

        const auto requests = parseRequests(body, nullptr);
        if (requests)
            emit requestsReceived(*requests);
    });
}

void ApiService::startFanout(quint32 workers)
{
    QJsonObject payload;
    payload.insert(QStringLiteral("workers"), static_cast<int>(workers));
    const QByteArray body = QJsonDocument(payload).toJson(QJsonDocument::Compact);

    m_client->postJson(QStringLiteral("/api/demo/fanout"), body,
                       [this](int status, const QByteArray &responseBody, const QString &error) {
                           if (!error.isEmpty()) {
                               emit fanoutFailed(error);
                               return;
                           }
                           if (status < 200 || status >= 300) {
                               emit fanoutFailed(QStringLiteral("HTTP %1").arg(status));
                               return;
                           }

                           QString parseError;
                           const auto jobId = parseFanoutJobId(responseBody, &parseError);
                           if (!jobId) {
                               emit fanoutFailed(parseError.isEmpty() ? QStringLiteral("invalid JSON")
                                                                      : parseError);
                               return;
                           }
                           emit fanoutStarted(*jobId);
                       });
}

} // namespace data
