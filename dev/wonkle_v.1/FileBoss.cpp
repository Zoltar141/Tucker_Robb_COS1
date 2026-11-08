#include "FileBoss.h"
#include <filesystem>
#include <sstream>
#include <nlohmann/json.hpp>
#include <iostream>


// Checks whether a file exists.
bool FileBoss::FileExists(const std::string& filePath)
{
    if (filePath.empty())
    {
        return false;
    }

    std::error_code error;
    return std::filesystem::is_regular_file(filePath, error) && !error;
}

// Reads the contents of a file.
bool FileBoss::ReadFile(const std::string& filePath, std::string& content)
{
    if (filePath.empty())
    {
        return false;
    }

    std::ifstream file(filePath, std::ios::in | std::ios::binary);

    if (!file.is_open())
    {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    if (file.bad() || buffer.bad())
    {
        return false;
    }

    content = buffer.str();

    return true;
}

// Creates or overwrites a file.
bool FileBoss::WriteFile(const std::string& filePath, const std::string& content)
{
    if (filePath.empty())
    {
        return false;
    }

    std::ofstream file(filePath, std::ios::out | std::ios::binary | std::ios::trunc);

    if (!file.is_open())
    {
        return false;
    }

    file.write(content.data(), content.size());

    if (!file.good())
    {
        return false;
    }

    file.close();

    return !file.fail();
}

// Appends content to an existing file or creates it.
bool FileBoss::AppendFile(const std::string& filePath, const std::string& content)
{
    if (filePath.empty())
    {
        return false;
    }

    std::ofstream file(filePath, std::ios::out | std::ios::binary | std::ios::app);

    if (!file.is_open())
    {
        return false;
    }

    file.write(content.data(), content.size());

    if (!file.good())
    {
        return false;
    }

    file.close();

    return !file.fail();
}

//reads and parses JSON
bool FileBoss::ReadJSON(const std::string& filePath, nlohmann::json& data)
{
    if (filePath.empty())
    {
        return false;
    }

    std::ifstream file(filePath);

    if (!file.is_open())
    {
        return false;
    }

    try
    {
        file >> data;
    }
    catch (const nlohmann::json::parse_error&)
    {
        return false;
    }

    return true;
}