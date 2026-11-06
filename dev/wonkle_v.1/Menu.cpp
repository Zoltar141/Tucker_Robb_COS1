#include "Menu.h"
#include <iostream>
#include "PONG.h"
#include "InputHelper.h"
#include "Theme.h"
#include "ThemeManager.h"


void PrintTheme(ThemeManager& themeManager, const std::string& text, const std::string& role)
{
    Theme* activeTheme = themeManager.GetActiveTheme();
    ThemeColor color = activeTheme->GetColor(role);

    Colors::Print(text, color.foreground);
}

void Menu::Run()//main loop open ended right now while settings and game dont exist
{
    int choice = 0;

    while (choice != 3)
    {
        DisplayMainMenu();//show menu

        choice = InputHelper::GetInt("Please choose a number: ", 1, 3);//get choice

        HandleSelection(choice);//execute game or settings or exit 
    }
}

void Menu::DisplayMainMenu()
{
    PrintTheme(themeManager, "\n========================\n", ThemeRoles::Border);
    PrintTheme(themeManager, "       WONKLE\n", ThemeRoles::MenuHighlight);
    PrintTheme(themeManager, "========================\n", ThemeRoles::Border);

    PrintTheme(themeManager, "1. Choose Game\n", ThemeRoles::MenuText);
    PrintTheme(themeManager, "2. Settings\n", ThemeRoles::MenuText);
    PrintTheme(themeManager, "3. Exit\n", ThemeRoles::MenuText);

    PrintTheme(themeManager, "========================\n", ThemeRoles::Border);
}

void Menu::HandleSelection(int choice)
{
    switch (choice)
    {
    case 1:
    {
        int gameChoice = 0;

        PrintTheme(themeManager, "\n=== CHOOSE GAME ===\n", ThemeRoles::Border);
        PrintTheme(themeManager, "1. Pong\n", ThemeRoles::MenuText);
        PrintTheme(themeManager, "2. Back\n", ThemeRoles::MenuText);

        gameChoice = InputHelper::GetInt("Selection: ", 1, 2);

        if (gameChoice == 1)
        {
            PONG game(themeManager);
            game.Run();
        }

        break;
    }
    case 2:
        RunSettings();
        break;

    case 3:
        PrintTheme(themeManager, "\nExiting Wonkle.\n", ThemeRoles::MenuHighlight);
        break;

    default:
        PrintTheme(themeManager, "\nInvalid selection.\n", ThemeRoles::MenuHighlight);
        break;
    }
}

void Menu::RunSettings()
{
    int choice = 0;

    const std::vector<Theme>& themes = themeManager.GetThemes();

    while (choice != static_cast<int>(themes.size()) + 1)
    {
        PrintTheme(themeManager, "\n======= SETTINGS =======\n", ThemeRoles::Border);

        PrintTheme(themeManager, "Active Theme: ", ThemeRoles::MenuHighlight);
        PrintTheme(themeManager, themeManager.GetActiveTheme()->GetName() + "\n\n", ThemeRoles::MenuText);

        int index = 1;

        for (const Theme& theme : themes)                           //display available themes
        {
            PrintTheme(themeManager, std::to_string(index) + ". " + theme.GetName() + "\n", ThemeRoles::MenuText);
            index++;
        }

        PrintTheme(themeManager, std::to_string(index) + ". Back\n", ThemeRoles::MenuText);
        PrintTheme(themeManager, "========================\n", ThemeRoles::Border);

        choice = InputHelper::GetInt("Select a theme: ", 1, index);

        if (choice < index)                                          //change active theme if theme selected
        {
            const std::string& selectedName =
                themes[choice - 1].GetName();

            themeManager.SetActiveTheme(selectedName);

            PrintTheme(themeManager, "\nTheme changed to: ", ThemeRoles::MenuHighlight);
            PrintTheme(themeManager, selectedName + "\n", ThemeRoles::MenuText);
            break;
        }
    }
}