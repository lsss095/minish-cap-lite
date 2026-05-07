// 主窗口类：管理游戏循环、输入、渲染

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

    // 正常形态贴图
    QPixmap shadowPixmap;
    QPixmap playerActionSheet;  // 站立/行走动作表
    QPixmap playerRollSheet;    // 翻滚动作表
    QPixmap minish1Sheet;       // 树桩等待动画表

    // 缩小形态贴图
    QPixmap miniShadowPixmap;
    QPixmap miniActionSheet;  // 缩小站立/行走动作表
    QPixmap miniRollSheet;    // 缩小翻滚动作表

    std::vector<OverlayObject> overlayObjects; // 透视遮挡物

    QTimer gameTimer;

    // 按键状态
    bool keyW;
    bool keyA;
    bool keyS;
    bool keyD;
    bool keyL;

    std::list<Qt::Key> keyOrder; // 按键按下顺序

    void pushKeyOrder(Qt::Key key);
    void removeKeyOrder(Qt::Key key);

    void drawScene();
    void drawPlayer(QPainter &painter, int shadowScreenX, int shadowScreenY);
    void initOverlayObjects();
    void updateGame();

    bool isNearStump() const;
    bool isBlockedByStump() const; // 是否被树桩阻挡
};

#endif
