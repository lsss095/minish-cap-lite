#ifndef PLAYER_H
#define PLAYER_H

#include <QRect>

class Player
{
public:
    enum Direction
    {
        Up,
        Down,
        Left,
        Right
    };

    Player();

    int x() const;
    int y() const;
    Direction direction() const;

    void moveTo(int x, int y);
    void setDirection(Direction dir);

    QRect rect() const;

private:
    int m_x;
    int m_y;
    Direction m_direction;
};

#endif