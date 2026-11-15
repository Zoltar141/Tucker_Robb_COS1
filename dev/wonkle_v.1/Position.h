#pragma once
class Position
{
public:
    Position(int x, int y);

    int GetX() const;
    int GetY() const;

    void SetX(int x);
    void SetY(int y);

    void Move(int x, int y);

private:
    int x;
    int y;
};
