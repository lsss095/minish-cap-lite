#include "player.h"
#include "gameconfig.h"

Player::Player()
    : m_x(532),
    m_y(76),
    m_direction(Down),
    m_action(Idle),
    m_isMoving(false),
    m_actionLocked(false),
    m_animationFrame(0),
    m_animationCounter(0)
{
}

int Player::x() const
{
    return m_x;
}

int Player::y() const
{
    return m_y;
}

Player::Direction Player::direction() const
{
    return m_direction;
}

Player::Action Player::action() const
{
    return m_action;
}

int Player::animationFrame() const
{
    return m_animationFrame;
}

bool Player::isMoving() const
{
    return m_isMoving;
}

bool Player::isActionLocked() const
{
    return m_actionLocked;
}

void Player::moveTo(int x, int y)
{
    m_x = x;
    m_y = y;
}

void Player::setDirection(Direction dir)
{
    if (m_actionLocked) return;

    m_direction = dir;
}

void Player::setAction(Action action)
{
    if (m_action != action) {
        m_action = action;
        m_animationFrame = 0;
        m_animationCounter = 0;
    }
}

void Player::startMoving()
{
    if (m_actionLocked) return;

    m_isMoving = true;
    setAction(Walk);
}

void Player::stopMoving()
{
    if (m_actionLocked) return;

    m_isMoving = false;
    setAction(Idle);
}

void Player::startRoll()
{
    if (m_actionLocked) return;

    m_isMoving = false;
    m_actionLocked = true;
    setAction(Roll);
}

void Player::updateAnimation()
{
    m_animationCounter++;

    int speed = GameConfig::ANIMATION_SPEED;
    int maxFrameCount = currentActionFrameCount();

    if (m_action == Roll) {
        speed = GameConfig::ROLL_ANIMATION_SPEED;
    }

    if (m_animationCounter < speed) {
        return;
    }

    m_animationCounter = 0;
    m_animationFrame++;

    if (m_animationFrame >= maxFrameCount) {
        m_animationFrame = 0;

        if (m_action == Roll) {
            m_actionLocked = false;
            setAction(Idle);
        }
    }
}

int Player::actionRow() const
{
    int directionIndex = 0;

    if (m_direction == Down) {
        directionIndex = 0;
    } else if (m_direction == Left) {
        directionIndex = 1;
    } else if (m_direction == Right) {
        directionIndex = 2;
    } else if (m_direction == Up) {
        directionIndex = 3;
    }

    if (m_action == Idle) {
        return 0 + directionIndex;
    }

    if (m_action == Walk) {
        return 4 + directionIndex;
    }

    // Roll 不在原动作表里，这里不用管
    return directionIndex;
}

QRect Player::rect() const
{
    return QRect(m_x,
                 m_y,
                 GameConfig::PLAYER_BODY_WIDTH,
                 GameConfig::PLAYER_BODY_HEIGHT);
}

int Player::currentActionFrameCount() const
{
    if (m_action == Roll) {
        if (m_direction == Left || m_direction == Right) {
            return GameConfig::ROLL_SIDE_FRAME_COUNT;
        }

        return GameConfig::ROLL_FRAME_COUNT;
    }

    return GameConfig::PLAYER_FRAME_COUNT;
}