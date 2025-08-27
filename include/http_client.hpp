#pragma once
#include <string>
#include <vector>

struct Message {
    std::string role;     // "user" or "assistant"
    std::string content;
};

struct LlmResult {
    bool ok = false;
    std::string text;     // model reply
    std::string error;    // error if !ok
};

// Send chat history to Ollama and return the latest reply.
LlmResult ollama_chat(const std::vector<Message>& history, int timeout_ms = 15000);
