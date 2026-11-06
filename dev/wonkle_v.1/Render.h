#pragma once
#include "ThemeManager.h"
#include "Colors.h"
#include <string>
#include <vector>

struct RenderObject
{
    int x;
    int y;
    char character;
    std::string role;
};

class Render
{
public:
    Render(ThemeManager& themeManager);

    void SetCursorPosition(int x, int y);
    void SetBackground();

    // Frame management.
    void BeginFrame(int width, int height, const std::string& backgroundRole);
    void PresentFrame();

    void DrawCharacter(int x, int y, char character, const std::string& role);
    void DrawText(int x, int y, const std::string& text, const std::string& role);
    void DrawHorizontalLine(int x, int y, int length, char character, const std::string& role);
    void DrawVerticalLine(int x, int y, int length, char character, const std::string& role);

    void DrawObjects(const std::vector<RenderObject>& objects);


private:
    ThemeManager& themeManager;

    struct Cell
    {
        char character;
        std::string role;
    };

    std::vector<Cell> currentFrame;
    std::vector<Cell> previousFrame;

    int frameWidth = 0;
    int frameHeight = 0;
    bool frameActive = false;

    ThemeColor GetThemeColor(const std::string& role);
};