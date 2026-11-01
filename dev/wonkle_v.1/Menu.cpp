#include "Menu.h"
#include <iostream>

void Menu::Run()//main loop open ended right now while settings and game dont exist
{
    int choice = 0;

    while (choice != 3)
    {
        DisplayMainMenu();//show menu

        std::cin >> choice;//get choice

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
    std::cout << "Selection: ";
}

void Menu::HandleSelection(int choice)
{
    switch (choice)
    {
    case 1:
        std::cout << "\nChoose Game selected.\n";//place holders for now
        break;

    case 2:
        std::cout << "\nSettings selected.\n";
        break;

    case 3:
        std::cout << "\nExiting Wonkle.\n";
        break;

    default:
        std::cout << "\nInvalid selection.\n";
        break;
    }
}