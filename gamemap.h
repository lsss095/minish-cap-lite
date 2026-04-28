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

    bool canMoveTo(const QRect &playerRect) const;

private:
    QPixmap m_map;
    std::vector<QRect> m_blockedAreas;

    void initBlockedAreas();
};

#endif