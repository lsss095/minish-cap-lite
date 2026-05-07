#include "mainwindow.h"

#include <QPainter>
#include <QtGlobal>
#include <cmath>

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

    // 加载地图和贴图
    gameMap.load(":/images/minish_woods.png");

    shadowPixmap = QPixmap(":/images/link_shadow.png");
    playerActionSheet = QPixmap(":/images/link_actions_32.png");
    playerRollSheet = QPixmap(":/images/link_roll_32.png");
    minish1Sheet = QPixmap(":/images/link_minish1_sheet_32.png");

    // 缩小形态贴图
    miniShadowPixmap = QPixmap(":/images/mini_shadow.png");
    miniActionSheet = QPixmap(":/images/mini_link_action_sheet_7.png");
    miniRollSheet = QPixmap(":/images/mini_link_roll_7.png");

    initOverlayObjects();

    drawScene();

    connect(&gameTimer, &QTimer::timeout, this, &MainWindow::updateGame);
    gameTimer.start(24);
}

// 初始化透视遮挡物（树丛等，玩家可走入后方被遮挡）
void MainWindow::initOverlayObjects()
{
    overlayObjects.emplace_back(":/images/wood0.png", 529, 4);
    overlayObjects.emplace_back(":/images/wood1.png", 449, 100);
    overlayObjects.emplace_back(":/images/wood4.png", 529, 340);
    overlayObjects.emplace_back(":/images/wood2.png", 417, 324);
    overlayObjects.emplace_back(":/images/wood3.png", 236, 180);
    overlayObjects.emplace_back(":/images/wood5.png", 262, 356);
}

// 玩家是否与树桩区域相交
bool MainWindow::isNearStump() const
{
    QRect stumpRect(GameConfig::STUMP_X, GameConfig::STUMP_Y,
                    GameConfig::STUMP_W, GameConfig::STUMP_H);

    return player.rect().intersects(stumpRect);
}

// 玩家是否紧贴树桩（各方向走一步都会与树桩相交）
bool MainWindow::isBlockedByStump() const
{
    QRect stumpRect(GameConfig::STUMP_X, GameConfig::STUMP_Y,
                    GameConfig::STUMP_W, GameConfig::STUMP_H);

    QRect nextRect(player.x(), player.y(),
                   player.bodyWidth(), player.bodyHeight());

    QRect rightRect(nextRect.x() + GameConfig::MOVE_STEP, nextRect.y(),
                    nextRect.width(), nextRect.height());
    QRect leftRect(nextRect.x() - GameConfig::MOVE_STEP, nextRect.y(),
                   nextRect.width(), nextRect.height());
    QRect downRect(nextRect.x(), nextRect.y() + GameConfig::MOVE_STEP,
                   nextRect.width(), nextRect.height());
    QRect upRect(nextRect.x(), nextRect.y() - GameConfig::MOVE_STEP,
                 nextRect.width(), nextRect.height());

    return nextRect.intersects(stumpRect)
        || rightRect.intersects(stumpRect)
        || leftRect.intersects(stumpRect)
        || downRect.intersects(stumpRect)
        || upRect.intersects(stumpRect);
}

// 正常翻滚动作表行映射（Down=0, Left=1, Right=2, Up=3）
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

// 缩小动作表行映射（Up=0, Left=1, Right=2, Down=3）
static int miniActionRow(Player::Direction direction)
{
    if (direction == Player::Up) {
        return 0;
    } else if (direction == Player::Left) {
        return 1;
    } else if (direction == Player::Right) {
        return 2;
    } else if (direction == Player::Down) {
        return 3;
    }

    return 0;
}

