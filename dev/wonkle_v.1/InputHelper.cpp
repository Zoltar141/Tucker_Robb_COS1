#include "InputHelper.h"
#include <iostream>
#include <conio.h>
#include <limits>

int InputHelper::GetInt(const std::string& prompt, int min, int max)//gets a valid int in a specific range
{
    int value;

    while (true)
    {
        std::cout << prompt;

        if (std::cin >> value && value >= min && value <= max)//check if input is valid
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');//clear any remaining input
            return value;
        }

        std::cout << "Invalid input. Try again.\n";//you did it wrong

        std::cin.clear();//reset the stream
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');//discard invalid input
    }
}

bool InputHelper::IsKeyPressed()//checks for key pressed
{
    return _kbhit();
}

char InputHelper::GetKey()//returns key pressed without enter
{
    return _getch();
}