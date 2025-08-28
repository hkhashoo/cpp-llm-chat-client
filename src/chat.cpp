#include "chat.hpp"
#include <cstdlib>
#include <iostream>
#include "util_log.hpp"


static int env_int_or(const char* k, int dflt) {
    if (const char* v = std::getenv(k)) {
        try { return std::stoi(v); } catch (...) {}
    }
    return dflt;
}

std::string Chat::reply(const std::string& user_input) {

    log::get()->info("User: {}", user_input);
    // add user message
    history_.push_back({"user", user_input});

    // trim to last N
    history_limit_ = env_int_or("HISTORY_LIMIT", history_limit_);
    if (history_.size() > static_cast<size_t>(history_limit_)) {
        const auto trim = history_.size() - static_cast<size_t>(history_limit_);
        history_.erase(history_.begin(), history_.begin() + trim);
    }

    // call LLM
        const int timeout_ms = env_int_or("TIMEOUT_MS", 15000);
    const bool stream = []{
        if (const char* v = std::getenv("STREAM")) {
            return std::string(v) == "1" || std::string(v) == "true" || std::string(v) == "TRUE";
        }
        return false;
    }();

    if (stream) {
        std::string printed = "";
        auto r = ollama_chat_stream(
            history_,
            [&](const std::string& chunk){ std::cout << chunk << std::flush; printed += chunk; },
            timeout_ms
        );
        std::cout << "\n";
        if (r.ok) {
            log::get()->info("Assistant: {}...", printed.substr(0, 40));
            history_.push_back({"assistant", printed});
            return printed;
        }
        log::get()->error("Assistant error: {}...", r.error);
        return std::string("[error] ") + r.error;
    } else {
        auto r = ollama_chat(history_, timeout_ms);
        if (r.ok) {
            log::get()->info("Assistant: {}...", r.text.substr(0, 40));
            history_.push_back({"assistant", r.text});
            return r.text;
        }
        log::get()->error("Assistant error: {}...", r.error);
        return std::string("[error] ") + r.error;
    }
}
