#pragma once

#include <string>
#include <unordered_map>

#include "Colors.h"

//semantic visual roles
namespace ThemeRoles
{
    //general application colors
    inline constexpr char Background[] = "Background";
    inline constexpr char Foreground[] = "Foreground";

    //menu colors
    inline constexpr char MenuText[] = "MenuText";
    inline constexpr char MenuHighlight[] = "MenuHighlight";

    //game environment 
    inline constexpr char Border[] = "Border";
    inline constexpr char GameBoundary[] = "GameBoundary";

    //game objects
    inline constexpr char Paddle[] = "Paddle";
    inline constexpr char Ball[] = "Ball";
    inline constexpr char Brick[] = "Brick";
    inline constexpr char SnakeBody[] = "SnakeBody";
    inline constexpr char SnakeFood[] = "SnakeFood";

    //interface colors
    inline constexpr char UI[] = "UI";
}

struct ThemeColor//seperate foreground and background
{
    Colors::Color foreground;
    Colors::Color background;
};

class Theme
{
public:
    Theme(const std::string& name);//theme name i.e. forest

    std::string GetName() const;

    void SetColor(const std::string& role, Colors::Color foreground, Colors::Color background);//sets the colors to each piece paddle, ball, etc.
    ThemeColor GetColor(const std::string& role) const;

private:
    std::string name;
    std::unordered_map<std::string, ThemeColor> colors;//using a map for flexibility
};