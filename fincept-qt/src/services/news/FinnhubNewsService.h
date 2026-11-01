#pragma once
// FinnhubNewsService — optional free news source using the user's own FINNHUB_API_KEY.
// Reads the key from SettingsRepository ("finnhub_api_key") or the environment variable.
// Does not require Fincept servers.

#include <QObject>
#include <QString>
#include <QVector>

namespace fincept::services {

struct FinnhubNewsItem {
    QString headline;
    QString summary;
    QString source;
    QString url;
    qint64 datetime = 0; // unix seconds
};

class FinnhubNewsService : public QObject {
    Q_OBJECT
  public:
    static FinnhubNewsService& instance();
    void fetch_general(std::function<void(bool, QVector<FinnhubNewsItem>)> cb);
    QString api_key() const;

  private:
    FinnhubNewsService();
};

} // namespace fincept::services
