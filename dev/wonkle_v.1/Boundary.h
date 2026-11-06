#pragma once
#include "Position.h"
#include "Render.h"

class Boundary
{
public:
    Boundary(int width, int height, int left, int top, int right, int bottom);
    void Draw(Render& render, std::vector<RenderObject>& objects);

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