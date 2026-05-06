#include "mainwindow.h"

#include <QPainter>
#include <QDebug>
#include <QDir>
#include <QtGlobal>

#include "gameconfig.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    screenLabel(nullptr),
    keyW(false),
    keyA(false),
    keyS(false),
    keyD(false),
    keyL(false)
{
    setWindowTitle("Minish Cap Lite");

    setFixedSize(GameConfig::LOGIC_WIDTH * GameConfig::SCALE,
                 GameConfig::LOGIC_HEIGHT * GameConfig::SCALE);

    setFocusPolicy(Qt::StrongFocus);

    screenLabel = new QLabel(this);
    screenLabel->setGeometry(0,
                             0,
                             GameConfig::LOGIC_WIDTH * GameConfig::SCALE,
                             GameConfig::LOGIC_HEIGHT * GameConfig::SCALE);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    screenLabel->setFocusPolicy(Qt::NoFocus);

    qDebug() << "当前工作目录:" << QDir::currentPath();

    gameMap.load(":/images/minish_woods.png");

    shadowPixmap = QPixmap(":/images/link_shadow.png");

    qDebug() << "阴影是否加载成功:" << !shadowPixmap.isNull();
    qDebug() << "阴影尺寸:" << shadowPixmap.width() << shadowPixmap.height();

    if (shadowPixmap.isNull()) {
        qDebug() << "阴影图片加载失败！请检查 resources.qrc 和图片路径。";
    }

    playerActionSheet = QPixmap(":/images/link_actions_32.png");

    qDebug() << "完整人物动作表是否加载成功:" << !playerActionSheet.isNull();
    qDebug() << "完整人物动作表尺寸:" << playerActionSheet.width()
             << playerActionSheet.height();

    if (playerActionSheet.isNull()) {
        qDebug() << "完整人物动作表加载失败！请检查 resources.qrc 和图片路径。";
    }

    playerRollSheet = QPixmap(":/images/link_roll_32.png");

    qDebug() << "翻滚动作表是否加载成功:" << !playerRollSheet.isNull();
    qDebug() << "翻滚动作表尺寸:" << playerRollSheet.width()
             << playerRollSheet.height();

    if (playerRollSheet.isNull()) {
        qDebug() << "翻滚动作表加载失败！请检查 resources.qrc 和图片路径。";
    }

    initOverlayObjects();

    drawScene();

    connect(&gameTimer, &QTimer::timeout, this, &MainWindow::updateGame);
    gameTimer.start(24);
}

void MainWindow::initOverlayObjects()
{
    overlayObjects.emplace_back(":/images/wood0.png", 529, 4);
    overlayObjects.emplace_back(":/images/wood1.png", 545, 4);
    overlayObjects.emplace_back(":/images/wood1.png", 465, 100);
    overlayObjects.emplace_back(":/images/wood2.png", 529, 340);
    overlayObjects.emplace_back(":/images/wood2.png", 417, 324);
    overlayObjects.emplace_back(":/images/wood3.png", 241, 180);
}

static int rollDirectionRow(Player::Direction direction)
{
    if (direction == Player::Down) {
        return 0;
    } else if (direction == Player::Left) {
        return 1;
    } else if (direction == Player::Right) {
        return 2;
    } else if (direction == Player::Up) {
        return 3;
    }

    return 0;
}

