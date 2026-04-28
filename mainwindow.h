#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>
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
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLabel *screenLabel;

    Player player;
    GameMap gameMap;
    Camera camera;

    std::vector<OverlayObject> overlayObjects;

    void drawScene();
    void initOverlayObjects();
};

#endif