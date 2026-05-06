#include "mainwindow.h"

#include <QPainter>
#include <QDebug>
#include <QDir>
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

    minish1Sheet = QPixmap(":/images/link_minish1_sheet_32.png");

    qDebug() << "Minish1动作表是否加载成功:" << !minish1Sheet.isNull();
    qDebug() << "Minish1动作表尺寸:" << minish1Sheet.width()
             << minish1Sheet.height();

    if (minish1Sheet.isNull()) {
        qDebug() << "Minish1动作表加载失败！请检查 resources.qrc 和图片路径。";
    }

    // 缩小形态贴图加载（素材待提供，暂时为空会用色块占位）
    miniShadowPixmap = QPixmap(":/images/link_mini_shadow.png");
    miniActionSheet = QPixmap(":/images/link_mini_actions.png");
    miniRollSheet = QPixmap(":/images/link_mini_roll.png");

    qDebug() << "缩小阴影是否加载成功:" << !miniShadowPixmap.isNull();
    qDebug() << "缩小动作表是否加载成功:" << !miniActionSheet.isNull();
    qDebug() << "缩小翻滚表是否加载成功:" << !miniRollSheet.isNull();

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

bool MainWindow::isNearStump() const
{
    QRect stumpRect(GameConfig::STUMP_X, GameConfig::STUMP_Y,
                    GameConfig::STUMP_W, GameConfig::STUMP_H);

    return player.rect().intersects(stumpRect);
}

bool MainWindow::isBlockedByStump() const
{
    QRect stumpRect(GameConfig::STUMP_X, GameConfig::STUMP_Y,
                    GameConfig::STUMP_W, GameConfig::STUMP_H);

    // 检测林克移动一步后的位置是否与树桩区域相交
    QRect nextRect(player.x(), player.y(),
                   player.bodyWidth(), player.bodyHeight());

    // 检测四个方向各走一步是否与树桩相交
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

    // 调试用：标记树桩区域
    painter.setBrush(QColor(0, 255, 0, 60));
    painter.drawRect(GameConfig::STUMP_X - camera.x(),
                     GameConfig::STUMP_Y - camera.y(),
                     GameConfig::STUMP_W, GameConfig::STUMP_H);

    painter.setPen(Qt::white);
    painter.drawText(5,
                     15,
                     QString("Player: (%1, %2) %3")
                         .arg(player.x())
                         .arg(player.y())
                         .arg(player.isMini() ? "[Mini]" : ""));

    if (player.isNearStumpState() && !player.isMini()) {
        painter.setPen(Qt::green);
        painter.drawText(5, 25, "L: Shrink");
    } else if (player.isNearStumpState() && player.isMini()) {
        painter.setPen(Qt::green);
        painter.drawText(5, 25, "L: Grow");
    }

    int shadowScreenX = player.x() - camera.x();
    int shadowScreenY = player.y() - camera.y();

    QRect playerRect = player.rect();

    // 缩小/放大动画期间：绘制过渡效果
    if (player.sizeState() == Player::Shrinking || player.sizeState() == Player::Growing) {
        int totalFrames = player.shrinkTotalFrames();
        int currentFrame = player.shrinkFrame();

        float progress = static_cast<float>(currentFrame) / totalFrames;

        float normalScale = 1.0f;
        float miniScale = 0.5f; // 缩小后约为原来一半

        float scale;
        if (player.sizeState() == Player::Shrinking) {
            scale = normalScale + (miniScale - normalScale) * progress;
        } else {
            scale = miniScale + (normalScale - miniScale) * progress;
        }

        // 绘制阴影
        int shadowW = static_cast<int>(GameConfig::SHADOW_DRAW_WIDTH * scale);
        int shadowH = static_cast<int>(GameConfig::SHADOW_DRAW_HEIGHT * scale);
        int shadowDrawX = shadowScreenX - (shadowW - player.bodyWidth()) / 2;
        int shadowDrawY = shadowScreenY - (shadowH - player.bodyHeight()) / 2 + 1;

        if (!shadowPixmap.isNull()) {
            painter.drawPixmap(shadowDrawX, shadowDrawY, shadowW, shadowH, shadowPixmap);
        }

        // 绘制角色（用正常动作表缩放）
        int drawW = static_cast<int>(GameConfig::PLAYER_DRAW_WIDTH * scale);
        int drawH = static_cast<int>(GameConfig::PLAYER_DRAW_HEIGHT * scale);
        int linkDrawX = shadowScreenX - (drawW - player.bodyWidth()) / 2;
        int linkDrawY = shadowScreenY - drawH + player.bodyHeight() + 5;

        if (!playerActionSheet.isNull()) {
            int frameX = player.animationFrame() * GameConfig::PLAYER_FRAME_WIDTH;
            int frameY = player.actionRow() * GameConfig::PLAYER_FRAME_HEIGHT;
            QPixmap currentFrame = playerActionSheet.copy(frameX, frameY,
                                                          GameConfig::PLAYER_FRAME_WIDTH,
                                                          GameConfig::PLAYER_FRAME_HEIGHT);
            painter.drawPixmap(linkDrawX, linkDrawY, drawW, drawH, currentFrame);
        }

        // 闪烁效果
        if (currentFrame % 3 == 0) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(255, 255, 255, 60));
            painter.drawEllipse(shadowScreenX - 2, shadowScreenY - 2,
                                player.bodyWidth() + 4, player.bodyHeight() + 4);
        }
    } else {
        drawPlayer(painter, shadowScreenX, shadowScreenY);
    }

    // 最后补画所有"遮挡玩家"的透视素材
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

