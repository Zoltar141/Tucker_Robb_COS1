#include "Boundary.h"

constexpr int TOP_BOUNDARY_OFFSET = -1;

Boundary::Boundary(int width, int height, int left, int top, int right, int bottom)
    : width(width), height(height), left(left), top(top), right(right), bottom(bottom)
{
}

void Boundary::Draw(Render& render, std::vector<RenderObject>& objects, char topCharacter,
    char bottomCharacter, char verticalCharacter, const std::string& role)
{
    // Top boundary.
    for (int i = 0; i < width; i++)
        objects.push_back({ Position(i, TOP_BOUNDARY_OFFSET), topCharacter, role });

    // Bottom boundary.
    for (int i = 0; i < width; i++)
        objects.push_back({ Position(i, height), bottomCharacter, role });

    // Left boundary.
    for (int j = 0; j < height; j++)
        objects.push_back({ Position(0, j), verticalCharacter, role });

    // Right boundary.
    for (int j = 0; j < height; j++)
        objects.push_back({ Position(width - 1, j), verticalCharacter, role });
}

int Boundary::GetWidth() const
{
    return width;
}

int Boundary::GetHeight() const
{
    return height;
}

int Boundary::GetLeft() const 
{ 
    return left; 
}

int Boundary::GetTop() const 
{ 
    return top; 
}

int Boundary::GetRight() const 
{ 
    return right; 
}

int Boundary::GetBottom() const 
{ 
    return bottom; 
}