#pragma once
#include <iostream>
#include <string>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif 

namespace Colors
{
    enum class Color
    {
        Black, White, Blue, DarkBlue, Green, DarkGreen, Cyan, DarkCyan,
        Red, DarkRed, Purple, DarkPurple, Yellow, DarkYellow, Gray, DarkGray, Tan, Brown
    };

    void Print(const std::string& words, Color color);
    void Print(char character, Color color);
    void SetBackground(Color color);
    void Reset();
}
