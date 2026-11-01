#include "services/news/NewsTranslationService.h"
#include "core/logging/Logger.h"
#include "services/llm/LlmService.h"
#include "storage/repositories/SettingsRepository.h"

#include <QtConcurrent>

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
    if (!LlmService::instance().is_configured()) {
        LOG_WARN("NewsTranslation", "No LLM configured — cannot translate");
        if (cb) cb(false, text);
        return;
    }

    const QString prompt = QStringLiteral(
        "Translate the following financial news text to Spanish. "
        "Return ONLY the translated text, with no quotes, explanations or extra commentary:\n\n") + text;

    // Blocking chat() must run off the GUI thread.
    QtConcurrent::run([prompt, cb = std::move(cb)]() {
        auto resp = LlmService::instance().chat(prompt, {}, /*use_tools=*/false);
        const bool ok = !resp.error.isEmpty() ? false : !resp.content.isEmpty();
        const QString out = ok ? resp.content.trimmed() : prompt; // fallback to original on failure
        if (cb)
            cb(ok, out);
    });
}

} // namespace fincept::services
