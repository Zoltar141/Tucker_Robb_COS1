#include "Colors.h"

namespace Colors
{
    void Print(const std::string& words, Color color)
    {
#ifdef _WIN32
        HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO original{};
        if (output != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(output, &original))
        {
            WORD attribute = FOREGROUND_RED |
                FOREGROUND_GREEN |
                FOREGROUND_BLUE |
                FOREGROUND_INTENSITY;

            if (color == Color::Black)
            {
                attribute = 0;
            }
            else if (color == Color::White)
            {
                attribute = FOREGROUND_RED | FOREGROUND_GREEN |
                    FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            }
            else if (color == Color::Blue)
            {
                attribute = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            }
            else if (color == Color::DarkBlue)
            {
                attribute = FOREGROUND_BLUE;
            }
            else if (color == Color::Green)
            {
                attribute = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            }
            else if (color == Color::DarkGreen)
            {
                attribute = FOREGROUND_GREEN;
            }
            else if (color == Color::Cyan)
            {
                attribute = FOREGROUND_GREEN | FOREGROUND_BLUE |
                    FOREGROUND_INTENSITY;
            }
            else if (color == Color::DarkCyan)
            {
                attribute = FOREGROUND_GREEN | FOREGROUND_BLUE;
            }
            else if (color == Color::Red)
            {
                attribute = FOREGROUND_RED | FOREGROUND_INTENSITY;
            }
            else if (color == Color::DarkRed)
            {
                attribute = FOREGROUND_RED;
            }
            else if (color == Color::Purple)
            {
                attribute = FOREGROUND_RED | FOREGROUND_BLUE |
                    FOREGROUND_INTENSITY;
            }
            else if (color == Color::DarkPurple)
            {
                attribute = FOREGROUND_RED | FOREGROUND_BLUE;
            }
            else if (color == Color::Yellow)
            {
                attribute = FOREGROUND_RED | FOREGROUND_GREEN |
                    FOREGROUND_INTENSITY;
            }
            else if (color == Color::DarkYellow)
            {
                attribute = FOREGROUND_RED | FOREGROUND_GREEN;
            }
            else if (color == Color::Gray)
            {
                attribute = FOREGROUND_RED | FOREGROUND_GREEN |
                    FOREGROUND_BLUE;
            }
            else if (color == Color::DarkGray)
            {
                attribute = FOREGROUND_INTENSITY;
            }
            else if (color == Color::Tan)
            {
                attribute = FOREGROUND_RED | FOREGROUND_GREEN |
                    FOREGROUND_INTENSITY;
            }
            else if (color == Color::Brown)
            {
                attribute = FOREGROUND_RED | FOREGROUND_GREEN;
            }

            std::cout.flush();
            SetConsoleTextAttribute(output, attribute);

            std::cout << words;
            std::cout.flush();

            SetConsoleTextAttribute(output, original.wAttributes);

            return;
        }
#endif 
        std::cout << words;
    }

    void Print(char character, Color color)
    {
        Print(std::string(1, character), color);
    }

    void SetBackground(Color color)
    {
#ifdef _WIN32
        HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO original{};

        if (output != INVALID_HANDLE_VALUE &&
            GetConsoleScreenBufferInfo(output, &original))
        {
            WORD attribute = 0;

            if (color == Color::Black)
            {
                attribute = 0;
            }
            else if (color == Color::White)
            {
                attribute = BACKGROUND_RED | BACKGROUND_GREEN |
                    BACKGROUND_BLUE | BACKGROUND_INTENSITY;
            }
            else if (color == Color::Blue)
            {
                attribute = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
            }
            else if (color == Color::DarkBlue)
            {
                attribute = BACKGROUND_BLUE;
            }
            else if (color == Color::Green)
            {
                attribute = BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            }
            else if (color == Color::DarkGreen)
            {
                attribute = BACKGROUND_GREEN;
            }
            else if (color == Color::Cyan)
            {
                attribute = BACKGROUND_GREEN | BACKGROUND_BLUE |
                    BACKGROUND_INTENSITY;
            }
            else if (color == Color::DarkCyan)
            {
                attribute = BACKGROUND_GREEN | BACKGROUND_BLUE;
            }
            else if (color == Color::Red)
            {
                attribute = BACKGROUND_RED | BACKGROUND_INTENSITY;
            }
            else if (color == Color::DarkRed)
            {
                attribute = BACKGROUND_RED;
            }
            else if (color == Color::Purple)
            {
                attribute = BACKGROUND_RED | BACKGROUND_BLUE |
                    BACKGROUND_INTENSITY;
            }
            else if (color == Color::DarkPurple)
            {
                attribute = BACKGROUND_RED | BACKGROUND_BLUE;
            }
            else if (color == Color::Yellow)
            {
                attribute = BACKGROUND_RED | BACKGROUND_GREEN |
                    BACKGROUND_INTENSITY;
            }
            else if (color == Color::DarkYellow)
            {
                attribute = BACKGROUND_RED | BACKGROUND_GREEN;
            }
            else if (color == Color::Gray)
            {
                attribute = BACKGROUND_RED | BACKGROUND_GREEN |
                    BACKGROUND_BLUE;
            }
            else if (color == Color::DarkGray)
            {
                attribute = BACKGROUND_INTENSITY;
            }
            else if (color == Color::Tan)
            {
                attribute = BACKGROUND_RED | BACKGROUND_GREEN |
                    BACKGROUND_INTENSITY;
            }
            else if (color == Color::Brown)
            {
                attribute = BACKGROUND_RED | BACKGROUND_GREEN;
            }

            WORD background = original.wAttributes & 0x000F;

            SetConsoleTextAttribute(output, background | attribute);
        }
#else
        (void)color;
#endif
    }

    void Reset()
    {
#ifdef _WIN32
        HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO original{};

        if (output != INVALID_HANDLE_VALUE &&
            GetConsoleScreenBufferInfo(output, &original))
        {
            SetConsoleTextAttribute(output, original.wAttributes);
        }
#endif
    }
}