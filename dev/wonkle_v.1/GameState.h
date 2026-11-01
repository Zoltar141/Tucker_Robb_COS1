#pragma once
#include <unordered_map>
#include <string>

class GameState
{
public:

	bool GetValue(const std::string& key, int& value) const;//gets state, returns false if no key

	void SetValue(const std::string& key, int value);//sets or updates state

	bool HasValue(const std::string& key) const;//checks for key
private:
	std::unordered_map<std::string, int> values_;
};

