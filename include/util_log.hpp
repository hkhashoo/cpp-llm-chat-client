#pragma once
#include <memory>
#include <spdlog/spdlog.h>

namespace log {
    void init();
    std::shared_ptr<spdlog::logger> get();
}