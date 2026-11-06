#include "Boundary.h"

Boundary::Boundary(int width, int height, int left, int top, int right, int bottom)
    : width(width), height(height), left(left), top(top), right(right), bottom(bottom)
{
}

void Boundary::Draw(Render& render, std::vector<RenderObject>& objects)
{
    // Top boundary.
    for (int i = 0; i < width; i++)
    {
        objects.push_back({ Position(i, 1), '_', ThemeRoles::Border });
    }

    // Bottom boundary.
    for (int i = 0; i < width; i++)
    {
        objects.push_back({ Position(i, height + 2), '-', ThemeRoles::Border });
    }

    // Left boundary.
    for (int j = 0; j < height; j++)
    {
        objects.push_back({ Position(0, j + 2), '|', ThemeRoles::Border });
    }

    // Right boundary.
    for (int j = 0; j < height; j++)
    {
        objects.push_back({ Position(width - 1, j + 2), '|', ThemeRoles::Border });
    }
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