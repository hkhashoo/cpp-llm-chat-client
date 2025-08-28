#include "chat.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
#include "util_env.hpp"
#include <cstdlib>
#include "util_log.hpp"

static void usage(const char* prog) {
    std::cout << "Usage: " << prog << " [--stream|--no-stream] [--model NAME] [--history N] [--timeout MS]\n";
}

static void setenv_if_empty(const char* k, const std::string& v) {
    if (!std::getenv(k)) setenv(k, v.c_str(), 0);
}

static bool parse_bool(const std::string& s) {
    std::string t;
    t.reserve(s.size());
    for (char c: s) t.push_back(std::tolower(c));
    return (t=="1" || t=="true" || t=="yes" || t=="on");
}

static void parse_args(int argc, char** argv) {
    std::vector<std::string> args(argv+1, argv+argc);
    for (size_t i=0; i<args.size(); ++i) {
        const std::string& a = args[i];
        if (a=="--help" || a=="-h") { usage(argv[0]); std::exit(0); }
        else if (a=="--stream") { setenv("STREAM","true",1); }
        else if (a=="--no-stream") { setenv("STREAM","false",1); }
        else if (a=="--model") {
            if (i+1>=args.size()) { std::cerr<<"--model requires a value\n"; std::exit(2); }
            setenv("MODEL", args[++i].c_str(), 1);
        }
        else if (a=="--history") {
            if (i+1>=args.size()) { std::cerr<<"--history requires a number\n"; std::exit(2); }
            setenv("HISTORY_LIMIT", args[++i].c_str(), 1);
        }
        else if (a=="--timeout") {
            if (i+1>=args.size()) { std::cerr<<"--timeout requires ms\n"; std::exit(2); }
            setenv("TIMEOUT_MS", args[++i].c_str(), 1);
        }
        else {
            std::cerr << "Unknown option: " << a << "\n";
            usage(argv[0]); std::exit(2);
        }
    }
}

static bool env_true(const char* k) {
    if (const char* v = std::getenv(k)) {
        std::string s(v);
        for (auto& c : s) c = std::tolower(c);
        return (s == "1" || s == "true" || s == "yes");
    }
    return false;
}

int main(int argc, char** argv) {
    log::init();
    load_env_file("../.env");
    parse_args(argc, argv);

    setenv_if_empty("PROVIDER", "ollama");
    setenv_if_empty("MODEL", "llama3");

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