void MainWindow::drawPlayer(QPainter &painter, int shadowScreenX, int shadowScreenY)
{
    bool mini = player.isMini();

    int bodyW = player.bodyWidth();
    int bodyH = player.bodyHeight();

    // 1. 阴影
    int shadowDrawW = mini ? GameConfig::MINI_SHADOW_DRAW_WIDTH : GameConfig::SHADOW_DRAW_WIDTH;
    int shadowDrawH = mini ? GameConfig::MINI_SHADOW_DRAW_HEIGHT : GameConfig::SHADOW_DRAW_HEIGHT;

    int shadowDrawX = shadowScreenX - (shadowDrawW - bodyW) / 2;
    int shadowDrawY = shadowScreenY - (shadowDrawH - bodyH) / 2 + 1;

    if (mini && !miniShadowPixmap.isNull()) {
        painter.drawPixmap(shadowDrawX, shadowDrawY, shadowDrawW, shadowDrawH, miniShadowPixmap);
    } else if (!mini && !shadowPixmap.isNull()) {
        painter.drawPixmap(shadowDrawX, shadowDrawY, shadowDrawW, shadowDrawH, shadowPixmap);
    } else {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 90));
        painter.drawEllipse(shadowScreenX, shadowScreenY, bodyW, bodyH);
    }

    // 2. 角色贴图
    int drawW = mini ? GameConfig::MINI_DRAW_WIDTH : GameConfig::PLAYER_DRAW_WIDTH;
    int drawH = mini ? GameConfig::MINI_DRAW_HEIGHT : GameConfig::PLAYER_DRAW_HEIGHT;
    int frameW = mini ? GameConfig::MINI_FRAME_WIDTH : GameConfig::PLAYER_FRAME_WIDTH;
    int frameH = mini ? GameConfig::MINI_FRAME_HEIGHT : GameConfig::PLAYER_FRAME_HEIGHT;

    int linkDrawX = shadowScreenX - (drawW - bodyW) / 2;
    int linkDrawY = shadowScreenY - drawH + bodyH + (mini ? 2 : 5);

    // 翻滚上/下树桩时的抛物线弧线偏移
    int arcOffset = player.rollArcOffset();
    linkDrawY += arcOffset;

    if (player.action() == Player::Roll) {
        QPixmap &sheet = mini ? miniRollSheet : playerRollSheet;
        if (!sheet.isNull()) {
            int frameX = player.animationFrame() * frameW;
            int frameY = rollDirectionRow(player.direction()) * frameH;
            QPixmap currentFrame = sheet.copy(frameX, frameY, frameW, frameH);
            painter.drawPixmap(linkDrawX, linkDrawY, drawW, drawH, currentFrame);
        } else {
            // 占位色块
            painter.setPen(Qt::black);
            painter.setBrush(mini ? QColor(100, 200, 100) : QColor(255, 220, 40));
            painter.drawRect(linkDrawX, linkDrawY, drawW, drawH);
        }
    } else if (player.action() == Player::Minish1) {
        if (!minish1Sheet.isNull()) {
            int frameX = player.animationFrame() * frameW;
            int frameY = 0;  // Minish1只有一行，四个朝向共用
            QPixmap currentFrame = minish1Sheet.copy(frameX, frameY, frameW, frameH);
            painter.drawPixmap(linkDrawX, linkDrawY, drawW, drawH, currentFrame);
        } else {
            painter.setPen(Qt::black);
            painter.setBrush(QColor(255, 220, 40));
            painter.drawRect(linkDrawX, linkDrawY, drawW, drawH);
        }
    } else {
        QPixmap &sheet = mini ? miniActionSheet : playerActionSheet;
        if (!sheet.isNull()) {
            int frameX = player.animationFrame() * frameW;
            int frameY = player.actionRow() * frameH;
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
    int mouseX = event->pos().x() / GameConfig::SCALE + camera.x();
    int mouseY = event->pos().y() / GameConfig::SCALE + camera.y();

    qDebug() << "点击地图坐标:" << mouseX << mouseY;
}

void MainWindow::updateGame()
{
    if (gameMap.isNull()) return;

    bool hasDirectionKey = keyW || keyA || keyS || keyD;

    // 走向树桩中心 / 平滑移出 / 缩小/放大动画期间：只更新动画帧，不接受操作
    if (player.sizeState() == Player::Approaching
        || player.sizeState() == Player::LeavingStump
        || player.sizeState() == Player::Shrinking
        || player.sizeState() == Player::Growing) {
        player.updateAnimation();
        drawScene();
        return;
    }

    // NearStump 状态：已到达树桩中心，等待按L
    if (player.isNearStumpState()) {
        // 更新Minish1动画帧
        player.updateAnimation();

        // 按方向键持续8帧：平滑移出树桩区域
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
            // 按L键且无方向键：执行缩小或放大
            if (keyL) {
                if (!player.isMini()) {
                    player.startShrink();
                } else {
                    player.startGrow();
                }
                keyL = false;
            }
            drawScene();
            return;
        }

    }

    // NearStump 状态下已处理完毕，不会再走到这里

    // 方向键 + L：翻滚
    if (keyL && hasDirectionKey && player.canRoll()) {
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

    // 正在翻滚时
    if (player.action() == Player::Roll) {
        int dx = 0;
        int dy = 0;
        int rollStep = player.isMini() ? GameConfig::MOVE_STEP + 1 : GameConfig::ROLL_STEP;

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
        // 根据按键按下顺序决定朝向，先按下的优先
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

        // 分离 X/Y 轴碰撞检测：斜向碰墙时保留未碰撞方向，实现滑墙效果
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

        // 按了方向键但坐标没变，且是被树桩挡住的，累加帧数
        if (player.x() == oldX && player.y() == oldY
            && isBlockedByStump()
            && player.sizeState() != Player::NearStump
            && player.sizeState() != Player::Approaching) {
            player.incStumpContactFrames();
            if (player.stumpContactFrames() >= 8) {
                bool toMini = !player.isMini();
                player.startApproach(toMini);
            }
        } else {
            player.resetStumpContactFrames();
        }

        player.updateAnimation();
    } else {
        player.stopMoving();
        player.resetStumpContactFrames();
    }

    drawScene();
}
