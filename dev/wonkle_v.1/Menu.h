#pragma once
#include "ThemeManager.h"

#include <string>

class Menu
{
public:
    void Run();//main loop

private:
    ThemeManager themeManager;

    void DisplayMainMenu();
    void HandleSelection(int choice);//I have a nested menu
    void RunGameMenu(const std::string& fileName);//inbetween menu where save function will live 
    void RunSettings();
    void RunThemeMenu();
    void RunLoadMenu();
};

