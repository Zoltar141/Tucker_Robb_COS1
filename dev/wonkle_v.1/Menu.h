#pragma once
#include "ThemeManager.h"

class Menu
{
public:
    void Run();//main loop

private:
    ThemeManager themeManager;

    void DisplayMainMenu();
    void HandleSelection(int choice);//I have a nested menu
    void RunSettings();
};

