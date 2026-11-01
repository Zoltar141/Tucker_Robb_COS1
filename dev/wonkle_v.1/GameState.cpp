#include "GameState.h"


bool GameState::GetValue(const std::string& key, int& value) const//true means succesful retrival of value
{
    auto it = values_.find(key);//looks for a key

    if (it == values_.end())//if no key
    {
        return false;//no value retrieved
    }

    value = it->second;//get value from key 
    return true;//retrieved value
}

void GameState::SetValue(const std::string& key, int value)
{
    values_[key] = value;//if no key it creates one otherwise overwrites
}

bool GameState::HasValue(const std::string& key) const
{
    return values_.find(key) != values_.end();//if key is found return true if not found return false
}