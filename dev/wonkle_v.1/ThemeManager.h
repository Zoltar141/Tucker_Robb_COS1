#pragma once

#include <string>
#include <vector>

#include "Theme.h"

class ThemeManager
{
public:
    ThemeManager();

    bool RegisterTheme(const Theme& theme);//makes a new theme
    
    Theme* GetTheme(const std::string& name);//gets exsisting theme

    bool SetActiveTheme(const std::string& name);//changes active
    Theme* GetActiveTheme();//gets current

    void LoadThemes();
    const std::vector<Theme>& GetThemes() const;
private:
    std::vector<Theme> themes;
    std::string activeThemeName;
};
