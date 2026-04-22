#include "mainwindow.h"
#include <QPainter>
#include <QDebug>
#include <QFile>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    screenLabel(nullptr),
    playerX(532),
    playerY(76),
    cameraX(0),
    cameraY(0)
{
    playerDir = Down;

    setWindowTitle("Minish Cap Lite");
    setFixedSize(LOGIC_WIDTH * SCALE, LOGIC_HEIGHT * SCALE);
    setFocusPolicy(Qt::StrongFocus);

    screenLabel = new QLabel(this);
    screenLabel->setGeometry(0, 0, LOGIC_WIDTH * SCALE, LOGIC_HEIGHT * SCALE);

    qDebug() << "当前工作目录:" << QDir::currentPath();

    QFile testFile(":/images/minish_woods.png");
    qDebug() << "资源文件存在吗:" << testFile.exists();

    fullMap = QPixmap(":/images/minish_woods.png");;
    qDebug() << "地图是否加载成功:" << !fullMap.isNull();
    qDebug() << "地图尺寸:" << fullMap.width() << fullMap.height();

    if (fullMap.isNull()) {
        qDebug() << "地图加载失败！请检查 resources.qrc 和图片路径。";
    }

    initBlockedAreas();
    drawScene();
}

void MainWindow::initBlockedAreas()
{
    blockedAreas.clear();
    blockedAreas.push_back(QRect(625, 0, 48, 388));
    blockedAreas.push_back(QRect(609, 84, 16, 208));
    blockedAreas.push_back(QRect(593, 100, 16, 176));
    blockedAreas.push_back(QRect(577, 132, 16, 128));
    blockedAreas.push_back(QRect(545, 356, 80, 32));
    blockedAreas.push_back(QRect(401, 340, 144, 48));
    blockedAreas.push_back(QRect(545, 164, 16, 112));
    blockedAreas.push_back(QRect(529, 132, 16, 160));
    blockedAreas.push_back(QRect(465, 116, 64, 208));
    blockedAreas.push_back(QRect(465, 20, 16, 48));
    blockedAreas.push_back(QRect(449, 0, 16, 308));
    blockedAreas.push_back(QRect(433, 0, 16, 276));
    blockedAreas.push_back(QRect(369, 0, 64, 180));
    blockedAreas.push_back(QRect(257, 0, 112, 84));
    blockedAreas.push_back(QRect(337, 116, 32, 64));
    blockedAreas.push_back(QRect(225, 0, 32, 68));
    blockedAreas.push_back(QRect(209, 0, 16, 20));
    blockedAreas.push_back(QRect(0, 0, 209, 68));
    blockedAreas.push_back(QRect(0, 68, 65, 320));
    blockedAreas.push_back(QRect(65, 116, 176, 160));
    blockedAreas.push_back(QRect(65, 372, 336,16));
    blockedAreas.push_back(QRect(65, 276, 16, 96));
    blockedAreas.push_back(QRect(81, 276, 16, 48));
    blockedAreas.push_back(QRect(97, 276, 32, 32));
    blockedAreas.push_back(QRect(241, 196, 160,112));
    blockedAreas.push_back(QRect(401, 212, 16, 64));
    blockedAreas.push_back(QRect(241, 308, 128, 48));
    blockedAreas.push_back(QRect(369, 308, 16, 16));
    blockedAreas.push_back(QRect(145, 276, 32, 32));
    blockedAreas.push_back(QRect(177, 276, 64, 64));
}

void MainWindow::updateCamera()
{
    cameraX = playerX - LOGIC_WIDTH / 2;
    cameraY = playerY - LOGIC_HEIGHT / 2;

    // 限制镜头不能超出地图边界
    if (cameraX < 0) cameraX = 0;
    if (cameraY < 0) cameraY = 0;

    if (cameraX + LOGIC_WIDTH > fullMap.width()) {
        cameraX = fullMap.width() - LOGIC_WIDTH;
    }
    if (cameraY + LOGIC_HEIGHT > fullMap.height()) {
        cameraY = fullMap.height() - LOGIC_HEIGHT;
    }

    if (cameraX < 0) cameraX = 0;
    if (cameraY < 0) cameraY = 0;
}
bool MainWindow::canMoveTo(int x, int y)
{
    if (fullMap.isNull()) return false;

    QRect playerRect(x, y, PLAYER_WIDTH, PLAYER_HEIGHT);

    // 不能走出地图
    if (x < 0 || y < 0 ||
        x + PLAYER_WIDTH > fullMap.width() ||
        y + PLAYER_HEIGHT > fullMap.height()) {
        return false;
    }

    // 不能撞到障碍区
    for (const QRect &rect : blockedAreas) {
        if (playerRect.intersects(rect)) {
            return false;
        }
    }

    return true;
}

void MainWindow::drawScene()
{
    if (fullMap.isNull()) return;

    updateCamera();

    QPixmap frame(LOGIC_WIDTH, LOGIC_HEIGHT);
    frame.fill(Qt::black);

    QPainter painter(&frame);

    // 从大地图中裁剪出 240x160 的视口
    QPixmap view = fullMap.copy(cameraX, cameraY, LOGIC_WIDTH, LOGIC_HEIGHT);
    painter.drawPixmap(0, 0, view);

    // 调试用：把障碍区画成半透明红色
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 0, 0, 80));

    for (const QRect &rect : blockedAreas) {
        QRect screenRect(rect.x() - cameraX,
                         rect.y() - cameraY,
                         rect.width(),
                         rect.height());
        painter.drawRect(screenRect);
    }

    painter.setPen(Qt::white);
    painter.drawText(5, 15, QString("Player: (%1, %2)").arg(playerX).arg(playerY));

    // 玩家在屏幕中的位置 = 玩家世界坐标 - 摄像机坐标
    int screenX = playerX - cameraX;
    int screenY = playerY - cameraY;

    // 先画一个简单的像素风角色占位块
    painter.setPen(Qt::black);
    painter.setBrush(QColor(255, 220, 40));   // 黄帽子风格
    painter.drawRect(screenX, screenY, PLAYER_WIDTH, PLAYER_HEIGHT);

    painter.setBrush(QColor(40, 120, 255));   // 蓝衣服风格
    painter.drawRect(screenX + 2, screenY + 4, PLAYER_WIDTH - 4, PLAYER_HEIGHT - 4);

    painter.end();

    // 放大到窗口大小，保留像素风
    screenLabel->setPixmap(frame.scaled(LOGIC_WIDTH * SCALE,
                                        LOGIC_HEIGHT * SCALE,
                                        Qt::IgnoreAspectRatio,
                                        Qt::FastTransformation));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (fullMap.isNull()) return;

    int newX = playerX;
    int newY = playerY;

    if (event->key() == Qt::Key_W) {
        newY -= MOVE_STEP;
        playerDir = Up;
    } else if (event->key() == Qt::Key_S) {
        newY += MOVE_STEP;
        playerDir = Down;
    } else if (event->key() == Qt::Key_A) {
        newX -= MOVE_STEP;
        playerDir = Left;
    } else if (event->key() == Qt::Key_D) {
        newX += MOVE_STEP;
        playerDir = Right;
    }

    if (canMoveTo(newX, newY)) {
        playerX = newX;
        playerY = newY;
    }

    drawScene();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int mouseX = event->pos().x() / SCALE + cameraX;
    int mouseY = event->pos().y() / SCALE + cameraY;

    qDebug() << "点击地图坐标:" << mouseX << mouseY;
}