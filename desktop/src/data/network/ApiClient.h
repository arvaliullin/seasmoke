#pragma once

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QUrl>
#include <functional>

namespace data {

/// Low-level HTTP transport. No domain knowledge.
class ApiClient : public QObject
{
    Q_OBJECT

public:
    using ResponseCallback = std::function<void(int status, const QByteArray &body, const QString &error)>;

    explicit ApiClient(QUrl baseUrl, QObject *parent = nullptr);

    QUrl baseUrl() const;
    void setBaseUrl(const QUrl &baseUrl);

    void get(const QString &path, ResponseCallback callback);
    void postJson(const QString &path, const QByteArray &jsonBody, ResponseCallback callback);

private:
    void send(QNetworkReply *reply, ResponseCallback callback);

    QUrl m_baseUrl;
    QNetworkAccessManager m_nam;
};

} // namespace data
