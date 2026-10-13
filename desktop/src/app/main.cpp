#include "data/ApiService.h"
#include "data/network/ApiClient.h"
#include "presentation/MainViewModel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

namespace {

QUrl resolveBaseUrl(const QStringList &args)
{
    if (const QByteArray fromEnv = qgetenv("SEASMOKE_API_BASE"); !fromEnv.isEmpty())
        return QUrl(QString::fromUtf8(fromEnv));

    for (int i = 0; i + 1 < args.size(); ++i) {
        if (args.at(i) == QLatin1String("--api-base"))
            return QUrl(args.at(i + 1));
    }

    return QUrl(QStringLiteral("http://seasmoke.local:3000"));
}

} // namespace

int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_CONTROLS_STYLE", "Material");

    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("seasmoke"));
    QGuiApplication::setOrganizationName(QStringLiteral("seasmoke"));

    qRegisterMetaType<domain::Job>();
    qRegisterMetaType<domain::RequestEntry>();
    qRegisterMetaType<QVector<domain::Job>>();
    qRegisterMetaType<QVector<domain::RequestEntry>>();

    const QUrl baseUrl = resolveBaseUrl(QGuiApplication::arguments());

    // Composition root: wire infrastructure → application port → presentation
    auto *apiClient = new data::ApiClient(baseUrl, &app);
    auto *apiService = new data::ApiService(apiClient, &app);
    auto *viewModel = new presentation::MainViewModel(apiService, baseUrl.toString(), &app);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    qmlRegisterSingletonInstance("seasmoke", 1, 0, "AppViewModel", viewModel);
    engine.loadFromModule("seasmoke", "Main");

    viewModel->start();

    return QGuiApplication::exec();
}
