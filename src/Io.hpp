#pragma once

#include <cstdint>
#include <filesystem>
#include <utility>
#include <string>

auto ReadTextFromFile(const std::filesystem::path& filePath) -> std::string;
auto ReadBinaryFromFile(const std::filesystem::path& filePath) -> std::pair<std::unique_ptr<std::byte[]>, std::size_t>;