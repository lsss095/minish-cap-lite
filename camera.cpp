#include "camera.h"
#include "gameconfig.h"

Camera::Camera()
    : m_x(0),
    m_y(0)
{
}

int Camera::x() const
{
    return m_x;
}

int Camera::y() const
{
    return m_y;
}

void Camera::update(int playerX, int playerY, int mapWidth, int mapHeight)
{
    m_x = playerX - GameConfig::LOGIC_WIDTH / 2;
    m_y = playerY - GameConfig::LOGIC_HEIGHT / 2;

    if (m_x < 0) m_x = 0;
    if (m_y < 0) m_y = 0;

    if (m_x + GameConfig::LOGIC_WIDTH > mapWidth) {
        m_x = mapWidth - GameConfig::LOGIC_WIDTH;
    }

    if (m_y + GameConfig::LOGIC_HEIGHT > mapHeight) {
        m_y = mapHeight - GameConfig::LOGIC_HEIGHT;
    }

    if (m_x < 0) m_x = 0;
    if (m_y < 0) m_y = 0;
}