// 绘制整个游戏画面
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

    // 绘制地图
    QPixmap view = gameMap.pixmap().copy(camera.x(),
                                         camera.y(),
                                         GameConfig::LOGIC_WIDTH,
                                         GameConfig::LOGIC_HEIGHT);

    painter.drawPixmap(0, 0, view);

    int shadowScreenX = player.x() - camera.x();
    int shadowScreenY = player.y() - camera.y();

    QRect playerRect = player.rect();

    drawPlayer(painter, shadowScreenX, shadowScreenY);

    // 绘制透视遮挡物
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

    // 白闪效果
    if (player.sizeState() == Player::FlashWhite) {
        // 缩小方向：16帧渐亮到全白
        const int shrinkFlashRiseFrames = 16;
        int currentFrame = player.flashFrame();
        int alpha = 255 * (currentFrame + 1) / shrinkFlashRiseFrames;
        if (alpha > 255) alpha = 255;
        if (alpha < 0) alpha = 0;
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, alpha));
        painter.drawRect(0, 0, GameConfig::LOGIC_WIDTH, GameConfig::LOGIC_HEIGHT);
    } else if (player.sizeState() == Player::FlashWhiteApproaching) {
        if (player.isMini()) {
            // 放大方向：全白覆盖
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(255, 255, 255, 255));
            painter.drawRect(0, 0, GameConfig::LOGIC_WIDTH, GameConfig::LOGIC_HEIGHT);
        }
        // 缩小方向：不闪白，正常显示翻滚
    }

    painter.end();

    // 缩放到窗口大小显示
    screenLabel->setPixmap(frame.scaled(GameConfig::LOGIC_WIDTH * GameConfig::SCALE,
                                        GameConfig::LOGIC_HEIGHT * GameConfig::SCALE,
                                        Qt::IgnoreAspectRatio,
                                        Qt::FastTransformation));
}

// 绘制玩家角色（阴影+贴图）
void MainWindow::drawPlayer(QPainter &painter, int shadowScreenX, int shadowScreenY)
{
    bool mini = player.isMini();

    int bodyW = player.bodyWidth();
    int bodyH = player.bodyHeight();

    // 阴影
    int shadowDrawW = mini ? GameConfig::MINI_SHADOW_DRAW_WIDTH : GameConfig::SHADOW_DRAW_WIDTH;
    int shadowDrawH = mini ? GameConfig::MINI_SHADOW_DRAW_HEIGHT : GameConfig::SHADOW_DRAW_HEIGHT;

    int shadowDrawX = shadowScreenX - (shadowDrawW - bodyW) / 2;
    int shadowDrawY = shadowScreenY - (shadowDrawH - bodyH) / 2 + 1;

    if (mini && !miniShadowPixmap.isNull()) {
        painter.drawPixmap(shadowDrawX, shadowDrawY, shadowDrawW, shadowDrawH, miniShadowPixmap);
    } else if (!mini && !shadowPixmap.isNull()) {
        painter.drawPixmap(shadowDrawX, shadowDrawY, shadowDrawW, shadowDrawH, shadowPixmap);
    } else {
        // 备用阴影
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 90));
        painter.drawEllipse(shadowScreenX, shadowScreenY, bodyW, bodyH);
    }

    // 角色贴图
    int drawW = mini ? GameConfig::MINI_DRAW_WIDTH : GameConfig::PLAYER_DRAW_WIDTH;
    int drawH = mini ? GameConfig::MINI_DRAW_HEIGHT : GameConfig::PLAYER_DRAW_HEIGHT;
    int frameW = mini ? GameConfig::MINI_FRAME_WIDTH : GameConfig::PLAYER_FRAME_WIDTH;
    int frameH = mini ? GameConfig::MINI_FRAME_HEIGHT : GameConfig::PLAYER_FRAME_HEIGHT;

    int linkDrawX = shadowScreenX - (drawW - bodyW) / 2;
    int linkDrawY = shadowScreenY - drawH + bodyH + (mini ? 1 : 5);

    // 翻滚抛物线偏移
    int arcOffset = player.rollArcOffset();
    linkDrawY += arcOffset;

    // 翻滚动作
    if (player.action() == Player::Roll) {
        QPixmap &sheet = mini ? miniRollSheet : playerRollSheet;
        if (!sheet.isNull()) {
            int frameX = player.animationFrame() * frameW;
            int row = mini ? miniActionRow(player.direction()) : rollDirectionRow(player.direction());
            int frameY = row * frameH;
            QPixmap currentFrame = sheet.copy(frameX, frameY, frameW, frameH);
            painter.drawPixmap(linkDrawX, linkDrawY, drawW, drawH, currentFrame);
        } else {
            // 占位色块
            painter.setPen(Qt::black);
            painter.setBrush(mini ? QColor(100, 200, 100) : QColor(255, 220, 40));
            painter.drawRect(linkDrawX, linkDrawY, drawW, drawH);
        }
    } else if (player.action() == Player::Minish1) {
        // 树桩等待动画
        if (!minish1Sheet.isNull()) {
            int frameX = player.animationFrame() * frameW;
            int frameY = 0;  // 只有一行
            QPixmap currentFrame = minish1Sheet.copy(frameX, frameY, frameW, frameH);
            painter.drawPixmap(linkDrawX, linkDrawY, drawW, drawH, currentFrame);
        } else {
            painter.setPen(Qt::black);
            painter.setBrush(QColor(255, 220, 40));
            painter.drawRect(linkDrawX, linkDrawY, drawW, drawH);
        }
    } else {
        // 站立/行走动作
        QPixmap &sheet = mini ? miniActionSheet : playerActionSheet;
        if (!sheet.isNull()) {
            int frameX = player.animationFrame() * frameW;
            int frameY;
            if (mini) {
                // 缩小动作表行映射
                frameY = miniActionRow(player.direction()) * frameH;
            } else {
                frameY = player.actionRow() * frameH;
            }
            QPixmap currentFrame = sheet.copy(frameX, frameY, frameW, frameH);
            painter.drawPixmap(linkDrawX, linkDrawY, drawW, drawH, currentFrame);
        } else {
            // 占位色块
            painter.setPen(Qt::black);
            painter.setBrush(mini ? QColor(100, 200, 100) : QColor(255, 220, 40));
            painter.drawRect(linkDrawX, linkDrawY, drawW, drawH);
        }
    }
}

