#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QTimer>
#include <vector>

#include "player.h"
#include "gamemap.h"
#include "camera.h"
#include "overlayobject.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLabel *screenLabel;

    Player player;
    GameMap gameMap;
    Camera camera;

    QPixmap shadowPixmap;
    QPixmap playerActionSheet;
    QPixmap playerRollSheet;

    std::vector<OverlayObject> overlayObjects;

    QTimer gameTimer;

    bool keyW;
    bool keyA;
    bool keyS;
    bool keyD;
    bool keyL;

    void drawScene();
    void initOverlayObjects();
    void updateGame();
};

#endif