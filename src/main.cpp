#include "chat.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
#include "util_env.hpp"
#include <cstdlib>

static bool env_true(const char* k) {
    if (const char* v = std::getenv(k)) {
        std::string s(v);
        for (auto& c : s) c = std::tolower(c);
        return (s == "1" || s == "true" || s == "yes");
    }
    return false;
}

int main() {
    load_env_file("../.env");
    std::cout << "LLM Chat (Ollama). Type 'exit' to quit.\n";
    const bool stream = env_true("STREAM");

    Chat chat;
    std::string line;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line == "exit") break;

        const std::string resp = chat.reply(line);

        // In streaming mode, Chat::reply already printed chunks.
        if (!stream) {
            std::cout << resp << "\n";
        }
    }
    return 0;
}