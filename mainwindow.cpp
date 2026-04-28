#include "mainwindow.h"

#include <QPainter>
#include <QDebug>
#include <QDir>

#include "gameconfig.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    screenLabel(nullptr)
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

    qDebug() << "当前工作目录:" << QDir::currentPath();

    gameMap.load(":/images/minish_woods.png");

    initOverlayObjects();

    drawScene();
}

void MainWindow::initOverlayObjects()
{
    overlayObjects.emplace_back(":/images/wood.png", 529, 4);
    overlayObjects.emplace_back(":/images/wood1.png", 465, 100);
    overlayObjects.emplace_back(":/images/wood2.png", 529, 340);
    overlayObjects.emplace_back(":/images/wood2.png", 417, 324);
    overlayObjects.emplace_back(":/images/wood3.png", 241, 180);
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

    int screenX = player.x() - camera.x();
    int screenY = player.y() - camera.y();

    QRect playerRect = player.rect();

    // 画角色
    painter.setPen(Qt::black);
    painter.setBrush(QColor(255, 220, 40));
    painter.drawRect(screenX,
                     screenY,
                     GameConfig::PLAYER_WIDTH,
                     GameConfig::PLAYER_HEIGHT);

    painter.setBrush(QColor(40, 120, 255));
    painter.drawRect(screenX + 2,
                     screenY + 4,
                     GameConfig::PLAYER_WIDTH - 4,
                     GameConfig::PLAYER_HEIGHT - 4);

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
    if (gameMap.isNull()) return;

    int newX = player.x();
    int newY = player.y();

    if (event->key() == Qt::Key_W) {
        newY -= GameConfig::MOVE_STEP;
        player.setDirection(Player::Up);
    } else if (event->key() == Qt::Key_S) {
        newY += GameConfig::MOVE_STEP;
        player.setDirection(Player::Down);
    } else if (event->key() == Qt::Key_A) {
        newX -= GameConfig::MOVE_STEP;
        player.setDirection(Player::Left);
    } else if (event->key() == Qt::Key_D) {
        newX += GameConfig::MOVE_STEP;
        player.setDirection(Player::Right);
    }

    QRect nextRect(newX,
                   newY,
                   GameConfig::PLAYER_WIDTH,
                   GameConfig::PLAYER_HEIGHT);

    if (gameMap.canMoveTo(nextRect)) {
        player.moveTo(newX, newY);
    }

    drawScene();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int mouseX = event->pos().x() / GameConfig::SCALE + camera.x();
    int mouseY = event->pos().y() / GameConfig::SCALE + camera.y();

    qDebug() << "点击地图坐标:" << mouseX << mouseY;
}