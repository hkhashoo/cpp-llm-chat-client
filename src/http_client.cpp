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

// --- Streaming version of Ollama chat ---

static void feed_lines(std::string& carry, std::string_view incoming,
                       const std::function<void(std::string_view)>& on_line) {
    carry.append(incoming.data(), incoming.size());
    size_t start = 0;
    while (true) {
        size_t pos = carry.find('\n', start);
        if (pos == std::string::npos) break;
        std::string_view line{carry.data() + start, pos - start};
        if (!line.empty()) on_line(line);
        start = pos + 1;
    }
    if (start > 0) {
        carry.erase(0, start);
    }
}

LlmResult ollama_chat_stream(
    const std::vector<Message>& history,
    const std::function<void(const std::string&)>& on_chunk,
    int timeout_ms) {

    LlmResult out;

    const std::string host  = env_or("OLLAMA_HOST", "http://localhost:11434");
    const std::string model = env_or("MODEL", "llama3");
    const std::string url   = host + "/api/chat";

    json msgs = json::array();
    for (const auto& m : history) {
        msgs.push_back({{"role", m.role}, {"content", m.content}});
    }
    json body = {
        {"model", model},
        {"stream", true},
        {"messages", msgs}
    };

    std::string full;
    std::string carry;
    bool had_error = false;
    std::string err_msg;

    cpr::Session session;
    session.SetUrl(cpr::Url{url});
    session.SetHeader({{"Content-Type","application/json"}});
    session.SetBody(cpr::Body{body.dump()});
    session.SetTimeout(cpr::Timeout{timeout_ms});
    session.SetWriteCallback(cpr::WriteCallback(
        [&](std::string data, intptr_t) {
            feed_lines(carry, std::string_view{data}, [&](std::string_view line_sv){
                try {
                    auto j = json::parse(line_sv);
                    if (j.contains("message") && j["message"].contains("content")) {
                        const std::string chunk = j["message"]["content"].get<std::string>();
                        if (!chunk.empty()) {
                            full += chunk;
                            if (on_chunk) on_chunk(chunk);
                        }
                    } else if (j.contains("response")) {
                        const std::string chunk = j["response"].get<std::string>();
                        if (!chunk.empty()) {
                            full += chunk;
                            if (on_chunk) on_chunk(chunk);
                        }
                    }
                } catch (const std::exception& e) {
                    had_error = true;
                    err_msg = std::string("json stream parse: ") + e.what();
                }
            });
            return !had_error;
        }
    ));

    auto r = session.Post();
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
    if (had_error) {
        out.ok = false;
        out.error = err_msg.empty() ? "stream error" : err_msg;
        return out;
    }
    out.ok = true;
    out.text = std::move(full);
    return out;
}