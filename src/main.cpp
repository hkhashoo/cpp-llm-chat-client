#include "chat.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "LLM Chat (Ollama). Type 'exit' to quit.\n";
    Chat chat;

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line == "exit") break;

        const std::string resp = chat.reply(line);
        std::cout << resp << "\n";
    }
    return 0;
}