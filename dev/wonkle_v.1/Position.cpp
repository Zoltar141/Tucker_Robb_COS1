#include "Position.h"

Position::Position(int x, int y)
    : x(x), y(y)
{
}

int Position::GetX() const
{
    return x;
}

int Position::GetY() const
{
    return y;
}

void Position::SetX(int x)
{
    this->x = x;
}

void Position::SetY(int y)
{
    this->y = y;
}

void Position::Move(int x, int y)
{
    this->x += x;
    this->y += y;
}