#include "Render.h"
#include <iostream>
#include <Windows.h>

// Connects the renderer to ThemeManager.
Render::Render(ThemeManager& themeManager)
    : themeManager(themeManager)
{
}

// Retrieves a color from the active theme using its semantic role.
ThemeColor Render::GetThemeColor(const std::string& role)
{
    Theme* activeTheme = themeManager.GetActiveTheme();
    return activeTheme->GetColor(role);
}

// Moves the console cursor to the specified position.
void Render::SetCursorPosition(int x, int y)
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position;

    position.X = x;
    position.Y = y;

    SetConsoleCursorPosition(console, position);
}

// Applies the active theme's background color.
void Render::SetBackground()
{
    ThemeColor background = GetThemeColor(ThemeRoles::Background);
    Colors::SetBackground(background.background);
}

// Starts a new frame.
void Render::BeginFrame(int width, int height, const std::string& backgroundRole, int worldOffsetY)
{
    frameWidth = width;
    frameHeight = height;
    this->worldOffsetY = worldOffsetY;

    frameActive = true;

    currentFrame.assign(width * height, { ' ', backgroundRole });
}
// Writes only characters that changed.
void Render::PresentFrame()
{
    if (!frameActive)
    {
        return;
    }

    frameActive = false;

    for (int j = 0; j < frameHeight; j++)
    {
        int i = 0;

        while (i < frameWidth)
        {
            int index = j * frameWidth + i;

            bool changed = previousFrame.size() != currentFrame.size() ||
                currentFrame[index].character != previousFrame[index].character ||
                currentFrame[index].role != previousFrame[index].role;

            if (!changed)
            {
                i++;
                continue;
            }

            int startX = i;
            std::string text;
            std::string role = currentFrame[index].role;

            // Collect consecutive changed characters with the same role.
            while (i < frameWidth)
            {
                index = j * frameWidth + i;

                changed = previousFrame.size() != currentFrame.size() ||
                    currentFrame[index].character != previousFrame[index].character ||
                    currentFrame[index].role != previousFrame[index].role;

                if (!changed || currentFrame[index].role != role)
                {
                    break;
                }

                text += currentFrame[index].character;
                i++;
            }

            ThemeColor color = GetThemeColor(role);
            Colors::SetBackground(color.background);
            DrawText(startX, j, text, role);
        }
    }

    previousFrame = currentFrame;
}

// Draws a character.
void Render::DrawCharacter(int x, int y, char character, const std::string& role)
{
    if (frameActive)
    {
        if (x >= 0 && x < frameWidth && y >= 0 && y < frameHeight)
        {
            int index = y * frameWidth + x;
            currentFrame[index] = { character, role };
        }

        return;
    }

    ThemeColor color = GetThemeColor(role);

    SetCursorPosition(x, y);
    Colors::Print(character, color.foreground);
}

// Draws text.
void Render::DrawText(int x, int y, const std::string& text, const std::string& role)
{
    if (frameActive)
    {
        int currentX = x;
        int currentY = y;

        for (char character : text)
        {
            if (character == '\t')
            {
                currentX = ((currentX / 8) + 1) * 8;
            }
            else if (character == '\n')
            {
                currentX = x;
                currentY++;
            }
            else
            {
                DrawCharacter(currentX, currentY, character, role);
                currentX++;
            }
        }

        return;
    }

    ThemeColor color = GetThemeColor(role);

    SetCursorPosition(x, y);
    Colors::Print(text, color.foreground);
}

// Draws a horizontal line.
void Render::DrawHorizontalLine(int x, int y, int length, char character, const std::string& role)
{
    for (int i = 0; i < length; i++)
    {
        DrawCharacter(x + i, y, character, role);
    }
}

// Draws a vertical line.
void Render::DrawVerticalLine(int x, int y, int length, char character, const std::string& role)
{
    for (int i = 0; i < length; i++)
    {
        DrawCharacter(x, y + i, character, role);
    }
}

// Draws a collection of game objects into the current frame.
void Render::DrawObjects(const std::vector<RenderObject>& objects)
{
    for (int i = 0; i < objects.size(); i++)
    {
        DrawCharacter(
            objects[i].position.GetX(),
            objects[i].position.GetY() + worldOffsetY,
            objects[i].character,
            objects[i].role
        );
    }
}