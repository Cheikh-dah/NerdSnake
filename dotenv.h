#pragma once
#include <string>
#include <unordered_map>

std::unordered_map<std::string, std::string> LoadEnv(const std::string& path);
