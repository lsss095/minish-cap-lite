#ifndef OVERLAYOBJECT_H
#define OVERLAYOBJECT_H

#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <QString>

class OverlayObject
{
public:
    OverlayObject(const QString &path, int x, int y);

    const QPixmap &pixmap() const;
    QPoint position() const;
    QRect rect() const;

    bool isNull() const;

private:
    QPixmap m_pixmap;
    QPoint m_position;
};

#endif