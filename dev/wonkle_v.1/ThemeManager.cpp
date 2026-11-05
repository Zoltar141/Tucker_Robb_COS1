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

const std::vector<Theme>& ThemeManager::GetThemes() const//returns collection registered themes

{
    return themes;
}

void ThemeManager::LoadThemes()
{
    Theme defaultTheme("Default");

    defaultTheme.SetColor(ThemeRoles::Background,
        Colors::Color::Black, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::Foreground,
        Colors::Color::White, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::MenuText,
        Colors::Color::White, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::MenuHighlight,
        Colors::Color::Yellow, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::Ball,
        Colors::Color::White, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::Border,
        Colors::Color::Gray, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::GameBoundary,
        Colors::Color::White, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::Paddle,
        Colors::Color::White, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::SnakeBody,
        Colors::Color::Green, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::SnakeFood,
        Colors::Color::Red, Colors::Color::Black);

    defaultTheme.SetColor(ThemeRoles::UI,
        Colors::Color::White, Colors::Color::Black);

    RegisterTheme(defaultTheme);


    Theme forestTheme("Forest");

    forestTheme.SetColor(ThemeRoles::Background,
        Colors::Color::DarkGreen, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::Foreground,
        Colors::Color::Tan, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::MenuText,
        Colors::Color::Tan, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::MenuHighlight,
        Colors::Color::Yellow, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::Ball,
        Colors::Color::Red, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::Border,
        Colors::Color::Brown, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::GameBoundary,
        Colors::Color::Green, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::Paddle,
        Colors::Color::Tan, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::SnakeBody,
        Colors::Color::Tan, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::SnakeFood,
        Colors::Color::Red, Colors::Color::DarkGreen);

    forestTheme.SetColor(ThemeRoles::UI,
        Colors::Color::Tan, Colors::Color::DarkGreen);

    RegisterTheme(forestTheme);


    Theme iceTheme("Ice");

    iceTheme.SetColor(ThemeRoles::Background,
        Colors::Color::DarkBlue, Colors::Color::DarkBlue);

    iceTheme.SetColor(ThemeRoles::Foreground,
        Colors::Color::White, Colors::Color::DarkBlue);

    iceTheme.SetColor(ThemeRoles::MenuText,
        Colors::Color::Cyan, Colors::Color::DarkBlue);

    iceTheme.SetColor(ThemeRoles::MenuHighlight,
        Colors::Color::White, Colors::Color::Blue);

    iceTheme.SetColor(ThemeRoles::Border,
        Colors::Color::Cyan, Colors::Color::DarkBlue);

    iceTheme.SetColor(ThemeRoles::GameBoundary,
        Colors::Color::White, Colors::Color::DarkBlue);

    iceTheme.SetColor(ThemeRoles::Paddle,
        Colors::Color::Cyan, Colors::Color::DarkBlue);

    iceTheme.SetColor(ThemeRoles::Ball,
        Colors::Color::White, Colors::Color::DarkBlue);

    iceTheme.SetColor(ThemeRoles::Brick,
        Colors::Color::Cyan, Colors::Color::DarkBlue);

    iceTheme.SetColor(ThemeRoles::SnakeBody,
        Colors::Color::White, Colors::Color::DarkBlue);

    iceTheme.SetColor(ThemeRoles::SnakeFood,
        Colors::Color::Red, Colors::Color::DarkBlue);

    iceTheme.SetColor(ThemeRoles::UI,
        Colors::Color::White, Colors::Color::DarkBlue);

    RegisterTheme(iceTheme);


    Theme darkTheme("Dark");

    darkTheme.SetColor(ThemeRoles::Background,
        Colors::Color::Black, Colors::Color::Black);

    darkTheme.SetColor(ThemeRoles::Foreground,
        Colors::Color::Gray, Colors::Color::Black);

    darkTheme.SetColor(ThemeRoles::MenuText,
        Colors::Color::Gray, Colors::Color::Black);

    darkTheme.SetColor(ThemeRoles::MenuHighlight,
        Colors::Color::Purple, Colors::Color::Black);

    darkTheme.SetColor(ThemeRoles::Border,
        Colors::Color::DarkGray, Colors::Color::Black);

    darkTheme.SetColor(ThemeRoles::GameBoundary,
        Colors::Color::DarkGray, Colors::Color::Black);

    darkTheme.SetColor(ThemeRoles::Paddle,
        Colors::Color::White, Colors::Color::Black);

    darkTheme.SetColor(ThemeRoles::Ball,
        Colors::Color::DarkRed, Colors::Color::Black);

    darkTheme.SetColor(ThemeRoles::Brick,
        Colors::Color::DarkPurple, Colors::Color::Black);

    darkTheme.SetColor(ThemeRoles::SnakeBody,
        Colors::Color::Gray, Colors::Color::Black);

    darkTheme.SetColor(ThemeRoles::SnakeFood,
        Colors::Color::Red, Colors::Color::Black);

    darkTheme.SetColor(ThemeRoles::UI,
        Colors::Color::White, Colors::Color::Black);

    RegisterTheme(darkTheme);


    Theme lightTheme("Light");

    lightTheme.SetColor(ThemeRoles::Background,
        Colors::Color::White, Colors::Color::White);

    lightTheme.SetColor(ThemeRoles::Foreground,
        Colors::Color::Black, Colors::Color::White);

    lightTheme.SetColor(ThemeRoles::MenuText,
        Colors::Color::Black, Colors::Color::White);

    lightTheme.SetColor(ThemeRoles::MenuHighlight,
        Colors::Color::White, Colors::Color::Blue);

    lightTheme.SetColor(ThemeRoles::Border,
        Colors::Color::DarkGray, Colors::Color::White);

    lightTheme.SetColor(ThemeRoles::GameBoundary,
        Colors::Color::Black, Colors::Color::White);

    lightTheme.SetColor(ThemeRoles::Paddle,
        Colors::Color::Blue, Colors::Color::White);

    lightTheme.SetColor(ThemeRoles::Ball,
        Colors::Color::Red, Colors::Color::White);

    lightTheme.SetColor(ThemeRoles::Brick,
        Colors::Color::DarkRed, Colors::Color::White);

    lightTheme.SetColor(ThemeRoles::SnakeBody,
        Colors::Color::Green, Colors::Color::White);

    lightTheme.SetColor(ThemeRoles::SnakeFood,
        Colors::Color::Red, Colors::Color::White);

    lightTheme.SetColor(ThemeRoles::UI,
        Colors::Color::Black, Colors::Color::White);

    RegisterTheme(lightTheme);


    Theme classicTheme("Classic");

    classicTheme.SetColor(ThemeRoles::Background,
        Colors::Color::Black, Colors::Color::Black);

    classicTheme.SetColor(ThemeRoles::Foreground,
        Colors::Color::Green, Colors::Color::Black);

    classicTheme.SetColor(ThemeRoles::MenuText,
        Colors::Color::Green, Colors::Color::Black);

    classicTheme.SetColor(ThemeRoles::MenuHighlight,
        Colors::Color::Black, Colors::Color::Green);

    classicTheme.SetColor(ThemeRoles::Border,
        Colors::Color::Green, Colors::Color::Black);

    classicTheme.SetColor(ThemeRoles::GameBoundary,
        Colors::Color::DarkGreen, Colors::Color::Black);

    classicTheme.SetColor(ThemeRoles::Paddle,
        Colors::Color::Green, Colors::Color::Black);

    classicTheme.SetColor(ThemeRoles::Ball,
        Colors::Color::Green, Colors::Color::Black);

    classicTheme.SetColor(ThemeRoles::Brick,
        Colors::Color::Cyan, Colors::Color::Black);

    classicTheme.SetColor(ThemeRoles::SnakeBody,
        Colors::Color::Green, Colors::Color::Black);

    classicTheme.SetColor(ThemeRoles::SnakeFood,
        Colors::Color::Green, Colors::Color::Black);

    classicTheme.SetColor(ThemeRoles::UI,
        Colors::Color::Green, Colors::Color::Black);

    RegisterTheme(classicTheme);
}