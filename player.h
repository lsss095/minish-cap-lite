#ifndef PLAYER_H
#define PLAYER_H

#include <QRect>

class Player
{
public:
    enum Direction
    {
        Down,
        Left,
        Right,
        Up,
    };

    enum Action
    {
        Idle,
        Walk,
        Roll
    };

    Player();

    int x() const;
    int y() const;

    Direction direction() const;
    Action action() const;

    int animationFrame() const;
    int actionRow() const;
    int currentActionFrameCount() const;

    bool isMoving() const;
    bool isActionLocked() const;

    void moveTo(int x, int y);
    void setDirection(Direction dir);
    void setAction(Action action);

    void startRoll();

    void startMoving();
    void stopMoving();
    void updateAnimation();

    QRect rect() const;

private:
    int m_x;
    int m_y;

    Direction m_direction;
    Action m_action;

    bool m_isMoving;
    bool m_actionLocked;

    int m_animationFrame;
    int m_animationCounter;
};

#endif