// 按键按下顺序管理
void MainWindow::pushKeyOrder(Qt::Key key)
{
    removeKeyOrder(key);
    keyOrder.push_back(key);
}

void MainWindow::removeKeyOrder(Qt::Key key)
{
    keyOrder.remove(key);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_W) {
        keyW = true;
        pushKeyOrder(Qt::Key_W);
    } else if (event->key() == Qt::Key_A) {
        keyA = true;
        pushKeyOrder(Qt::Key_A);
    } else if (event->key() == Qt::Key_S) {
        keyS = true;
        pushKeyOrder(Qt::Key_S);
    } else if (event->key() == Qt::Key_D) {
        keyD = true;
        pushKeyOrder(Qt::Key_D);
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
        removeKeyOrder(Qt::Key_W);
    } else if (event->key() == Qt::Key_A) {
        keyA = false;
        removeKeyOrder(Qt::Key_A);
    } else if (event->key() == Qt::Key_S) {
        keyS = false;
        removeKeyOrder(Qt::Key_S);
    } else if (event->key() == Qt::Key_D) {
        keyD = false;
        removeKeyOrder(Qt::Key_D);
    } else if (event->key() == Qt::Key_L) {
        keyL = false;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

// 游戏主循环（每帧调用）
void MainWindow::updateGame()
{
    if (gameMap.isNull()) return;

    bool hasDirectionKey = keyW || keyA || keyS || keyD;

    // 白闪动画和走向树桩动画期间
    if (player.sizeState() == Player::Approaching
        || player.sizeState() == Player::LeavingStump
        || player.sizeState() == Player::FlashWhite
        || player.sizeState() == Player::FlashWhiteApproaching) {
        player.updateAnimation();
        drawScene();
        return;
    }

    // NearStump状态
    if (player.isNearStumpState()) {
        // 放大方向：自动触发白闪变大
        if (player.isMini()) {
            player.startGrowFlash();
            drawScene();
            return;
        }

        // 缩小方向：等待按方向键离开或按L缩小
        player.updateAnimation();

        // 按方向键持续8帧：移出树桩
        if (hasDirectionKey) {
            if (keyW) {
                player.setDirection(Player::Up);
            } else if (keyS) {
                player.setDirection(Player::Down);
            } else if (keyA) {
                player.setDirection(Player::Left);
            } else if (keyD) {
                player.setDirection(Player::Right);
            }
            player.incLeaveContactFrames();
            if (player.leaveContactFrames() >= 8) {
                player.startLeaveStump();
            }
            drawScene();
            return;
        } else {
            player.resetLeaveContactFrames();
            // 按L键：执行缩小
            if (keyL) {
                player.startShrink();
                keyL = false;
            }
            drawScene();
            return;
        }

    }

    // 方向键+L：翻滚
    if (keyL && hasDirectionKey && player.canRoll()
        && !(player.isShrunk() && isBlockedByStump())) {
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

        keyL = false;
    }

    // 翻滚中
    if (player.action() == Player::Roll) {
        int dx = 0;
        int dy = 0;
        int rollStep = player.isMini() ? GameConfig::MINI_ROLL_STEP : GameConfig::ROLL_STEP;

        if (player.direction() == Player::Up) {
            dy = -rollStep;
        } else if (player.direction() == Player::Down) {
            dy = rollStep;
        } else if (player.direction() == Player::Left) {
            dx = -rollStep;
        } else if (player.direction() == Player::Right) {
            dx = rollStep;
        }

        QRect nextRect(player.x() + dx,
                       player.y() + dy,
                       player.bodyWidth(),
                       player.bodyHeight());

        if (gameMap.canMoveTo(nextRect)) {
            player.moveTo(player.x() + dx,
                          player.y() + dy);
        }

        player.updateAnimation();
        drawScene();
        return;
    }

    // 动作锁定时不执行普通移动
    if (player.isActionLocked()) {
        player.updateAnimation();
        drawScene();
        return;
    }

    // 普通行走
    int dx = 0;
    int dy = 0;

    int moveStep = player.isMini() ? GameConfig::MINI_MOVE_STEP : GameConfig::MOVE_STEP;

    if (keyW) {
        dy -= moveStep;
    }

    if (keyS) {
        dy += moveStep;
    }

    if (keyA) {
        dx -= moveStep;
    }

    if (keyD) {
        dx += moveStep;
    }

    bool isMoving = (dx != 0 || dy != 0);

    if (isMoving) {
        // 按键按下顺序决定朝向
        for (Qt::Key key : keyOrder) {
            if (key == Qt::Key_W && keyW) {
                player.setDirection(Player::Up);
                break;
            } else if (key == Qt::Key_S && keyS) {
                player.setDirection(Player::Down);
                break;
            } else if (key == Qt::Key_A && keyA) {
                player.setDirection(Player::Left);
                               break;
            } else if (key == Qt::Key_D && keyD) {
                player.setDirection(Player::Right);
                break;
            }
        }

        player.startMoving();

        // 分离X/Y轴碰撞检测，实现滑墙效果
        int newX = player.x();
        int newY = player.y();

        if (dx != 0) {
            QRect nextRectX(player.x() + dx,
                            player.y(),
                            player.bodyWidth(),
                            player.bodyHeight());

            if (gameMap.canMoveTo(nextRectX)) {
                newX += dx;
            }
        }

        if (dy != 0) {
            QRect nextRectY(newX,
                            player.y() + dy,
                            player.bodyWidth(),
                            player.bodyHeight());

            if (gameMap.canMoveTo(nextRectY)) {
                newY += dy;
            }
        }

        int oldX = player.x();
        int oldY = player.y();

        if (newX != player.x() || newY != player.y()) {
            player.moveTo(newX, newY);
        }

        // 被树桩挡住累加帧数（仅Normal状态）
        if (!player.isShrunk()
            && player.x() == oldX && player.y() == oldY
            && isBlockedByStump()
            && player.sizeState() != Player::NearStump
            && player.sizeState() != Player::Approaching) {
            player.incStumpContactFrames();
            if (player.stumpContactFrames() >= 8) {
                player.startShrinkFlash();  // 白闪覆盖翻滚走向树桩中心
            }
        } else {
            player.resetStumpContactFrames();
        }

        player.updateAnimation();
    } else {
        player.stopMoving();
        player.resetStumpContactFrames();
    }

    // 缩小状态下：与树桩接触且按L键超过8帧，白闪变大
    if (player.isShrunk() && player.sizeState() == Player::Mini
        && isBlockedByStump() && keyL) {
        player.incLKeyFrames();
        if (player.lKeyFrames() >= 8) {
            player.startGrowFlash();
            player.resetLKeyFrames();
        }
    } else if (player.sizeState() == Player::Mini) {
        player.resetLKeyFrames();
    }

    drawScene();
}
