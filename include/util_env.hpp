#pragma once
#include <string>

// Load KEY=VALUE pairs from a .env file (comments and blanks ignored).
// Does NOT overwrite variables that already exist in the process env.
bool load_env_file(const std::string& path);
