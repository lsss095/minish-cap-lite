#include "gamemap.h"
#include <QDebug>
#include <QFile>

GameMap::GameMap()
{
}

bool GameMap::load(const QString &path)
{
    QFile testFile(path);
    qDebug() << "资源文件存在吗:" << testFile.exists();

    m_map = QPixmap(path);

    qDebug() << "地图是否加载成功:" << !m_map.isNull();
    qDebug() << "地图尺寸:" << m_map.width() << m_map.height();

    if (m_map.isNull()) {
        qDebug() << "地图加载失败！请检查 resources.qrc 和图片路径。";
        return false;
    }

    initBlockedAreas();
    return true;
}

bool GameMap::isNull() const
{
    return m_map.isNull();
}

int GameMap::width() const
{
    return m_map.width();
}

int GameMap::height() const
{
    return m_map.height();
}

const QPixmap &GameMap::pixmap() const
{
    return m_map;
}

const std::vector<QRect> &GameMap::blockedAreas() const
{
    return m_blockedAreas;
}

bool GameMap::canMoveTo(const QRect &playerRect) const
{
    if (m_map.isNull()) return false;

    if (playerRect.x() < 0 ||
        playerRect.y() < 0 ||
        playerRect.right() > m_map.width() ||
        playerRect.bottom() > m_map.height()) {
        return false;
    }

    for (const QRect &rect : m_blockedAreas) {
        if (playerRect.intersects(rect)) {
            return false;
        }
    }

    return true;
}

void GameMap::initBlockedAreas()
{
    m_blockedAreas.clear();

    m_blockedAreas.push_back(QRect(625, 0, 48, 388));
    m_blockedAreas.push_back(QRect(609, 84, 16, 208));
    m_blockedAreas.push_back(QRect(593, 100, 16, 176));
    m_blockedAreas.push_back(QRect(577, 132, 16, 128));
    m_blockedAreas.push_back(QRect(545, 356, 80, 32));
    m_blockedAreas.push_back(QRect(401, 340, 144, 48));
    m_blockedAreas.push_back(QRect(545, 164, 16, 112));
    m_blockedAreas.push_back(QRect(529, 132, 16, 160));
    m_blockedAreas.push_back(QRect(465, 116, 64, 208));
    m_blockedAreas.push_back(QRect(465, 20, 16, 48));
    m_blockedAreas.push_back(QRect(449, 0, 16, 308));
    m_blockedAreas.push_back(QRect(433, 0, 16, 276));
    m_blockedAreas.push_back(QRect(369, 0, 64, 180));
    m_blockedAreas.push_back(QRect(257, 0, 112, 84));
    m_blockedAreas.push_back(QRect(337, 116, 32, 64));
    m_blockedAreas.push_back(QRect(225, 0, 32, 68));
    m_blockedAreas.push_back(QRect(209, 0, 16, 20));
    m_blockedAreas.push_back(QRect(0, 0, 209, 68));
    m_blockedAreas.push_back(QRect(0, 68, 65, 320));
    m_blockedAreas.push_back(QRect(65, 116, 176, 160));
    m_blockedAreas.push_back(QRect(65, 372, 336, 16));
    m_blockedAreas.push_back(QRect(65, 276, 16, 96));
    m_blockedAreas.push_back(QRect(81, 276, 16, 48));
    m_blockedAreas.push_back(QRect(97, 276, 32, 32));
    m_blockedAreas.push_back(QRect(241, 196, 160, 112));
    m_blockedAreas.push_back(QRect(401, 212, 16, 64));
    m_blockedAreas.push_back(QRect(241, 308, 128, 48));
    m_blockedAreas.push_back(QRect(369, 308, 16, 16));
    m_blockedAreas.push_back(QRect(145, 276, 32, 32));
    m_blockedAreas.push_back(QRect(177, 276, 64, 64));
    m_blockedAreas.push_back(QRect(545, 20, 32, 32));
}