void MainWindow::drawScene()
{
    if (gameMap.isNull()) return;

    camera.update(player.x(),
                  player.y(),
                  gameMap.width(),
                  gameMap.height());

    QPixmap frame(GameConfig::LOGIC_WIDTH,
                  GameConfig::LOGIC_HEIGHT);

    frame.fill(Qt::black);

    QPainter painter(&frame);

    QPixmap view = gameMap.pixmap().copy(camera.x(),
                                         camera.y(),
                                         GameConfig::LOGIC_WIDTH,
                                         GameConfig::LOGIC_HEIGHT);

    painter.drawPixmap(0, 0, view);

    // 调试用：把障碍区画成半透明红色
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 0, 0, 80));

    for (const QRect &rect : gameMap.blockedAreas()) {
        QRect screenRect(rect.x() - camera.x(),
                         rect.y() - camera.y(),
                         rect.width(),
                         rect.height());

        painter.drawRect(screenRect);
    }

    painter.setPen(Qt::white);
    painter.drawText(5,
                     15,
                     QString("Player: (%1, %2)")
                         .arg(player.x())
                         .arg(player.y()));

    int shadowScreenX = player.x() - camera.x();
    int shadowScreenY = player.y() - camera.y();

    QRect playerRect = player.rect();

    // 1. 先画阴影图片，也就是林克真正的位置
    int shadowDrawX = shadowScreenX
                      - (GameConfig::SHADOW_DRAW_WIDTH - GameConfig::PLAYER_BODY_WIDTH) / 2;

    int shadowDrawY = shadowScreenY
                      - (GameConfig::SHADOW_DRAW_HEIGHT - GameConfig::PLAYER_BODY_HEIGHT) / 2
                      + 1;

    if (!shadowPixmap.isNull()) {
        painter.drawPixmap(shadowDrawX,
                           shadowDrawY,
                           GameConfig::SHADOW_DRAW_WIDTH,
                           GameConfig::SHADOW_DRAW_HEIGHT,
                           shadowPixmap);
    } else {
        // 如果阴影图片加载失败，临时画椭圆，方便调试
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 90));
        painter.drawEllipse(shadowScreenX,
                            shadowScreenY,
                            GameConfig::PLAYER_BODY_WIDTH,
                            GameConfig::PLAYER_BODY_HEIGHT);
    }

    // 2. 再计算林克动作图的位置
    int linkDrawX = shadowScreenX
                    - (GameConfig::PLAYER_DRAW_WIDTH - GameConfig::PLAYER_BODY_WIDTH) / 2;

    int linkDrawY = shadowScreenY
                    - GameConfig::PLAYER_DRAW_HEIGHT
                    + GameConfig::PLAYER_BODY_HEIGHT
                    + 5;

    // 3. 再在影子上方画林克动作图
    if (player.action() == Player::Roll && !playerRollSheet.isNull()) {
        int frameX = player.animationFrame() * GameConfig::PLAYER_FRAME_WIDTH;
        int frameY = rollDirectionRow(player.direction()) * GameConfig::PLAYER_FRAME_HEIGHT;

        QPixmap currentFrame = playerRollSheet.copy(frameX,
                                                    frameY,
                                                    GameConfig::PLAYER_FRAME_WIDTH,
                                                    GameConfig::PLAYER_FRAME_HEIGHT);

        painter.drawPixmap(linkDrawX,
                           linkDrawY,
                           GameConfig::PLAYER_DRAW_WIDTH,
                           GameConfig::PLAYER_DRAW_HEIGHT,
                           currentFrame);
    } else if (!playerActionSheet.isNull()) {
        int frameX = player.animationFrame() * GameConfig::PLAYER_FRAME_WIDTH;
        int frameY = player.actionRow() * GameConfig::PLAYER_FRAME_HEIGHT;

        QPixmap currentFrame = playerActionSheet.copy(frameX,
                                                      frameY,
                                                      GameConfig::PLAYER_FRAME_WIDTH,
                                                      GameConfig::PLAYER_FRAME_HEIGHT);

        painter.drawPixmap(linkDrawX,
                           linkDrawY,
                           GameConfig::PLAYER_DRAW_WIDTH,
                           GameConfig::PLAYER_DRAW_HEIGHT,
                           currentFrame);
    } else {
        painter.setPen(Qt::black);
        painter.setBrush(QColor(255, 220, 40));
        painter.drawRect(linkDrawX,
                         linkDrawY,
                         GameConfig::PLAYER_DRAW_WIDTH,
                         GameConfig::PLAYER_DRAW_HEIGHT);
    }

    // 最后补画所有“遮挡玩家”的透视素材
    for (const OverlayObject &obj : overlayObjects) {
        if (obj.isNull()) continue;

        if (playerRect.intersects(obj.rect())) {
            int objScreenX = obj.position().x() - camera.x();
            int objScreenY = obj.position().y() - camera.y();

            painter.drawPixmap(objScreenX,
                               objScreenY,
                               obj.pixmap());
        }
    }

    painter.end();

    screenLabel->setPixmap(frame.scaled(GameConfig::LOGIC_WIDTH * GameConfig::SCALE,
                                        GameConfig::LOGIC_HEIGHT * GameConfig::SCALE,
                                        Qt::IgnoreAspectRatio,
                                        Qt::FastTransformation));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W) {
        keyW = true;
    } else if (event->key() == Qt::Key_A) {
        keyA = true;
    } else if (event->key() == Qt::Key_S) {
        keyS = true;
    } else if (event->key() == Qt::Key_D) {
        keyD = true;
    } else if (event->key() == Qt::Key_L) {
        keyL = true;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }

    if (event->key() == Qt::Key_W) {
        keyW = false;
    } else if (event->key() == Qt::Key_A) {
        keyA = false;
    } else if (event->key() == Qt::Key_S) {
        keyS = false;
    } else if (event->key() == Qt::Key_D) {
        keyD = false;
    } else if (event->key() == Qt::Key_L) {
        keyL = false;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int mouseX = event->pos().x() / GameConfig::SCALE + camera.x();
    int mouseY = event->pos().y() / GameConfig::SCALE + camera.y();

    qDebug() << "点击地图坐标:" << mouseX << mouseY;
}

