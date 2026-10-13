#include "ApiClient.h"

#include <QNetworkRequest>

namespace data {

ApiClient::ApiClient(QUrl baseUrl, QObject *parent)
    : QObject(parent)
    , m_baseUrl(std::move(baseUrl))
{
    m_baseUrl = m_baseUrl.adjusted(QUrl::StripTrailingSlash);
}

QUrl ApiClient::baseUrl() const
{
    return m_baseUrl;
}

void ApiClient::setBaseUrl(const QUrl &baseUrl)
{
    m_baseUrl = baseUrl.adjusted(QUrl::StripTrailingSlash);
}

void ApiClient::get(const QString &path, ResponseCallback callback)
{
    QNetworkRequest request(m_baseUrl.resolved(QUrl(path)));
    request.setRawHeader("Accept", "application/json");
    send(m_nam.get(request), std::move(callback));
}

void ApiClient::postJson(const QString &path, const QByteArray &jsonBody, ResponseCallback callback)
{
    QNetworkRequest request(m_baseUrl.resolved(QUrl(path)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Accept", "application/json");
    send(m_nam.post(request, jsonBody), std::move(callback));
}

void ApiClient::send(QNetworkReply *reply, ResponseCallback callback)
{
    connect(reply, &QNetworkReply::finished, this, [reply, callback = std::move(callback)]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError
            && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).isNull()) {
            callback(0, {}, reply->errorString());
            return;
        }

        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        callback(status, reply->readAll(), {});
    });
}

} // namespace data
