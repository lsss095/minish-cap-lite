#ifndef CAMERA_H
#define CAMERA_H

class Camera
{
public:
    Camera();

    int x() const;
    int y() const;

    void update(int playerX, int playerY, int mapWidth, int mapHeight);

private:
    int m_x;
    int m_y;
};

#endif