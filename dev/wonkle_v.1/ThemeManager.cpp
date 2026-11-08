#include "ThemeManager.h"
#include "FileBoss.h"
#include <unordered_map>

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
    FileBoss fileBoss;
    nlohmann::json data;

    // Read the themes JSON file.
    if (!fileBoss.ReadJSON("themes.json", data))
    {
        return;
    }

    // Verify the themes array exists.
    if (!data.contains("themes") || !data["themes"].is_array())
    {
        return;
    }

    // Load each theme.
    for (const auto& themeData : data["themes"])
    {
        if (!themeData.is_object() ||
            !themeData.contains("name") ||
            !themeData["name"].is_string() ||
            !themeData.contains("colors") ||
            !themeData["colors"].is_object())
        {
            continue;
        }

        // Create theme using its JSON name.
        Theme theme(themeData["name"].get<std::string>());

        // Load its semantic color roles.
        for (auto it = themeData["colors"].begin();
            it != themeData["colors"].end(); ++it)
        {
            const auto& colorData = it.value();

            if (!colorData.is_object() ||
                !colorData.contains("foreground") ||
                !colorData.contains("background") ||
                !colorData["foreground"].is_string() ||
                !colorData["background"].is_string())
            {
                continue;
            }

            // Convert JSON strings using the existing Colors system.
            Colors::Color foreground =
                Colors::FromString(colorData["foreground"].get<std::string>());

            Colors::Color background =
                Colors::FromString(colorData["background"].get<std::string>());

            // Assign colors to their semantic roles.
            theme.SetColor(
                it.key(),
                foreground,
                background
            );
        }

        // Register the completed theme.
        RegisterTheme(theme);
    }
}