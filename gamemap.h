// 地图类：管理地图贴图和碰撞区域

#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QPixmap>
#include <QRect>
#include <QString>
#include <vector>

class GameMap
{
public:
    GameMap();

    bool load(const QString &path);

    bool isNull() const;
    int width() const;
    int height() const;

    const QPixmap &pixmap() const;
    const std::vector<QRect> &blockedAreas() const;

    bool canMoveTo(const QRect &playerRect) const; // 检测是否可移动到指定区域

private:
    QPixmap m_map;
    std::vector<QRect> m_blockedAreas; // 碰撞阻挡区域

    void initBlockedAreas();
};

#endif
