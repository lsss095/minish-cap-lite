#include "player.h"
#include "gameconfig.h"

Player::Player()
    : m_x(532),
    m_y(76),
    m_direction(Down)
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

void Player::moveTo(int x, int y)
{
    m_x = x;
    m_y = y;
}

void Player::setDirection(Direction dir)
{
    m_direction = dir;
}

QRect Player::rect() const
{
    return QRect(m_x,
                 m_y,
                 GameConfig::PLAYER_WIDTH,
                 GameConfig::PLAYER_HEIGHT);
}