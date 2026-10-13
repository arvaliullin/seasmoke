#pragma once

#include "domain/Entities.h"

#include <QByteArray>
#include <QString>
#include <QVector>
#include <optional>

namespace data {

std::optional<QString> parseHealthStatus(const QByteArray &json, QString *error);
std::optional<QVector<domain::Job>> parseJobs(const QByteArray &json, QString *error);
std::optional<QVector<domain::RequestEntry>> parseRequests(const QByteArray &json, QString *error);
std::optional<quint64> parseFanoutJobId(const QByteArray &json, QString *error);

} // namespace data
