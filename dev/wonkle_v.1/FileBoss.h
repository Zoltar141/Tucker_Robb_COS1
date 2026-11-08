#pragma once
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

class FileBoss
{
public:
    bool FileExists(const std::string& filePath);

    bool ReadFile(const std::string& filePath, std::string& content);

    bool WriteFile(const std::string& filePath, const std::string& content);

    bool AppendFile(const std::string& filePath, const std::string& content);

    bool ReadJSON(const std::string& filePath, nlohmann::json& data);
};
