#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QKeyEvent>
#include <QPixmap>
#include <QRect>
#include <vector>
#include <QMouseEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLabel *screenLabel;

    QPixmap fullMap;

    int playerX;
    int playerY;

    int cameraX;
    int cameraY;

    enum Direction { Up, Down, Left, Right };
    Direction playerDir;

    std::vector<QRect> blockedAreas;

    static const int LOGIC_WIDTH = 240;
    static const int LOGIC_HEIGHT = 160;
    static const int SCALE = 4;
    static const int PLAYER_WIDTH = 12;
    static const int PLAYER_HEIGHT = 12;
    static const int MOVE_STEP = 5;

    void updateCamera();
    void drawScene();
    void initBlockedAreas();
    bool canMoveTo(int x, int y);
};

#endif