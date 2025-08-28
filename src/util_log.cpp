#include "util_log.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace log {
    static std::shared_ptr<spdlog::logger> logger = nullptr;

    void init() {
        logger = spdlog::stdout_color_mt("llmchat");
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
        spdlog::set_level(spdlog::level::info);
    }

    std::shared_ptr<spdlog::logger> get() {
        return logger;
    }
}