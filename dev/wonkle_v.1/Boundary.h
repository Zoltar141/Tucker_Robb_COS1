#pragma once
#include "Position.h"
#include "Render.h"

#include <string>
#include <vector>

class Boundary
{
public:
    Boundary(int width, int height, int left, int top, int right, int bottom);

    void Draw(Render& render, std::vector<RenderObject>& objects, char topCharacter,
        char bottomCharacter, char verticalCharacter, const std::string& role);

    int GetWidth() const;
    int GetHeight() const;
    int GetLeft() const;
    int GetTop() const;
    int GetRight() const;
    int GetBottom() const;

private:
    int width;
    int height;

    int left;
    int top;
    int right;
    int bottom;
};