#include "InputHelper.h"
#include <iostream>
#include <conio.h>
#include <limits>

int InputHelper::GetInt(const std::string& prompt, int min, int max)
{
    while (true)
    {
        std::string input;

        std::cout << prompt;
        std::getline(std::cin, input);

        try
        {
            int value = std::stoi(input);

            if (value >= min && value <= max)
            {
                return value;
            }

            std::cout << "That is not one of the available choices.\n";
        }
        catch (const std::invalid_argument&)
        {
            std::cout << "That is not a number.\n";
        }
        catch (const std::out_of_range&)
        {
            std::cout << "That number is beyond W.O.N.K.L.E.'s capabilities.\n";
        }
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