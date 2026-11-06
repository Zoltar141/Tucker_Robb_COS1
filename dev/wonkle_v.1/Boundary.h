#pragma once
#include "Position.h"
#include "Render.h"

class Boundary
{
public:
    Boundary(int width, int height);
    void Draw(Render& render, std::vector<RenderObject>& objects);

    int GetWidth() const;
    int GetHeight() const;

private:
    int width;
    int height;
};