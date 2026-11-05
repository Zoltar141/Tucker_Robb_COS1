#include "Menu.h"
#include <iostream>
#include "PONG.h"
#include "InputHelper.h"

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
    std::cout << "\n========================\n";
    std::cout << "       WONKLE\n";
    std::cout << "========================\n";
    std::cout << "1. Choose Game\n";
    std::cout << "2. Settings\n";
    std::cout << "3. Exit\n";
    std::cout << "========================\n";
}

void Menu::HandleSelection(int choice)
{
    switch (choice)
    {
    case 1:
    {
        int gameChoice = 0;

        std::cout << "\n=== CHOOSE GAME ===\n";
        std::cout << "1. Pong\n";
        std::cout << "2. Back\n";
        std::cout << "Selection: ";

        std::cin >> gameChoice;

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
        std::cout << "\nExiting Wonkle.\n";
        break;

    default:
        std::cout << "\nInvalid selection.\n";
        break;
    }
}

void Menu::RunSettings()
{
    int choice = 0;

    const std::vector<Theme>& themes = themeManager.GetThemes();

    while (choice != static_cast<int>(themes.size()) + 1)
    {
        std::cout << "\n======= SETTINGS =======\n";

        std::cout << "Active Theme: "                               //display currently active theme
            << themeManager.GetActiveTheme()->GetName() << "\n\n";

        int index = 1;

        for (const Theme& theme : themes)                           //display available themes
        {
            std::cout << index << ". " << theme.GetName() << "\n";
            index++;
        }

        std::cout << index << ". Back\n";
        std::cout << "========================\n";

        choice = InputHelper::GetInt(
            "Select a theme: ", 1, index);

        if (choice < index)                                          //change active theme if theme selected
        {
            const std::string& selectedName =
                themes[choice - 1].GetName();

            themeManager.SetActiveTheme(selectedName);

            std::cout << "\nTheme changed to: "
                << selectedName << "\n";
            break;
        }
    }
}