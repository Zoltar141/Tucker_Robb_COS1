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

bool Collision::CheckBoundary(const Position& position, const Boundary& boundary)
{
    return position.GetX() <= boundary.GetLeft() ||
        position.GetX() >= boundary.GetRight() ||
        position.GetY() <= boundary.GetTop() ||
        position.GetY() >= boundary.GetBottom();
}