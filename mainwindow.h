#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QTimer>
#include <vector>
#include <list>

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
    QPixmap minish1Sheet;

    // 缩小形态贴图（待用户提供素材后替换）
    QPixmap miniShadowPixmap;
    QPixmap miniActionSheet;
    QPixmap miniRollSheet;

    std::vector<OverlayObject> overlayObjects;

    QTimer gameTimer;

    bool keyW;
    bool keyA;
    bool keyS;
    bool keyD;
    bool keyL;

    std::list<Qt::Key> keyOrder;

    void pushKeyOrder(Qt::Key key);
    void removeKeyOrder(Qt::Key key);

    void drawScene();
    void drawPlayer(QPainter &painter, int shadowScreenX, int shadowScreenY);
    void initOverlayObjects();
    void updateGame();

    bool isNearStump() const;
    bool isBlockedByStump() const; // 林克紧贴树桩且下一步会被树桩阻挡
};

#endif
