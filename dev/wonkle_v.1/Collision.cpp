#include "Collision.h"

bool Collision::CheckPosition(const Position& first, const Position& second)
{
    return first.GetX() == second.GetX() &&
        first.GetY() == second.GetY();
}

bool Collision::CheckRectangle(const Position& position, const Position& rectanglePosition, int width, int height)
{
    return position.GetX() >= rectanglePosition.GetX() &&
        position.GetX() < rectanglePosition.GetX() + width &&
        position.GetY() >= rectanglePosition.GetY() &&
        position.GetY() < rectanglePosition.GetY() + height;
}

BoundarySide Collision::CheckBoundary(const Position& position, const Boundary& boundary)
{
    if (position.GetX() <= boundary.GetLeft())
    {
        return BoundarySide::Left;
    }

    if (position.GetX() >= boundary.GetRight())
    {
        return BoundarySide::Right;
    }

    if (position.GetY() <= boundary.GetTop())
    {
        return BoundarySide::Top;
    }

    if (position.GetY() >= boundary.GetBottom())
    {
        return BoundarySide::Bottom;
    }

    return BoundarySide::None;
}