void MainWindow::updateGame()
{
    if (gameMap.isNull()) return;

    bool hasDirectionKey = keyW || keyA || keyS || keyD;

    // 只有方向键 + L 同时按下，才触发翻滚
    if (keyL && hasDirectionKey && !player.isActionLocked()) {
        if (keyW) {
            player.setDirection(Player::Up);
        } else if (keyS) {
            player.setDirection(Player::Down);
        } else if (keyA) {
            player.setDirection(Player::Left);
        } else if (keyD) {
            player.setDirection(Player::Right);
        }

        player.startRoll();

        // 防止按住 L 连续触发翻滚
        keyL = false;
    }

    // 正在翻滚时：播放翻滚动作，并沿当前方向位移
    if (player.action() == Player::Roll) {
        int dx = 0;
        int dy = 0;

        if (player.direction() == Player::Up) {
            dy = -GameConfig::ROLL_STEP;
        } else if (player.direction() == Player::Down) {
            dy = GameConfig::ROLL_STEP;
        } else if (player.direction() == Player::Left) {
            dx = -GameConfig::ROLL_STEP;
        } else if (player.direction() == Player::Right) {
            dx = GameConfig::ROLL_STEP;
        }

        QRect nextRect(player.x() + dx,
                       player.y() + dy,
                       GameConfig::PLAYER_BODY_WIDTH,
                       GameConfig::PLAYER_BODY_HEIGHT);

        if (gameMap.canMoveTo(nextRect)) {
            player.moveTo(player.x() + dx,
                          player.y() + dy);
        }

        player.updateAnimation();
        drawScene();
        return;
    }

    // 如果有锁定动作，不执行普通走路
    if (player.isActionLocked()) {
        player.updateAnimation();
        drawScene();
        return;
    }

    int dx = 0;
    int dy = 0;

    if (keyW) {
        dy -= GameConfig::MOVE_STEP;
    }

    if (keyS) {
        dy += GameConfig::MOVE_STEP;
    }

    if (keyA) {
        dx -= GameConfig::MOVE_STEP;
    }

    if (keyD) {
        dx += GameConfig::MOVE_STEP;
    }

    bool isMoving = (dx != 0 || dy != 0);

    if (isMoving) {
        // 斜向移动时，优先显示上下方向
        if (dy < 0) {
            player.setDirection(Player::Up);
        } else if (dy > 0) {
            player.setDirection(Player::Down);
        } else if (dx < 0) {
            player.setDirection(Player::Left);
        } else if (dx > 0) {
            player.setDirection(Player::Right);
        }

        player.startMoving();

        QRect nextRect(player.x() + dx,
                       player.y() + dy,
                       GameConfig::PLAYER_BODY_WIDTH,
                       GameConfig::PLAYER_BODY_HEIGHT);

        if (gameMap.canMoveTo(nextRect)) {
            player.moveTo(player.x() + dx,
                          player.y() + dy);
        }

        player.updateAnimation();
    } else {
        player.stopMoving();
    }

    drawScene();
}