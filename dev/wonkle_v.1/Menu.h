#pragma once
class Menu
{
public:
    void Run();//main loop

private:
    void DisplayMainMenu();
    void HandleSelection(int choice);//I have a nested menu
};

