#pragma once

#include "domain/IApiService.h"
#include "network/ApiClient.h"

namespace data {

/// Infrastructure adapter: maps HTTP/JSON to domain port.
class ApiService final : public domain::IApiService
{
    Q_OBJECT

public:
    explicit ApiService(ApiClient *client, QObject *parent = nullptr);

    void fetchHealth() override;
    void fetchJobs() override;
    void fetchRequests() override;
    void startFanout(quint32 workers) override;

private:
    ApiClient *m_client = nullptr;
};

} // namespace data
