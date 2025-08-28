#include "util_env.hpp"
#include <fstream>
#include <string>
#include <cctype>
#include <cstdlib>

static inline void rtrim(std::string& s) {
    while (!s.empty() && (s.back()==' ' || s.back()=='\t' || s.back()=='\r')) s.pop_back();
}
static inline void ltrim(std::string& s) {
    size_t i = 0;
    while (i < s.size() && (s[i]==' ' || s[i]=='\t')) ++i;
    if (i) s.erase(0, i);
}
static inline void trim(std::string& s) { rtrim(s); ltrim(s); }

static inline void strip_quotes(std::string& s) {
    if (s.size() >= 2) {
        if ((s.front()=='"' && s.back()=='"') || (s.front()=='\'' && s.back()=='\'')) {
            s = s.substr(1, s.size()-2);
        }
    }
}

bool load_env_file(const std::string& path) {
    std::ifstream in(path);
    if (!in) return false;

    std::string line;
    while (std::getline(in, line)) {
        // remove comments
        auto hash = line.find('#');
        if (hash != std::string::npos) line.erase(hash);
        trim(line);
        if (line.empty()) continue;

        // split KEY=VALUE
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        trim(key); trim(val);
        strip_quotes(val);
        if (key.empty()) continue;

#if defined(_WIN32)
        // On Windows you'd use _putenv_s; skipping for now (project targets mac)
        _putenv_s(key.c_str(), val.c_str());
#else
        // Only set if not already set in the environment
        if (!std::getenv(key.c_str())) {
            setenv(key.c_str(), val.c_str(), 0); // overwrite = 0
        }
#endif
    }
    return true;
}
