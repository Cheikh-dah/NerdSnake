#include "dotenv.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>

// Trim whitespace from both ends
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::unordered_map<std::string, std::string> LoadEnv(const std::string& path)
{
    std::unordered_map<std::string, std::string> env;

    std::ifstream file(path);
    if (!file.is_open()) return env;

    std::string line;

    while (std::getline(file, line))
    {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = trim(line.substr(0, eq));
        std::string value = trim(line.substr(eq + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }

        if (!key.empty()) {
            env[key] = value;
        }
    }

    return env;
}
