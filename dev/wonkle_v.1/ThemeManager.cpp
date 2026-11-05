#include "ThemeManager.h"

ThemeManager::ThemeManager()//constructor
{
    LoadThemes();
}

bool ThemeManager::RegisterTheme(const Theme& theme)//make a new one
{
    
    if (GetTheme(theme.GetName()) != nullptr)//check if theme already exists
    {
        return false; //prevents duplicate
    }
    themes.push_back(theme);//add the theme to vector

    if (activeThemeName.empty())//set first registered theme as active 
    {
        activeThemeName = theme.GetName();
    }
    return true; //confirms theme
}


Theme* ThemeManager::GetTheme(const std::string& name)//gets theme by name
{
    for (Theme& theme : themes)//auto loop through options
    {
        if (theme.GetName() == name)//checks that the current theme isn't the one it's looking for
        {
            return &theme; //return pointer
        }
    }
    return nullptr; //return nullptr if the theme doesn't exist
}

bool ThemeManager::SetActiveTheme(const std::string& name)//change themes
{
    if (GetTheme(name) == nullptr)//verify theme exists
    {
        return false;
    }
    activeThemeName = name;//update active theme 

    return true; //confirms theme 
}


Theme* ThemeManager::GetActiveTheme()//returns pointer to theme
{
    return GetTheme(activeThemeName);//retrieve theme using name
}

void ThemeManager::LoadThemes()
{
    Theme defaultTheme("Default");

    defaultTheme.SetColor(ThemeRoles::Background,
        Colors::Color::Black, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::Foreground,
        Colors::Color::White, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::Ball,
        Colors::Color::White, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::Border,
        Colors::Color::Gray, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::GameBoundary,
        Colors::Color::White, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::Paddle,
        Colors::Color::White, Colors::Color::Black);

    RegisterTheme(defaultTheme);


    Theme forestTheme("Forest");

    forestTheme.SetColor(ThemeRoles::Background,
        Colors::Color::DarkGreen, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::Foreground,
        Colors::Color::Tan, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::Ball,
        Colors::Color::Red, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::Border,
        Colors::Color::Brown, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::GameBoundary,
        Colors::Color::Green, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::Paddle,
        Colors::Color::Tan, Colors::Color::DarkGreen);

    RegisterTheme(forestTheme);
}