#include "overlayobject.h"
#include <QDebug>

OverlayObject::OverlayObject(const QString &path, int x, int y)
    : m_pixmap(path),
    m_position(x, y)
{
    qDebug() << "加载透视素材:" << path
             << "成功:" << !m_pixmap.isNull()
             << "尺寸:" << m_pixmap.width() << m_pixmap.height()
             << "位置:" << m_position;
}

const QPixmap &OverlayObject::pixmap() const
{
    return m_pixmap;
}

QPoint OverlayObject::position() const
{
    return m_position;
}

QRect OverlayObject::rect() const
{
    return QRect(m_position.x(),
                 m_position.y(),
                 m_pixmap.width(),
                 m_pixmap.height());
}

bool OverlayObject::isNull() const
{
    return m_pixmap.isNull();
}