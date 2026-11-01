#include "services/news/NewsTranslationService.h"
#include "core/logging/Logger.h"
#include "services/llm/LlmService.h"
#include "storage/repositories/SettingsRepository.h"

namespace fincept::services {

NewsTranslationService& NewsTranslationService::instance() {
    static NewsTranslationService s;
    return s;
}

NewsTranslationService::NewsTranslationService() {}

bool NewsTranslationService::auto_translate_enabled() const {
    auto r = SettingsRepository::instance().get("news.auto_translate_es");
    return r.is_ok() && r.value() == "true";
}

void NewsTranslationService::set_auto_translate_enabled(bool on) {
    SettingsRepository::instance().set("news.auto_translate_es", on ? "true" : "false", "news");
}

void NewsTranslationService::translate_to_spanish(const QString& text, std::function<void(bool, QString)> cb) {
    if (text.trimmed().isEmpty()) {
        if (cb) cb(false, {});
        return;
    }
    // Use the active LLM provider configured by the user (Gemini, Groq, DeepSeek, Ollama, ...).
    // LlmService routes to the provider the user selected in Settings.
    QString prompt = QStringLiteral("Translate the following financial news text to Spanish. "
                                    "Return only the translated text, no explanations:\n\n") + text;

    // Placeholder: actual call goes through LlmService once the provider is set.
    // For now we log and return the original so the UI can be wired later.
    LOG_INFO("NewsTranslation", "Translation requested (LLM integration point)");
    if (cb) cb(true, text); // temporary: return original until full LlmService wiring
}

} // namespace fincept::services
