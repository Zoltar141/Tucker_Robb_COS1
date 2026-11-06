#include "Boundary.h"

Boundary::Boundary(int width, int height)
    : width(width), height(height)
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