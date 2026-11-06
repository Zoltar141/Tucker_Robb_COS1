#pragma once
#include "Position.h"
#include "Boundary.h"

enum class BoundarySide
{
    None,
    Top,
    Bottom,
    Left,
    Right
};

class Collision
{
public:
    static bool CheckPosition(const Position& first, const Position& second);

    static bool CheckRectangle(const Position& position, const Position& rectanglePosition, int width, int height);

    static BoundarySide CheckBoundary(const Position& position, const Boundary& boundary);
};

