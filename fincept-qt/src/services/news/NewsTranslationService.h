#pragma once
// NewsTranslationService — translate headlines/summaries to Spanish using the
// user's configured LLM (Gemini, Groq, DeepSeek, Ollama, etc.).
// Offline mode: no Fincept LLM.

#include <QObject>
#include <QString>
#include <functional>

namespace fincept::services {

class NewsTranslationService : public QObject {
    Q_OBJECT
  public:
    static NewsTranslationService& instance();

    /// Translate text to Spanish. cb receives (ok, translated_text).
    void translate_to_spanish(const QString& text, std::function<void(bool, QString)> cb);

    /// Whether automatic translation is enabled (settings key "news.auto_translate_es").
    bool auto_translate_enabled() const;
    void set_auto_translate_enabled(bool on);

  private:
    NewsTranslationService();
};

} // namespace fincept::services
