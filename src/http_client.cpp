#include "http_client.hpp"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <cstdlib>

using json = nlohmann::json;

static std::string env_or(const char* k, const char* d) {
    if (const char* v = std::getenv(k)) return std::string(v);
    return std::string(d);
}

LlmResult ollama_chat(const std::vector<Message>& history, int timeout_ms) {
    LlmResult out;

    const std::string host  = env_or("OLLAMA_HOST", "http://localhost:11434");
    const std::string model = env_or("MODEL", "llama3");
    const std::string url   = host + "/api/chat";

    // Build Ollama-compatible messages array
    json msgs = json::array();
    for (const auto& m : history) {
        msgs.push_back({{"role", m.role}, {"content", m.content}});
    }

    // Non-streaming request body
    json body = {
        {"model", model},
        {"stream", false},
        {"messages", msgs}
    };

    auto r = cpr::Post(
        cpr::Url{url},
        cpr::Header{{"Content-Type","application/json"}},
        cpr::Body{body.dump()},
        cpr::Timeout{timeout_ms}
    );

    if (r.error.code != cpr::ErrorCode::OK) {
        out.ok = false;
        out.error = "http error: " + r.error.message;
        return out;
    }
    if (r.status_code < 200 || r.status_code >= 300) {
        out.ok = false;
        out.error = "status " + std::to_string(r.status_code) + ": " + r.text;
        return out;
    }

    try {
        auto j = json::parse(r.text);
        // /api/chat (non-stream) returns: {"message":{"role":"assistant","content":"..."},"done":true,...}
        if (j.contains("message") && j["message"].contains("content")) {
            out.ok = true;
            out.text = j["message"]["content"].get<std::string>();
            return out;
        }
        out.ok = false;
        out.error = "unexpected json";
        return out;
    } catch (const std::exception& e) {
        out.ok = false;
        out.error = std::string("json parse: ") + e.what();
        return out;
    }
}