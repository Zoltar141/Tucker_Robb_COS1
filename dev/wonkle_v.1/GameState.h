#pragma once
#include <unordered_map>
#include <string>

class GameState
{
public:

    bool GetValue(const std::string& key, int& value) const;
    bool GetValue(const std::string& key, double& value) const;

    void SetValue(const std::string& key, int value);
    void SetValue(const std::string& key, double value);

    bool HasValue(const std::string& key) const;

private:
    std::unordered_map<std::string, double> values_;
};
