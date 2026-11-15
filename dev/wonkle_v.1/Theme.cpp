#include "Theme.h"

Theme::Theme(const std::string& name)//constructor
    : name(name)//initalize member
{
}

std::string Theme::GetName() const//returns name of theme
{
    return name;
}

void Theme::SetColor(const std::string& role, Colors::Color foreground, Colors::Color background)//assign color to role i.e. blue to player paddle
{
    colors[role] = { foreground, background };//updates role color
}

ThemeColor Theme::GetColor(const std::string& role) const//get color of role
{
    auto it = colors.find(role);//search the map

    if (it != colors.end())//checks if role exists
    {
        return it->second;//returns color
    }

    return { Colors::Color::White, Colors::Color::Black };//default if no color stored for that role
}