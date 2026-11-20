#include "Menu.h"
#include <iostream>
#include "GameObjects.h"
#include "GameController.h"
#include "InputHelper.h"
#include "Theme.h"
#include "ThemeManager.h"
#include "FileBoss.h"
#include <algorithm>
#include <conio.h>
#include <stdexcept>

constexpr int MAIN_MENU_EXIT = 3;

void PrintTheme(ThemeManager& themeManager, const std::string& text, const std::string& role)
{
    Theme* activeTheme = themeManager.GetActiveTheme();
    ThemeColor color = activeTheme->GetColor(role);

    Colors::Print(text, color.foreground);
}

void Menu::Run()//main loop open ended right now while settings and game dont exist
{
    int choice = 0;

    while (choice != MAIN_MENU_EXIT)
    {
        DisplayMainMenu();//show menu

        choice = InputHelper::GetInt("Please choose a number: ", 1, MAIN_MENU_EXIT);//get choice

        HandleSelection(choice);//execute game or settings or exit 
    }
}

void Menu::DisplayMainMenu()
{
    PrintTheme(themeManager, "\n============================================================\n", ThemeRoles::Border);
    PrintTheme(themeManager, 
        " _         _____         _     __        _         ______\n"
        "| |  _    |     |    ___| |   |  |__    | |       |   ___|\n"
        "| |_| |   |  |  |   |   | |   |  |  |   | |       |  |__\n"
        "| | | |   |  |  |   |   | |   |    <    | |___    |   __|\n"
        "| | | | _ |  |  | _ | |   | _ |  |  | _ |     | _ |  |____  _\n"
        "|_____||_||_____||_|| |___||_||__^__||_||_____||_||_______||_|\n"
        "                    |_|\n",
        ThemeRoles::MenuHighlight);
    PrintTheme(themeManager, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", ThemeRoles::Border);

    PrintTheme(themeManager, "\t1. Choose Program\t", ThemeRoles::MenuText);
    PrintTheme(themeManager, "2. Settings\t", ThemeRoles::MenuText);
    PrintTheme(themeManager, "3. Exit\n", ThemeRoles::MenuText);

    PrintTheme(themeManager, "============================================================\n", ThemeRoles::Border);
}

void Menu::HandleSelection(int choice)
{
    std::string fileName;
    switch (choice)
    {
    case 1:
    {
        system("cls");

        int gameChoice = 0;

        PrintTheme(themeManager, "\n.'.'. CHOOSE PROGRAM .'.'.\n", ThemeRoles::Border);
        PrintTheme(themeManager, "1. Pong\n", ThemeRoles::MenuText);
        PrintTheme(themeManager, "2. Brick Breaker\n", ThemeRoles::MenuText);
        PrintTheme(themeManager, "3. Snake\n", ThemeRoles::MenuText);
        PrintTheme(themeManager, "4. Maze\n", ThemeRoles::MenuText);
        PrintTheme(themeManager, "5. Wonk-ulator\n", ThemeRoles::MenuText);
        PrintTheme(themeManager, "6. Back\n", ThemeRoles::MenuText);

        gameChoice = InputHelper::GetInt("Selection: ", 1, 6);

        if (gameChoice == 1)
        {
            fileName = "pong.json";
        }
        else if (gameChoice == 2)
        {
            fileName = "brickbreaker.json";
        }
        else if (gameChoice == 3)
        {
            fileName = "snake.json";
        }
        else if (gameChoice == 4)
        {
            fileName = "maze.json";
        }
        else if (gameChoice == 5)
        {
            fileName = "calculator.json";
        }
        else if (gameChoice == 6)
        {
            system("cls");
            break;
        }

        if (!fileName.empty())
        {
            RunGameMenu(fileName);
        }

        break;
    }

    case 2:
        RunSettings();
        break;

    case MAIN_MENU_EXIT:
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

    while (choice != 3)
    {
        PrintTheme(themeManager, "\n======= SETTINGS =======\n", ThemeRoles::Border);

        PrintTheme(themeManager, "Active Theme: ", ThemeRoles::MenuHighlight);
        PrintTheme(themeManager, themeManager.GetActiveTheme()->GetName() + "\n\n", ThemeRoles::MenuText);

        PrintTheme(themeManager, "1. Change Theme\n", ThemeRoles::MenuText);
        PrintTheme(themeManager, "2. Reset All Saves\n", ThemeRoles::MenuText);
        PrintTheme(themeManager, "3. Back\n", ThemeRoles::MenuText);
        PrintTheme(themeManager, "========================\n", ThemeRoles::Border);

        choice = InputHelper::GetInt("Select a theme: ", 1, 3);
        if (choice == 1)
        {
            RunThemeMenu();
        }
        else if(choice == 2)
        {
            system("cls");

            PrintTheme(themeManager,
                "\nWARNING: This will delete ALL game saves and high scores.\n",
                ThemeRoles::MenuHighlight);

            PrintTheme(themeManager,
                "1. Confirm Reset\n2. Cancel\n",
                ThemeRoles::MenuText);

            int confirm = InputHelper::GetInt("Selection: ", 1, 2);

            if (confirm == 1)
            {
                if (GameController::ResetAllSaveData())
                {
                    PrintTheme(themeManager,
                        "\nAll saves and high scores have been reset.\n",
                        ThemeRoles::MenuHighlight);
                }
                else
                {
                    PrintTheme(themeManager,
                        "\nFailed to reset save data.\n",
                        ThemeRoles::MenuHighlight);
                }
            }
            else
            {
                PrintTheme(themeManager, "\nReset cancelled.\n",
                    ThemeRoles::MenuText);
            }
        }
    }
}

void Menu::RunGameMenu(const std::string& fileName)
{
    system("cls");

    FileBoss fileBoss;
    nlohmann::json config;

    if (!fileBoss.ReadJSON(fileName, config))
    {
        PrintTheme(themeManager, "\nCould not load " + fileName + "\n", ThemeRoles::MenuHighlight);
        return;
    }

    const auto& game = config["game"];
    int choice = 0;

    while (choice != 3)
    {
        for (const auto& line : game["menu"]["art"])
        {
            PrintTheme(themeManager, line.get<std::string>() + "\n", ThemeRoles::MenuHighlight);
        }
        PrintTheme(themeManager, game["menu"]["description"].get<std::string>() + "\n\n", ThemeRoles::MenuHighlight);
        PrintTheme(themeManager, "1. Play\n", ThemeRoles::MenuText);
        PrintTheme(themeManager, "2. Controls\n", ThemeRoles::MenuText);
        PrintTheme(themeManager, "3. Back\n", ThemeRoles::MenuText);

        choice = InputHelper::GetInt("Selection:", 1, 3);

        if (choice == 1)
        {
            try
            {
                GameObjects play(themeManager, fileName);
                play.Run();
            }
            catch (const std::exception& error)
            {
                PrintTheme(themeManager, "W.O.N.K.L.E. Cannot load this game: " +
                    std::string(error.what()) + "\n", ThemeRoles::MenuHighlight);
            }
           
        }
        else if (choice == 2)
        {
            system("cls");

            PrintTheme(themeManager, "\n" + 
                game["menu"]["controls"].get<std::string>() + 
                "\n", ThemeRoles::MenuText);

            PrintTheme(themeManager, "Press any key to go Back\n", ThemeRoles::MenuText);
            
            _getch();

            system("cls");
        }
        else if (choice == 3)
        {
            system("cls");
            Menu::HandleSelection(1);
        }
    }
}

void Menu::RunThemeMenu()
{
    const std::vector<Theme>& themes = themeManager.GetThemes();
    const int backChoice = static_cast<int>(themes.size()) + 1;

    PrintTheme(themeManager, "\n===== CHANGE THEME =====\n", ThemeRoles::Border);

    int index = 1;

    for (const Theme& theme : themes)
    {
        PrintTheme(themeManager, std::to_string(index) + ". " + theme.GetName() + "\n", ThemeRoles::MenuText);
        index++;
    }

    PrintTheme(themeManager, std::to_string(backChoice) + ". Back\n", ThemeRoles::MenuText);
    PrintTheme(themeManager, "===========================\n", ThemeRoles::Border);

    int choice = InputHelper::GetInt("Select a theme: ", 1, backChoice);

    if (choice == backChoice)
    {
        return;
    }

    const std::string& selectedName = themes[choice - 1].GetName();

    themeManager.SetActiveTheme(selectedName);

    PrintTheme(themeManager, "Theme changed to: \n", ThemeRoles::MenuHighlight);
    PrintTheme(themeManager, selectedName + "\n", ThemeRoles::MenuText);

}