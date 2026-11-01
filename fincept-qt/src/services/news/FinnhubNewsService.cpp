#include "services/news/FinnhubNewsService.h"
#include "core/logging/Logger.h"
#include "storage/repositories/SettingsRepository.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrlQuery>

namespace fincept::services {

FinnhubNewsService& FinnhubNewsService::instance() {
    static FinnhubNewsService s;
    return s;
}

FinnhubNewsService::FinnhubNewsService() {}

QString FinnhubNewsService::api_key() const {
    auto r = SettingsRepository::instance().get("finnhub_api_key");
    if (r.is_ok() && !r.value().isEmpty())
        return r.value();
    return qEnvironmentVariable("FINNHUB_API_KEY");
}

void FinnhubNewsService::fetch_general(std::function<void(bool, QVector<FinnhubNewsItem>)> cb) {
    const QString key = api_key();
    if (key.isEmpty()) {
        LOG_INFO("FinnhubNews", "No FINNHUB_API_KEY configured — skipping");
        if (cb) cb(false, {});
        return;
    }

    auto* nam = new QNetworkAccessManager(this);
    QUrl url("https://finnhub.io/api/v1/news");
    QUrlQuery q;
    q.addQueryItem("category", "general");
    q.addQueryItem("token", key);
    url.setQuery(q);

    auto* reply = nam->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, [this, reply, cb = std::move(cb)]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            LOG_WARN("FinnhubNews", "Request failed: " + reply->errorString());
            if (cb) cb(false, {});
            return;
        }
        auto doc = QJsonDocument::fromJson(reply->readAll());
        QVector<FinnhubNewsItem> items;
        if (doc.isArray()) {
            for (const auto& v : doc.array()) {
                auto o = v.toObject();
                FinnhubNewsItem it;
                it.headline = o["headline"].toString();
                it.summary = o["summary"].toString();
                it.source = o["source"].toString();
                it.url = o["url"].toString();
                it.datetime = o["datetime"].toVariant().toLongLong();
                items.append(it);
            }
        }
        if (cb) cb(true, items);
    });
}

} // namespace fincept::services
