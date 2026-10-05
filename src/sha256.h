#pragma once

#include <cstdint>
#include <string>

std::string sha256(const std::string& data);
std::string sha256_file(const std::string& path, std::string& error);
